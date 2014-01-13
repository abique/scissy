#include <cstdio>
#include <cstring>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>

#include <mimosa/init.hh>
#include <mimosa/options/options.hh>
#include <mimosa/net/connect.hh>
#include <mimosa/stream/fd-stream.hh>

#include "service.pb.h"
#include "log.hh"

enum Action
{
  kGitUploadPack,
  kGitReceivePack,
  kGitUploadArchive,
};

auto & USER_ID = *mimosa::options::addOption<int64_t>("", "user-id", "user id", 0);
auto & SOCKET_PATH = *mimosa::options::addOption<std::string>(
  "", "socket", "unix socket to scissyd", "");

static bool parseCmd(const char *  cmd,
                     Action *      action,
                     std::string * repo_name)
{
  if (!strncmp(cmd, "git-upload-pack ", 16))
  {
    *action = kGitUploadPack;
    cmd += 16;
  }
  else if (!strncmp(cmd, "git-upload-archive ", 19))
  {
    *action = kGitUploadArchive;
    cmd += 19;
  }
  else if (!strncmp(cmd, "git-receive-pack ", 17))
  {
    *action = kGitReceivePack;
    cmd += 17;
  }
  else
    return false;

  repo_name->reserve(strlen(cmd));

  for (bool in_quote = false; *cmd; ++cmd)
  {
    if (in_quote && *cmd == '\'')
      in_quote = false;
    else if (in_quote)
      repo_name->append(cmd, 1);
    else if (*cmd == '\'')
      in_quote = true;
    else if (*cmd == '\\' && cmd[1] != '\0')
    {
      ++cmd;
      repo_name->append(cmd, 1);
    }
    else if (*cmd == '\\' && cmd[1] == '\0')
      break;
    else
      repo_name->append(cmd, 1);
  }

  return true;
}

bool checkAcl(int64_t             user_id,
              Action              action,
              const std::string & repo_name,
              std::string *       repo_path)
{
  // connect to the socket
  int fd = mimosa::net::connectToUnixSocket(SOCKET_PATH);
  if (fd < 0)
    return false;

  // create a channel
  mimosa::rpc::Channel::Ptr channel(
    new mimosa::rpc::Channel(
      new mimosa::stream::FdStream(fd)));
  channel->start();

  // create a service stub
  scissy::pb::Service::Client service(channel);

  // prepare the request
  auto request = new scissy::pb::ShellControl;
  request->set_repo_name(repo_name);
  request->set_user_id(user_id);

  // make the call
  auto call = service.shellControl(request);
  call->wait();

  // check response
  if (call->isCanceled() ||
      call->response().status() == scissy::pb::kFailed)
    return false;

  *repo_path = call->response().repo_path();
  if (call->response().is_public() && action == kGitReceivePack)
    return true;

  switch (call->response().role())
  {
  case scissy::pb::kNone:
    return false;

  case scissy::pb::kWriter:
  case scissy::pb::kOwner:
    return true;

  case scissy::pb::kReader:
    if (action == kGitReceivePack)
      return false;

    assert(action == kGitUploadPack || action == kGitUploadArchive);
    return true;

  default:
    return false;
  }
}

bool runAction(Action action, const std::string & repo_path)
{
  static const char *pathes[] = {
    "git-upload-pack",
    "git-receive-pack",
    "git-upload-archive"
  };

  execlp(pathes[action], pathes[action], repo_path.c_str(), nullptr);
  return false;
}

int main(int argc, char ** argv)
{
  mimosa::init(argc, argv);

  const char * cmd = getenv("SSH_ORIGINAL_COMMAND");
  Action       action;
  std::string  repo_name;

  if (!parseCmd(cmd, &action, &repo_name)) {
    scissy::log->error("failed to parse command: %s", cmd);
    return 1;
  }

  std::string repo_path;
  if (!checkAcl(USER_ID, action, repo_name, &repo_path)) {
    scissy::log->error("operation forbidden");
    return 1;
  }

  runAction(action, repo_path);

  mimosa::deinit();
  return 0;
}
