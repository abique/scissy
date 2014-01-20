#include "git-signature.hh"

namespace scissy
{
  void copySignature(const git_signature * in,
                     pb::GitSignature *    out)
  {
    out->set_name(in->name);
    out->set_email(in->email);
    out->set_time(in->when.time);
    out->set_time_offset(in->when.offset);
  }
}
