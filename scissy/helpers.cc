#include "helpers.hh"

namespace scissy
{
  void copyCommit(const mimosa::git::Commit & src,
                  pb::GitCommit *             dst)
  {
    char oid_str[GIT_OID_HEXSZ + 1];
    const git_oid *oid;

    oid = git_commit_id(src);
    git_oid_tostr(oid_str, sizeof (oid_str), oid);
    dst->set_id(oid_str);

    for (unsigned int i = 0; i < git_commit_parentcount(src); ++i) {
      oid = git_commit_parent_id(src, i);
      git_oid_tostr(oid_str, sizeof (oid_str), oid);
      dst->add_parents_id(oid_str);
    }

    oid = git_commit_tree_id(src);
    git_oid_tostr(oid_str, sizeof (oid_str), oid);
    dst->set_tree_id(oid_str);

    dst->set_msg(git_commit_message(src));
    dst->set_time(static_cast<uint64_t>(git_commit_time(src)) * 1000 +
                     git_commit_time_offset(src));
    copySignature(git_commit_committer(src), dst->mutable_commiter());
    copySignature(git_commit_author(src), dst->mutable_author());
  }

  void copySignature(const git_signature * in,
                     pb::GitSignature *    out)
  {
    out->set_name(in->name);
    out->set_email(in->email);
    out->set_time(static_cast<uint64_t>(in->when.time) * 1000 + in->when.offset);
  }
}
