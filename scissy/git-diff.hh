#ifndef SCISSY_GIT_DIFF_HH
# define SCISSY_GIT_DIFF_HH

# include <git2/diff.h>

namespace scissy
{
  class GitDiff
  {
  public:
    inline GitDiff(git_diff * diff = nullptr) : diff_(diff) {}
    inline ~GitDiff() { git_diff_free(diff_); }

    inline operator git_diff *() { return diff_; }
    inline git_diff ** ref() { return &diff_; }

  private:
    git_diff * diff_;
  };
}

#endif /* !SCISSY_GIT_DIFF_HH */
