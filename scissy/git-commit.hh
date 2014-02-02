#ifndef SCISSY_GIT_COMMIT_HH
# define SCISSY_GIT_COMMIT_HH

# include <git2.h>

# include "../mimosa/mimosa/non-copyable.hh"

# include "service.pb.h"

namespace scissy
{
  class GitCommit : public mimosa::NonCopyable
  {
  public:
    inline GitCommit() : commit_(nullptr) {}
    GitCommit(git_repository *repo, const git_oid *id);
    ~GitCommit();

    inline git_commit** ref() { return &commit_; }
    inline operator git_commit *() { return commit_; }

    void copyTo(pb::GitCommit * commit);

  private:
    git_commit *commit_;
  };
}

#endif /* !SCISSY_GIT_COMMIT_HH */
