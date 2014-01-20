#include "repository.hh"

namespace scissy
{
  Repository::Repository(const std::string & repo_path)
    : repo_(nullptr)
  {
    git_repository_open(&repo_, repo_path.c_str());
  }

  Repository::~Repository()
  {
    git_repository_free(repo_);
  }
}
