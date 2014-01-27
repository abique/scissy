#ifndef SCISSY_GIT_TREE_ENTRY_HH
# define SCISSY_GIT_TREE_ENTRY_HH

namespace scissy
{
  class GitTreeEntry
  {
  public:
    inline GitTreeEntry(git_tree_entry * entry = nullptr) : entry_(entry) {}
    inline ~GitTreeEntry() { git_tree_entry_free(entry_); }

    inline operator git_tree_entry *() { return entry_; }
    inline git_tree_entry ** ref() { return &entry_; }

  private:
    git_tree_entry * entry_;
  };
}

#endif /* !SCISSY_GIT_TREE_ENTRY_HH */
