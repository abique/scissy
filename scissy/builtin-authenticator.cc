#include <mimosa/stream/base16-decoder.hh>
#include <mimosa/stream/hash.hh>

#include "db.hh"
#include "builtin-authenticator.hh"
#include "log.hh"

namespace scissy
{
  bool
  BuiltinAuthenticator::auth(pb::UserAuth & request,
                        pb::Session & response) const
  {
    // load the password from the db
    std::string type;
    std::string salt16;
    std::string hash16;

    auto stmt             = Db::prepare(
      "select password_type, password_salt, password_hash from users where login = ?");
    if (!stmt.bind(request.user()).fetch(&type, &salt16, &hash16)) {
      response.set_status(pb::kFailed);
      response.set_msg("user not found");
      return true;
    }

    std::string salt = mimosa::stream::filter<mimosa::stream::Base16Decoder>(
      salt16);
    std::string hash_bin = mimosa::stream::filter<mimosa::stream::Base16Decoder>(
      hash16);
    std::string hash_bin_computed;

    // compute the hash
    if (type == "SHA3-512") {
      mimosa::stream::Sha3_512 hash;
      hash.write(salt.data(), salt.size());
      hash.write(request.password().data(), request.password().size());
      hash_bin_computed = std::string(hash.digest(), hash.digestLen());
    } else {
      log->error("unsupported hash type: %s for user %s", type, request.user());
      return false;
    }

    // chech if hashes equals
    if (hash_bin != hash_bin_computed) {
      log->error("invalid password for user %s", request.user());
      response.set_status(pb::kFailed);
      response.set_msg("invalid password");
      return true;
    }

    // password matched
    return true;
  }
}
