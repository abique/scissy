#include <cerrno>

#include <mimosa/init.hh>
#include <mimosa/thread.hh>
#include <mimosa/log/log.hh>
#include <mimosa/options/options.hh>
#include <mimosa/http/server.hh>
#include <mimosa/http/dispatch-handler.hh>
#include <mimosa/http/fs-handler.hh>
#include <mimosa/http/log-handler.hh>
#include <mimosa/rpc/server.hh>

#include "config.hh"
#include "db.hh"
#include "repositories.hh"
#include "gen-authorized-keys.hh"

// handlers
#include "root-handler.hh"
#include "service.hh"

int main(int argc, char ** argv)
{
  bool stop = false;

  sqlite3_initialize();
  mimosa::init(argc, argv);
  scissy::Config::instance();
  scissy::Db::instance();

  scissy::Db::prepare("PRAGMA foreign_keys = ON").exec();

  scissy::genAuthorizedKeys();

  auto service = new scissy::Service;
  mimosa::rpc::ServiceMap::Ptr service_map = new mimosa::rpc::ServiceMap;
  service_map->add(service);
  mimosa::rpc::Server::Ptr rpc_server = new mimosa::rpc::Server;
  rpc_server->setServiceMap(service_map.get());
  rpc_server->listenUnix(scissy::Config::instance().socket());

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
  dispatch->registerHandler("/", new scissy::RootHandler);
  dispatch->registerHandler("/api/*", new scissy::pb::ServiceHttpHandler(
                              service, "/api/"));

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
    threads.back()->start();
  }

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
