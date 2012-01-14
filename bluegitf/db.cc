#include <cassert>

#include <mimosa/log/log.hh>

#include "config.hh"
#include "db.hh"

namespace bluegitf
{
  Db::Db()
    : db_(nullptr)
  {
    int err = sqlite3_open_v2(Config::instance().dbPath().c_str(),
                              &db_, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE |
                              SQLITE_OPEN_FULLMUTEX, nullptr);

    if (err != SQLITE_OK)
    {
      mimosa::log::fatal("failed to open sqlite database: %s: %s",
                         Config::instance().dbPath(), sqlite3_errmsg(db_));
      sqlite3_close(db_);
      db_ = nullptr;
    }
  }

  Db::~Db()
  {
    if (db_)
    {
      sqlite3_close(db_);
      db_ = nullptr;
    }
  }

  sqlite3 *
  Db::handle()
  {
    auto db = Db::instance().db_;
    assert(db);
    return db;
  }
}
