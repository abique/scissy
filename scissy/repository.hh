#ifndef SCISSY_REPOSITORY_HH
# define SCISSY_REPOSITORY_HH

# include <cstdint>
# include <string>

# include <git2.h>

namespace scissy
{
  class Repository
  {
  public:
    Repository(const std::string & repo_path);
    ~Repository();

    inline operator git_repository * () {
      return repo_;
    }

    int64_t lastCommitTime() const;

  private:
    git_repository *repo_;
  };
}

#endif /* !SCISSY_REPOSITORY_HH */
