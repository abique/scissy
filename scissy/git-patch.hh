#ifndef SCISSY_GIT_PATCH_HH
# define SCISSY_GIT_PATCH_HH

namespace scissy
{
  class GitPatch
  {
  public:
    inline GitPatch(git_patch * patch = nullptr) : patch_(patch) {}
    inline ~GitPatch() { git_patch_free(patch_); }

    inline operator git_patch *() { return patch_; }
    inline git_patch ** ref() { return &patch_; }

  private:
    git_patch * patch_;
  };
}

#endif /* !SCISSY_GIT_PATCH_HH */
