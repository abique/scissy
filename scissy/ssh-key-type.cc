#include "ssh-key-type.hh"

namespace scissy
{
  const char *sshKeyTypeName(pb::SshKeyType key_type)
  {
    switch (key_type) {
    case pb::kSshKeyRsa:
      return "ssh-rsa";

    case pb::kSshKeyDsa:
      return "ssh-dsa";

    default:
      return "unknown";
    }
  }
}
