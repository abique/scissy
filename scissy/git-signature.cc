#include "git-signature.hh"

namespace scissy
{
  void copySignature(const git_signature * in,
                     pb::GitSignature *    out)
  {
    out->set_name(in->name);
    out->set_email(in->email);
    out->set_time(static_cast<uint64_t>(in->when.time) * 1000 + in->when.offset);
  }
}
