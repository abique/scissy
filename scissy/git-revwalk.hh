#ifndef SCISSY_GIT_REVWALK_HH
# define SCISSY_GIT_REVWALK_HH

# include <git2.h>

namespace scissy
{
  class GitRevwalk
  {
  public:
    GitRevwalk(git_repository *repo);
    ~GitRevwalk();

    inline operator git_revwalk *() { return walk_; }

  private:
    git_revwalk *walk_;
  };
}

#endif /* !SCISSY_GIT_REVWALK_HH */
