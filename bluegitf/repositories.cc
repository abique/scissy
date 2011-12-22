#include <mimosa/format/format.hh>
#include <mimosa/sqlite/sqlite.hh>

#include <git2.h>

#include "config.hh"
#include "db.hh"
#include "repositories.hh"

namespace bluegitf
{
  Repositories::Repositories()
  {
  }

  Repositories::~Repositories()
  {
  }

  bool
  Repositories::getId(const std::string & name,
                      int64_t &           id)
  {
    mimosa::sqlite::Stmt stmt;
    int err = stmt.prepare(Db::handle(),
                           "select repo_id from repos where name = ?");
    assert(err == SQLITE_OK); // must pass

    err = stmt.bindBlob(1, name.c_str(), name.size());
    assert(err == SQLITE_OK);

    err = stmt.step();
    if (err != SQLITE_ROW)
      return false;

    id = sqlite3_column_int64(stmt, 0);
    return true;
  }

  bool
  Repositories::getRepoPath(const std::string & name,
                            std::string &       path)
  {
    int64_t repo_id = 0;
    if (!getId(name, repo_id))
      return false;

    path = mimosa::format::str("%s/%x", Config::instance().repoDir(), repo_id);
    return true;
  }

  bool
  Repositories::create(const std::string & name,
                       const std::string & desc,
                       std::string &       error)
  {
    // insert the data into sqlite
    {
      mimosa::sqlite::Stmt stmt;
      int err = stmt.prepare(Db::handle(),
                             "insert or fail into repos (name, desc)"
                             " values (?, ?)");
      assert(err == SQLITE_OK); // must pass

      err = stmt.bindBlob(1, name.c_str(), name.size());
      assert(err == SQLITE_OK);
      err = stmt.bind(2, desc);
      assert(err == SQLITE_OK);

      err = stmt.step();
      if (err == SQLITE_CONSTRAINT)
      {
        error = "name already used";
        return false;
      }

      if (err != SQLITE_DONE)
      {
        error = "failed to register, internal error";
        return false;
      }
    }

    // create the repository on the filesystem
    std::string path;
    if (!getRepoPath(name, path))
    {
      error = "failed to get the repo path";
      return false;
    }

    git_repository * repo = nullptr;
    int err = git_repository_init(&repo, path.c_str(), true);
    if (err != GIT_SUCCESS)
    {
      error = mimosa::format::str("failed to initialize git repository: %s",
                                  git_strerror(err));
      return false;
    }

    git_repository_free(repo);
    return true;
  }
}
