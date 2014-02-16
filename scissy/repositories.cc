#include <mimosa/format/format.hh>
#include <mimosa/sqlite/sqlite.hh>
#include <mimosa/fs/rm.hh>

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

  std::string
  Repositories::getRepoPath(int64_t repo_id)
  {
    return mimosa::format::str("%s/%x", Config::instance().repos(), repo_id);
  }

  bool
  Repositories::create(const std::string & name,
                       const std::string & desc,
                       bool                is_public,
                       int64_t             user_id,
                       int64_t *           repo_id,
                       std::string *       error)
  {
    // insert the data into sqlite
    {
      auto stmt = Db::prepare("insert or fail into repos (name, desc, is_public)"
                              " values (?, ?, ?)");
      int err = stmt.bind(name, desc, is_public).step();

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

    if (!Db::repoAddUser(*repo_id, user_id, pb::kOwner)) {
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
    mimosa::fs::rm(getRepoPath(repo_id), true, true);
    return true;
  }
}
