#ifndef SCISSY_GIT_COMMIT_HH
# define SCISSY_GIT_COMMIT_HH

# include <git2.h>

# include "service.pb.h"

namespace scissy
{
  class GitCommit
  {
  public:
    GitCommit(git_repository *repo, const git_oid *id);
    ~GitCommit();

    inline int status() { return status_; }
    inline operator git_commit *() { return commit_; }

    void copyTo(pb::GitCommit * commit);

  private:
    int status_;
    git_commit *commit_;
  };
}

#endif /* !SCISSY_GIT_COMMIT_HH */
