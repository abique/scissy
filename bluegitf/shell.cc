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

#include "acl.pb.h"

enum Action
{
  kGitUploadPack,
  kGitReceivePack,
  kGitUploadArchive,
};

int64_t & USER_ID = *mimosa::options::addOption<int64_t>("", "user-id", "the user id", 1);
std::string & SOCKET_PATH = *mimosa::options::addOption<std::string>(
  "", "socket-path", "path to bluegitfd's unix socket", "");

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
  bluegitf::pb::Acl::Client acl(channel);

  // prepare the request
  auto request = new bluegitf::pb::UserRepo;
  request->set_user_id(user_id);
  request->set_repo(repo_name);

  // make the call
  auto call = acl.get(request);
  call->wait(); // wait for the call to complete

  if (call->isCanceled())
    return false;

  *repo_path = call->response().path();

  switch (call->response().right())
  {
  case bluegitf::pb::kNone:
    return false;

  case bluegitf::pb::kWrite:
    return true;

  case bluegitf::pb::kRead:
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

  execlp(pathes[action], pathes[action], repo_path.c_str());
  return false;
}

int main(int argc, char ** argv)
{
  mimosa::init(argc, argv);

  const char * cmd = getenv("SSH_ORIGINAL_COMMAND");
  Action       action;
  std::string  repo_name;

  if (!parseCmd(cmd, &action, &repo_name))
  {
    fprintf(stderr, "failed to parse command: %s\n", cmd);
    return 1;
  }

  std::string repo_path;
  if (!checkAcl(USER_ID, action, repo_name, &repo_path))
  {
    fprintf(stderr, "operation forbidden\n");
    return 1;
  }

  runAction(action, repo_path);

  mimosa::deinit();
  return 0;
}
