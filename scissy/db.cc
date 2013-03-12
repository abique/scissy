#include <cassert>

#include <mimosa/log/log.hh>

#include "config.hh"
#include "db.hh"

namespace scissy
{
  Db::Db()
    : db_()
  {
    int err = db_.open(Config::instance().dbPath().c_str(),
                       SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE |
                       SQLITE_OPEN_FULLMUTEX, nullptr);

    if (err != SQLITE_OK)
      mimosa::log::fatal("failed to open sqlite database: %s: %s",
                         Config::instance().dbPath(), sqlite3_errmsg(db_));
  }

  mimosa::sqlite::Stmt
  Db::prepare(const char *rq)
  {
    return Db::instance().db_.prepare(rq);
  }

  bool
  Db::getUserId(const std::string & user, uint64_t * user_id)
  {
    auto stmt = prepare("select user_id from users where login = ?");
    return stmt.bind(user).fetch(user_id);
  }
}
