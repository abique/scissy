#include <cerrno>

#include <mimosa/init.hh>
#include <mimosa/log/log.hh>
#include <mimosa/options/options.hh>
#include <mimosa/http/server.hh>
#include <mimosa/http/dispatch-handler.hh>
#include <mimosa/http/fs-handler.hh>
#include <mimosa/http/log-handler.hh>

#include "config.hh"
#include "db.hh"
#include "repositories.hh"

// handlers
#include "clear-cache-handler.hh"
#include "groups-handler.hh"
#include "login-handler.hh"
#include "logout-handler.hh"
#include "register-handler.hh"
#include "root-handler.hh"
#include "new-repo-handler.hh"
#include "new-group-handler.hh"
#include "repos-handler.hh"
#include "settings-handler.hh"

uint16_t & PORT = *mimosa::options::addOption<uint16_t>("", "port", "the port to use", 19042);
bool & UNSECURE = *mimosa::options::addSwitch("", "unsecure", "disable https, usefull for dev");

int main(int argc, char ** argv)
{
  sqlite3_initialize();
  mimosa::init(argc, argv);
  bluegitf::Config::instance();
  bluegitf::Db::instance();

  auto dispatch = new mimosa::http::DispatchHandler;
  dispatch->registerHandler(
    "/css/*", new mimosa::http::FsHandler(
      bluegitf::Config::instance().cssDir(), 1, true));
  dispatch->registerHandler("/", new bluegitf::RootHandler);
  dispatch->registerHandler("/clear-cache", new bluegitf::ClearCacheHandler);
  dispatch->registerHandler("/register", new bluegitf::RegisterHandler);
  dispatch->registerHandler("/login", new bluegitf::LoginHandler);
  dispatch->registerHandler("/logout", new bluegitf::LogoutHandler);
  dispatch->registerHandler("/new-repo", new bluegitf::NewRepoHandler);
  dispatch->registerHandler("/new-group", new bluegitf::NewGroupHandler);
  dispatch->registerHandler("/repos", new bluegitf::ReposHandler);
  dispatch->registerHandler("/settings", new bluegitf::SettingsHandler);
  dispatch->registerHandler("/groups", new bluegitf::GroupsHandler);

  auto log_handler = new mimosa::http::LogHandler;
  log_handler->setHandler(dispatch);

  mimosa::http::Server::Ptr server(new mimosa::http::Server);
  server->setHandler(log_handler);
  if (!UNSECURE)
    server->setSecure(bluegitf::Config::instance().certPem(),
                      bluegitf::Config::instance().keyPem());

  if (!server->listenInet4(PORT))
  {
    MIMOSA_LOG(Fatal, NULL, "failed to listen on the port %d: %s",
               PORT, ::strerror(errno));
    return 1;
  }

  while (true)
    server->serveOne();

  bluegitf::Repositories::release();
  bluegitf::Db::release();
  bluegitf::Config::release();
  mimosa::deinit();
  sqlite3_shutdown();
  return 0;
}
