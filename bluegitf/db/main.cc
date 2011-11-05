#include <cerrno>

#include <mimosa/init.hh>
#include <mimosa/rpc/server.hh>
#include <mimosa/log/log.hh>

#include "../config.hh"
#include "db.hh"

int main(int argc, char ** argv)
{
  mimosa::init(argc, argv);

  mimosa::rpc::ServiceMap::Ptr service_map = new mimosa::rpc::ServiceMap;
  service_map->add(new bluegitf::db::Db);

  mimosa::rpc::Server::Ptr server = new mimosa::rpc::Server;
  server->setServiceMap(service_map);
  if (!server->listenUnix(bluegitf::Config::instance().dbSocket()))
  {
    MIMOSA_LOG(Fatal, NULL, "failed to bind unix socket to %s: %s",
               bluegitf::Config::instance().dbSocket(),
               strerror(errno));
    return 1;
  }

  while (true)
    server->serveOne();

  mimosa::deinit();
  return 0;
}
