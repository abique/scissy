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
#include "group-detail-handler.hh"
#include "login-handler.hh"
#include "logout-handler.hh"
#include "register-handler.hh"
#include "root-handler.hh"
#include "new-repo-handler.hh"
#include "new-group-handler.hh"
#include "repos-handler.hh"
#include "settings-handler.hh"
#include "service.hh"

uint16_t & PORT = *mimosa::options::addOption<uint16_t>("", "port", "the port to use", 19042);

int main(int argc, char ** argv)
{
  sqlite3_initialize();
  mimosa::init(argc, argv);
  scissy::Config::instance();
  scissy::Db::instance();

  scissy::Db::prepare("PRAGMA foreign_keys = ON").exec();

  auto dispatch = new mimosa::http::DispatchHandler;
  dispatch->registerHandler(
    "/css/*.css", new mimosa::http::FsHandler(
      scissy::Config::instance().cssDir(), 1, true));
  dispatch->registerHandler(
    "/js/*", new mimosa::http::FsHandler(
      scissy::Config::instance().jsDir(), 1, true));
  dispatch->registerHandler(
    "/img/*", new mimosa::http::FsHandler(
      scissy::Config::instance().imgDir(), 1, true));
  dispatch->registerHandler(
    "/html/*", new mimosa::http::FsHandler(
      scissy::Config::instance().htmlDir(), 1, true));
  dispatch->registerHandler("/", new scissy::RootHandler);
  dispatch->registerHandler("/clear-cache", new scissy::ClearCacheHandler);
  dispatch->registerHandler("/register", new scissy::RegisterHandler);
  dispatch->registerHandler("/login", new scissy::LoginHandler);
  dispatch->registerHandler("/logout", new scissy::LogoutHandler);
  dispatch->registerHandler("/new-repo", new scissy::NewRepoHandler);
  dispatch->registerHandler("/new-group", new scissy::NewGroupHandler);
  dispatch->registerHandler("/repos", new scissy::ReposHandler);
  dispatch->registerHandler("/settings", new scissy::SettingsHandler);
  dispatch->registerHandler("/groups", new scissy::GroupsHandler);
  dispatch->registerHandler("/group-detail", new scissy::GroupDetailHandler);
  dispatch->registerHandler("/api/*", new scissy::pb::ServiceHttpHandler(
                              new scissy::Service, "/api/"));

  auto log_handler = new mimosa::http::LogHandler;
  log_handler->setHandler(dispatch);

  mimosa::http::Server::Ptr server(new mimosa::http::Server);
  server->setHandler(log_handler);
  if (scissy::Config::instance().isSecure())
    server->setSecure(scissy::Config::instance().certPem(),
                      scissy::Config::instance().keyPem());

  if (!server->listenInet4(PORT))
  {
    mimosa::log::fatal("failed to listen on the port %d: %s",
                       PORT, ::strerror(errno));
    return 1;
  }

  while (true)
    server->serveOne();

  scissy::Repositories::release();
  scissy::Db::release();
  scissy::Config::release();
  mimosa::deinit();
  sqlite3_shutdown();
  return 0;
}
