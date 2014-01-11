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
  Db::userGetId(const std::string & user, int64_t * user_id)
  {
    auto stmt = prepare("select user_id from users where login = ?");
    return stmt.bind(user).fetch(user_id);
  }

  /***************/
  /* group stuff */
  /***************/

  bool
  Db::groupGetUserRole(int64_t group_id, int64_t user_id, pb::Role * role)
  {
    auto stmt = prepare(
      "select role_id from groups_users"
      " where group_id = ? and user_id = ?");
    return stmt.bind(group_id, user_id).fetch(reinterpret_cast<int *>(role));
  }

  bool
  Db::groupAddUser(int64_t grp_id, int64_t user_id, pb::Role role)
  {
    auto stmt = prepare(
      "insert into groups_users (group_id, user_id, role_id) values"
      " (?, ?, ?)");
    return stmt.bind(grp_id, user_id, role).step() == SQLITE_DONE;
  }

  void
  Db::groupRemoveUser(int64_t grp_id, int64_t user_id)
  {
    auto stmt = prepare(
      "delete from groups_users"
      " where group_id = ? and user_id = ?");
    stmt.bind(grp_id, user_id).exec();
  }

  bool
  Db::groupCreate(const std::string & group,
                  const std::string & desc,
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
  Db::groupDelete(int64_t grp_id)
  {
    auto stmt = prepare("delete from groups where group_id = ?");
    return stmt.bind(grp_id).step();
  }

  /**************/
  /* repo stuff */
  /**************/

  bool
  Db::repoGetId(const std::string & repo, int64_t * repo_id)
  {
    auto stmt = prepare("select repo_id from repos where `name` = ?");
    return stmt.bind(repo).fetch(repo_id);
  }
}
