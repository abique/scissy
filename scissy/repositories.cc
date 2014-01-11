#include <mimosa/format/format.hh>
#include <mimosa/sqlite/sqlite.hh>

#include <git2.h>

#include "config.hh"
#include "db.hh"
#include "repositories.hh"
#include "log.hh"

namespace scissy
{
  Repositories::Repositories()
  {
  }

  Repositories::~Repositories()
  {
  }

  bool
  Repositories::addUser(int64_t repo_id, int64_t user_id, pb::Role role)
  {
    auto stmt = Db::prepare(
      "insert into repos_users (repo_id, user_id, role_id) values (?, ?, ?)");
    return stmt.bind(repo_id, user_id, role).step() == SQLITE_DONE;
  }

  bool
  Repositories::removeUser(int64_t repo_id, int64_t user_id)
  {
    auto stmt = Db::prepare(
      "delete from repos_users where repo_id = ? and user_id = ?");
    return stmt.bind(repo_id, user_id).step() == SQLITE_DONE;
  }

  bool
  Repositories::addGroup(int64_t repo_id, int64_t group_id, pb::Role role)
  {
    auto stmt = Db::prepare(
      "insert into repos_groups (repo_id, group_id, role_id) values (?, ?, ?)");
    return stmt.bind(repo_id, group_id, role).step() == SQLITE_DONE;
  }

  bool
  Repositories::removeGroup(int64_t repo_id, int64_t group_id)
  {
    auto stmt = Db::prepare(
      "delete from repos_groups where repo_id = ? and group_id = ?");
    return stmt.bind(repo_id, group_id).step() == SQLITE_DONE;
  }

  bool
  Repositories::getId(const std::string & name,
                      int64_t *           id)
  {
    auto stmt = Db::prepare("select repo_id from repos where name = ?");
    return stmt.bind(name).fetch(id);
  }

  bool
  Repositories::getUserRole(int64_t repo_id, int64_t user_id, pb::Role * role)
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

  std::string
  Repositories::getRepoPath(int64_t repo_id)
  {
    return mimosa::format::str("%s/%x", Config::instance().repos(), repo_id);
  }

  bool
  Repositories::create(const std::string & name,
                       const std::string & desc,
                       int64_t             user_id,
                       int64_t *           repo_id,
                       std::string *       error)
  {
    // insert the data into sqlite
    {
      auto stmt = Db::prepare("insert or fail into repos (name, desc) values (?, ?)");
      int err = stmt.bind(name, desc).step();

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

      *repo_id = Db::lastInsertRowid();
    }

    // create the repository on the filesystem
    {
      std::string path = getRepoPath(*repo_id);
      git_repository * repo = nullptr;
      int err = git_repository_init(&repo, path.c_str(), true);
      if (err != GIT_OK) {
        *error = "failed to initialize git repository";
        auto stmt = Db::prepare("delete from repos where name = ?");
        stmt.bind(name).exec();
        return false;
      }

      git_repository_free(repo);
    }

    if (!addUser(*repo_id, user_id, pb::kOwner)) {
      *error = "failed to set owner";
      auto stmt = Db::prepare("delete from repos where name = ?");
      stmt.bind(name).exec();
      return false;
    }

    return true;
  }

  bool
  Repositories::remove(int64_t repo_id)
  {
    auto stmt = Db::prepare("delete from repos where repo_id = ?");
    stmt.bind(repo_id).exec();
    // XXX rm
    return true;
  }
}
