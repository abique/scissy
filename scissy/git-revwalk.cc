#include "git-revwalk.hh"

namespace scissy
{
  GitRevwalk::GitRevwalk(git_repository * repo)
    : walk_(nullptr)
  {
    git_revwalk_new(&walk_, repo);
  }

  GitRevwalk::~GitRevwalk()
  {
    git_revwalk_free(walk_);
  }
}
