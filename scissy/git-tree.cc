#include "git-tree.hh"
#include "git-tree-entry.hh"

namespace scissy
{
  GitTree::GitTree(git_repository * repo,
                   const git_oid * id)
    : tree_(nullptr)
  {
    git_tree_lookup(&tree_, repo, id);
  }

  GitTree::GitTree(git_repository * repo,
                   const git_oid * id,
                   const std::string & directory)
    : tree_(nullptr)
  {
    if (directory.empty()) {
      git_tree_lookup(&tree_, repo, id);
      return;
    }

    GitTree root(repo, id);
    GitTreeEntry entry;
    if (git_tree_entry_bypath(entry.ref(), root, directory.c_str()))
      return;

    if (git_tree_entry_type(entry) != GIT_OBJ_TREE)
      return;

    git_tree_lookup(&tree_, repo, git_tree_entry_id(entry));
  }

  GitTree::~GitTree()
  {
    git_tree_free(tree_);
  }
}
