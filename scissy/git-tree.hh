#ifndef SCISSY_GIT_TREE_HH
# define SCISSY_GIT_TREE_HH

# include <string>

# include <git2/tree.h>

# include <mimosa/non-copyable.hh>

namespace scissy
{
  class GitTree : private mimosa::NonCopyable
  {
  public:
    GitTree(git_repository * repo,
            const git_oid * oid);
    GitTree(git_repository * repo,
            const git_oid * oid,
            const std::string & directory);
    ~GitTree();

    inline operator git_tree *() const {
      return tree_;
    }

  private:
    git_tree * tree_;
  };
}

#endif /* !SCISSY_GIT_TREE_HH */
