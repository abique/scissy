#ifndef SCISSY_GIT_SIGNATURE_HH
# define SCISSY_GIT_SIGNATURE_HH

# include <git2.h>

# include "service.pb.h"

namespace scissy
{
  void copySignature(const git_signature * in,
                     pb::GitSignature *    out);
}

#endif /* !SCISSY_GIT_SIGNATURE_HH */
