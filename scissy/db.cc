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

  int64_t
  Db::lastInsertRowid()
  {
    return sqlite3_last_insert_rowid(Db::instance().db_);
  }

  mimosa::sqlite::Stmt
  Db::prepare(const char *rq)
  {
    return Db::instance().db_.prepare(rq);
  }

  /**************/
  /* user stuff */
  /**************/

  bool
  Db::userGetId(const std::string & user, uint64_t * user_id)
  {
    auto stmt = prepare("select user_id from users where login = ?");
    return stmt.bind(user).fetch(user_id);
  }

  /***************/
  /* group stuff */
  /***************/

  bool
  Db::groupGetUserRole(const std::string & group,
                       const std::string & user,
                       pb::Role *          role)
  {
    auto stmt = prepare(
      "select role_id from groups_users_view"
      " where name = ? and user = ?");
    return stmt.bind(group, user).fetch(reinterpret_cast<int *>(role));
  }

  bool
  Db::groupAddUser(const std::string & group,
                   const std::string & user,
                   pb::Role            role)
  {
    auto stmt = prepare(
      "insert into groups_users (group_id, user_id, role_id) values"
      " ((select group_id from groups where name = ?),"
      " (select user_id from users where login = ?),"
      " ?)");
    return stmt.bind(group, user, role).step() == SQLITE_DONE;
  }

  void
  Db::groupRemoveUser(const std::string & group,
                      const std::string & user)
  {
    auto stmt = prepare(
      "delete from groups_users"
      " where group_id = (select group_id from groups where name = ?)"
      " and user_id = (select user_id from users where login = ?)");
    stmt.bind(group, user).exec();
  }

  bool
  Db::groupCreate(const std::string & group,
                  const std::string & desc,
                  const std::string & owner,
                  std::string *       error)
  {
    // insert the data into sqlite
    {
      auto stmt = Db::prepare("insert or fail into groups (name, desc) values (?, ?)");
      int err = stmt.bind(group, desc).step();

      if (err == SQLITE_CONSTRAINT)
      {
        *error = "name already used";
        return false;
      }

      if (err != SQLITE_DONE)
      {
        *error = "failed to register, internal error";
        return false;
      }
    }

    if (!groupAddUser(group, owner, pb::kOwner))
    {
      *error = "failed to set owner";
      auto stmt = Db::prepare("delete from groups where name = ?");
      stmt.bind(group).exec();
      return false;
    }

    return true;
  }

  bool
  Db::groupGetId(const std::string & group,
                 int64_t *           id)
  {
    auto stmt = prepare("select group_id from groups where name = ?");
    return stmt.bind(group).fetch(id);
  }

  bool
  Db::groupDelete(const std::string & group)
  {
    auto stmt = prepare("delete from groups where name = ?");
    return stmt.bind(group).step();
  }
}
