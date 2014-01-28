#include "git-commit.hh"
#include "git-signature.hh"

namespace scissy
{
  GitCommit::GitCommit(git_repository *repo, const git_oid *id)
    : commit_(nullptr)
  {
    git_commit_lookup(&commit_, repo, id);
  }

  GitCommit::~GitCommit()
  {
    git_commit_free(commit_);
  }

  void
  GitCommit::copyTo(pb::GitCommit * commit)
  {
    char oid_str[GIT_OID_HEXSZ + 1];
    const git_oid *oid;

    oid = git_commit_id(commit_);
    git_oid_tostr(oid_str, sizeof (oid_str), oid);
    commit->set_id(oid_str);

    for (unsigned int i = 0; i < git_commit_parentcount(commit_); ++i) {
      oid = git_commit_parent_id(commit_, i);
      git_oid_tostr(oid_str, sizeof (oid_str), oid);
      commit->add_parents_id(oid_str);
    }

    oid = git_commit_tree_id(commit_);
    git_oid_tostr(oid_str, sizeof (oid_str), oid);
    commit->set_tree_id(oid_str);

    commit->set_msg(git_commit_message(commit_));
    commit->set_time(static_cast<uint64_t>(git_commit_time(commit_)) * 1000 +
                     git_commit_time_offset(commit_));
    copySignature(git_commit_committer(commit_), commit->mutable_commiter());
    copySignature(git_commit_author(commit_), commit->mutable_author());
  }
}
