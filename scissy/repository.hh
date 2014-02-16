#ifndef SCISSY_GIT_REPOSITORY_HH
# define SCISSY_GIT_REPOSITORY_HH

# include <cstdint>
# include <string>

# include <git2.h>

namespace scissy
{
  class GitRepository
  {
  public:
    GitRepository(const std::string & repo_path);
    ~GitRepository();

    inline operator git_repository * () {
      return repo_;
    }

    int64_t lastCommitTime() const;

  private:
    git_repository *repo_;
  };

  typedef GitRepository Repository; // XXX remove this
}

#endif /* !SCISSY_GIT_REPOSITORY_HH */
