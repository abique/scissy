#include <unistd.h>
#include <sys/types.h>
#include <grp.h>

#include <cerrno>

#include <mimosa/http/dispatch-handler.hh>
#include <mimosa/http/fs-handler.hh>
#include <mimosa/http/log-handler.hh>
#include <mimosa/http/server.hh>
#include <mimosa/init.hh>
#include <mimosa/log/log.hh>
#include <mimosa/options/options.hh>
#include <mimosa/priviledge-drop.hh>
#include <mimosa/rpc/server.hh>
#include <mimosa/thread.hh>

#include "config.hh"
#include "db.hh"
#include "repositories.hh"
#include "gen-authorized-keys.hh"

// handlers
#include "logout-handler.hh"
#include "root-handler.hh"
#include "session-handler.hh"
#include "service.hh"

#define USER_VERSION 1

int main(int argc, char ** argv)
{
  bool stop = false;

  sqlite3_initialize();
  mimosa::init(argc, argv);
  scissy::Config::instance();
  scissy::Db::instance();

  int user_version = 0;
  scissy::Db::prepare("PRAGMA foreign_keys = ON").exec();
  scissy::Db::prepare("PRAGMA user_version").fetch(&user_version);
  if (user_version != USER_VERSION) {
    mimosa::log::fatal("bad sqlite user_version: %d, requires: %d;"
                       " please upgrade either scissy or your database",
                       user_version, USER_VERSION);
    return 1;
  }

  scissy::genAuthorizedKeys();

  auto service = new scissy::Service;
  mimosa::rpc::ServiceMap::Ptr service_map = new mimosa::rpc::ServiceMap;
  service_map->add(service);
  mimosa::rpc::Server::Ptr rpc_server = new mimosa::rpc::Server;
  rpc_server->setServiceMap(service_map.get());

  auto dispatch = new mimosa::http::DispatchHandler;
  dispatch->registerHandler(
    "/css/*.css", new mimosa::http::FsHandler(
      scissy::Config::instance().css(), 1, true));
  dispatch->registerHandler(
    "/js/*", new mimosa::http::FsHandler(
      scissy::Config::instance().js(), 1, true));
  dispatch->registerHandler(
    "/fonts/*", new mimosa::http::FsHandler(
      scissy::Config::instance().fonts(), 1, true));
  dispatch->registerHandler(
    "/html/*", new mimosa::http::FsHandler(
      scissy::Config::instance().html(), 1, true));
  dispatch->registerHandler(
    "/cm/*", new mimosa::http::FsHandler(
      scissy::Config::instance().cm(), 1, true));
  dispatch->registerHandler("/", new scissy::RootHandler);
  dispatch->registerHandler("/logout", new scissy::LogoutHandler);
  dispatch->registerHandler(
    "/api/*", new scissy::SessionHandler(
      new scissy::pb::ServiceHttpHandler(service, "/api/")));

  auto log_handler = new mimosa::http::LogHandler;
  log_handler->setHandler(dispatch);

  std::vector<mimosa::Thread *> threads;
  for (auto & it : scissy::Config::instance().listens()) {
    mimosa::http::Server::Ptr http_server(new mimosa::http::Server);
    http_server->setHandler(log_handler);
    if (it.isSslEnabled())
      http_server->setSecure(it.ssl_cert_, it.ssl_key_);

    if (!http_server->listenInet4(it.port_)) {
      mimosa::log::fatal("failed to listen on the port %d: %s",
                         it.port_, ::strerror(errno));
      return 1;
    }

    threads.push_back(new mimosa::Thread([&stop, http_server] {
          while (!stop)
            http_server->serveOne();
        }));
  }

  if (!scissy::Config::instance().user().empty() ||
      !scissy::Config::instance().group().empty()) {
    mimosa::priviledgeDrop("/",
                           scissy::Config::instance().user(),
                           scissy::Config::instance().group());
  }

  for (auto & thread : threads)
    thread->start();

  rpc_server->listenUnix(scissy::Config::instance().socket());
  while (!stop)
    rpc_server->serveOne();

  for (auto & thread : threads) {
    thread->join();
    delete thread;
  }

  scissy::Repositories::release();
  scissy::Db::release();
  scissy::Config::release();
  mimosa::deinit();
  sqlite3_shutdown();
  return 0;
}
