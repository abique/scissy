#ifndef SCISSY_HELPERS_HH
# define SCISSY_HELPERS_HH

# include <mimosa/git/commit.hh>

#include "service.pb.h"

namespace scissy
{
  void copyCommit(const mimosa::git::Commit & src,
                  pb::GitCommit *             dst);

  void copySignature(const git_signature * in,
                     pb::GitSignature *    out);
}

#endif /* !SCISSY_HELPERS_HH */
