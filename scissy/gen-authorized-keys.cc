#include <fstream>

#include "config.hh"
#include "db.hh"
#include "gen-authorized-keys.hh"
#include "ssh-key-type.hh"

namespace scissy
{
  void genAuthorizedKeys()
  {
    std::ofstream out(Config::instance().authorizedKeysPath());

    uint64_t    user_id;
    int         key_type;
    std::string key;

    auto stmt = Db::prepare("select user_id, `key_type`, `key` from users_ssh_keys");
    while (stmt.fetch(&user_id, &key_type, &key)) {
      out << "command=\"scyssi-shell -user-id=" << user_id
          << "\",no-port-forwarding,no-pty,no-X11-forwarding "
          << sshKeyTypeName((pb::SshKeyType)(key_type))
          << " " << key << std::endl;
    }
  }
}
