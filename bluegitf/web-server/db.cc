#include <mimosa/log/log.hh>
#include <mimosa/net/connect.hh>
#include <mimosa/stream/fd-stream.hh>

#include "db.hh"
#include "../config.hh"

namespace bluegitf
{
  namespace web_server
  {
    db::pb::Db::Client::Ptr dbService()
    {
      static mimosa::sync::Mutex     lock;
      static db::pb::Db::Client::Ptr db;

      mimosa::sync::Mutex::Locker locker(lock);

      // connect to the db
      if (!db)
      {
        int fd = mimosa::net::connectToSocket(Config::instance().dbSocket());
        if (fd < 0)
        {
          MIMOSA_LOG(Error, NULL, "failed to connect to: %s: %s",
                     Config::instance().dbSocket(), strerror(errno));
          return nullptr;
        }

        mimosa::rpc::Channel::Ptr channel;
        channel = new mimosa::rpc::Channel(new mimosa::stream::FdStream(fd));
        channel->start();

        db = new db::pb::Db::Client(channel);
        MIMOSA_LOG(Error, NULL, "connected to %s", Config::instance().dbSocket());
      }

      return db;
    }
  }
}
