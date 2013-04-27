#include <mimosa/format/format.hh>
#include <mimosa/sqlite/sqlite.hh>

#include <git2.h>

#include "config.hh"
#include "db.hh"
#include "repositories.hh"

namespace scissy
{
  Repositories::Repositories()
  {
  }

  Repositories::~Repositories()
  {
  }

  bool
  Repositories::addOwner(const std::string & name,
                         const std::string & owner)
  {
    auto stmt = Db::prepare(
      "insert into repos_users (repo_id, user_id, role_id) values"
      " ((select repo_id from repos where name = ?),"
      " (select user_id from users where login = ?),"
      " 0)");
    return stmt.bind(name, owner).step() == SQLITE_DONE;
  }

  bool
  Repositories::getId(const std::string & name,
                      int64_t *           id)
  {
    auto stmt = Db::prepare("select repo_id from repos where name = ?");
    return stmt.bind(name).fetch(id);
  }

  bool
  Repositories::getRepoPath(const std::string & name,
                            std::string *       path)
  {
    int64_t repo_id = 0;
    if (!getId(name, &repo_id))
      return false;

    *path = mimosa::format::str("%s/%x", Config::instance().repoDir(), repo_id);
    return true;
  }

  bool
  Repositories::create(const std::string & name,
                       const std::string & desc,
                       const std::string & owner,
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
      std::string path;
      if (!getRepoPath(name, &path))
      {
        *error = "failed to get the repo path";
        return false;
      }

      git_repository * repo = nullptr;
      int err = git_repository_init(&repo, path.c_str(), true);
      if (err != GIT_OK)
      {
        *error = "failed to initialize git repository";
        auto stmt = Db::prepare("delete from repos where name = ?");
        stmt.bind(name).exec();
        return false;
      }

      git_repository_free(repo);
    }

    if (!addOwner(name, owner))
    {
      *error = "failed to set owner";
      auto stmt = Db::prepare("delete from repos where name = ?");
      stmt.bind(name).exec();
      return false;
    }

    return true;
  }
}
