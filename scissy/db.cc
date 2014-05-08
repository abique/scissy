#include <cassert>

#include <mimosa/log/log.hh>

#include "config.hh"
#include "db.hh"
#include "log.hh"

namespace scissy
{
  Db::Db()
    : db_()
  {
    int err = db_.open(Config::instance().db().c_str(),
                       SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE |
                       SQLITE_OPEN_FULLMUTEX, nullptr);

    if (err != SQLITE_OK)
      mimosa::log::fatal("failed to open sqlite database: %s: %s",
                         Config::instance().db(), sqlite3_errmsg(db_));
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

  bool
  Db::userIsAdmin(int64_t user_id, bool * is_admin)
  {
    auto stmt = prepare("select is_admin from users where user_id = ?");
    return stmt.bind(user_id).fetch(is_admin);
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

  bool
  Db::repoAddUser(int64_t repo_id, int64_t user_id, pb::Role role)
  {
    auto stmt = Db::prepare(
      "insert into repos_users (repo_id, user_id, role_id) values (?, ?, ?)");
    return stmt.bind(repo_id, user_id, role).step() == SQLITE_DONE;
  }

  bool
  Db::repoRemoveUser(int64_t repo_id, int64_t user_id)
  {
    auto stmt = Db::prepare(
      "delete from repos_users where repo_id = ? and user_id = ?");
    return stmt.bind(repo_id, user_id).step() == SQLITE_DONE;
  }

  bool
  Db::repoAddGroup(int64_t repo_id, int64_t group_id, pb::Role role)
  {
    auto stmt = Db::prepare(
      "insert into repos_groups (repo_id, group_id, role_id) values (?, ?, ?)");
    return stmt.bind(repo_id, group_id, role).step() == SQLITE_DONE;
  }

  bool
  Db::repoRemoveGroup(int64_t repo_id, int64_t group_id)
  {
    auto stmt = Db::prepare(
      "delete from repos_groups where repo_id = ? and group_id = ?");
    return stmt.bind(repo_id, group_id).step() == SQLITE_DONE;
  }

  bool
  Db::repoGetUserRole(int64_t repo_id, int64_t user_id, pb::Role * role)
  {
    int role1;
    *role = pb::kNone;

    // check users
    auto stmt = Db::prepare(" select role_id from repos_users"
                            "  where repo_id = ? and user_id = ? "
                            "union "
                            " select min(repos_groups.role_id, groups_users.role_id)"
                            " from repos_groups natural join groups_users"
                            " where repo_id = ? and user_id = ?");
    while (stmt.bind(repo_id, user_id, repo_id, user_id).fetch(&role1)) {
      if (!pb::Role_IsValid(role1)) {
        log->critical("getUserRole(%d, %d): invalid role %d", repo_id, user_id, role1);
        return false;
      }

      if (role1 > *role)
        *role = static_cast<pb::Role> (role1);
    }

    return true;
  }

  bool
  Db::repoIsPublic(int64_t repo_id, bool * is_public)
  {
    int tmp;
    auto stmt = Db::prepare("select is_public from repos where repo_id = ?");
    stmt.bind(repo_id);

    if (!stmt.fetch(&tmp))
      return false;
    *is_public = !!tmp;
    return true;
  }
}
