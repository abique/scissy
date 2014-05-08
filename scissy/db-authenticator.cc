#include <mimosa/stream/hash.hh>

#include "db.hh"
#include "db-authenticator.hh"
#include "log.hh"

namespace scissy
{
  bool
  DbAuthenticator::auth(pb::UserAuth & request,
                        pb::Session & response) const
  {
    // load the password from the db
    const void *blob      = nullptr;
    int         blob_size = 0;
    auto stmt             = Db::prepare(
      "select password from users where login = ?");
    if (!stmt.bind(request.user()).fetch(&blob, &blob_size)) {
      response.set_status(pb::kFailed);
      response.set_msg("user not found");
      return true;
    }

    // compute the hash
    mimosa::stream::Sha512 sha512;
    sha512.write(request.password().data(), request.password().size());

    // chech if hashes equals
    if ((size_t)blob_size != sha512.digestLen() ||
        ::memcmp(sha512.digest(), blob, blob_size)) {
      log->error("invalid password hash for user %s, (%d, %d), ",
                 request.user(), blob_size, sha512.digestLen());
      response.set_status(pb::kFailed);
      response.set_msg("invalid password");
      return true;
    }
    return true;
  }
}
