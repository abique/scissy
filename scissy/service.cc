#include <ctime>

#include <re2/re2.h>

#include <crack.h>

#include <mimosa/stream/hash.hh>
#include <mimosa/stream/string-stream.hh>
#include <mimosa/stream/base16-decoder.hh>
#include <mimosa/stream/base16-encoder.hh>
#include <mimosa/log/log.hh>

#include "config.hh"
#include "db.hh"
#include "service.hh"

namespace scissy
{
  bool
  Service::userCreate(pb::UserCreate & request,
                      pb::StatusMsg &  response)
  {
    static const RE2 login_match("^[[:alnum:]]+$");
    static const RE2 email_match("^[-._a-zA-Z0-9]+@[-._a-zA-Z0-9]+$");

    if (request.user().size() > 64) {
      response.set_status(pb::kFailed);
      response.set_msg("login too long (64 characters at most)");
      return true;
    }

    if (request.password().size() < 5) {
      response.set_status(pb::kFailed);
      response.set_msg("password too short (5 characters at least)");
      return true;
    }

    if (request.password().size() > 32) {
      response.set_status(pb::kFailed);
      response.set_msg("password too long (32 characters at most)");
      return true;
    }

    if (!RE2::FullMatch(request.user(), login_match)) {
      response.set_status(pb::kFailed);
      response.set_msg("login contains invalid characters");
      return true;
    }

    if (!RE2::FullMatch(request.email(), email_match)) {
      response.set_status(pb::kFailed);
      response.set_msg("invalid email");
      return true;
    }

    // useful check in dev mode
    if (scissy::Config::instance().crackPassword()) {
      const char * errmsg = ::FascistCheck(request.password().c_str(),
                                           ::GetDefaultCracklibDict());
      if (errmsg) {
        response.set_status(pb::kFailed);
        response.set_msg(errmsg);
        return true;
      }
    }

    mimosa::stream::Sha512 sha512;
    sha512.write(request.password().data(), request.password().size());

    auto stmt = Db::prepare(
      "insert or fail into users (login, email, password)"
      " values (?, ?, ?)");
    int err = stmt.bind(request.user(), request.email(),
                        (const void*)sha512.digest(), sha512.digestLen()).step();

    if (err == SQLITE_CONSTRAINT) {
      response.set_status(pb::kFailed);
      response.set_msg("failed to register, try another username/email");
      return true;
    }

    if (err != SQLITE_DONE) {
      response.set_status(pb::kFailed);
      response.set_msg("failed to register, internal error");
      return true;
    }

    response.set_status(pb::kSucceed);
    return true;
  }

  bool
  Service::userAuth(pb::UserAuth & request, pb::Session & response)
  {
    uint64_t    user_id   = 0;

    // load the password from the db
    {
      const void *blob      = nullptr;
      int         blob_size = 0;
      auto stmt             = Db::prepare(
        "select password, user_id from users where login = ?");
      if (!stmt.bind(request.user()).fetch(&blob, &blob_size, &user_id)) {
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
        mimosa::log::error("invalid password hash for user %s, (%d, %d), ",
                           request.user(), blob_size, sha512.digestLen());
        response.set_status(pb::kFailed);
        response.set_msg("invalid password");
        return true;
      }
    }

    //////////////////////////////////
    // auth succeed, generate token //
    //////////////////////////////////

    // generate a random value
    char auth[32];
    if (::gnutls_rnd(GNUTLS_RND_NONCE, auth, sizeof (auth)) < 0) {
      response.set_status(pb::kFailed);
      response.set_msg("2 internal error, please contact your administrator");
      return true;
    }

    std::string auth16 = mimosa::stream::filter<mimosa::stream::Base16Encoder>(
      auth, sizeof (auth));

    mimosa::stream::Sha512 sha512;
    sha512.write(request.password().data(), request.password().size());

    {
      // save the token in the db
      auto stmt = Db::prepare(
        "insert or fail into users_auths (user_id, token, ts_start)"
        " values (?, ?, ?)");
      stmt.bind((int)user_id, (const void*)auth, sizeof (auth), ::time(0));
      if (stmt.step() != SQLITE_DONE) {
        response.set_status(pb::kFailed);
        response.set_msg("3 internal error, please contact your administrator");
        return true;
      }
    }

    response.set_user(request.user());
    response.set_token(auth16);
    response.set_status(pb::kSucceed);
    return true;
  }

  static bool
  userGetSession(const pb::UserAuthToken & request,
                 pb::Session & response)
  {
    int         role_id;
    uint64_t    user_id;
    std::string token = mimosa::stream::filter<mimosa::stream::Base16Decoder>(request.token());
    auto stmt = Db::prepare(
      "select role_id, user_id"
      "  from users natural join users_auths"
      "  where login = ?");
    stmt.bind(request.user(), (const void *)token.data(), token.size());
    if (!stmt.fetch(&role_id, &user_id))
      return true;

    response.set_user(request.user());
    response.set_role((pb::Role)role_id);
    response.set_user_id(user_id);
    return true;
  }

  bool
  Service::userCheckAuthToken(pb::UserAuthToken & request,
                              pb::Session & response)
  {
    std::string email;
    int         role_id;
    uint64_t    user_id;
    std::string token = mimosa::stream::filter<mimosa::stream::Base16Decoder>(request.token());
    auto stmt = Db::prepare(
      "select email, role_id, user_id"
      "  from users natural join users_auths"
      "  where login = ?");
    stmt.bind(request.user(), (const void *)token.data(), token.size());
    if (!stmt.fetch(&email, &role_id, &user_id)) {
      response.set_status(pb::kFailed);
      response.set_msg("session not found");
      return true;
    }

    response.set_user(request.user());
    response.set_token(request.token());
    response.set_email(email);
    response.set_role((pb::Role)role_id);
    response.set_user_id(user_id);
    response.set_status(pb::kSucceed);
    return true;
  }

  bool
  Service::userRevokeAuthToken(pb::UserAuthToken & request,
                               pb::StatusMsg & response)
  {
    uint64_t user_id;

    if (!Db::userGetId(request.user(), &user_id)) {
      response.set_status(pb::kSucceed);
      response.set_msg("user not found");
      return true;
    }

    auto token = mimosa::stream::filter<mimosa::stream::Base16Decoder>(request.token());
    auto stmt = Db::prepare("delete from users_auths where user_id = ? and token = ?");
    stmt.bind(user_id, (const void *)token.data(), token.size()).step();

    response.set_status(pb::kSucceed);
    return true;
  }

  bool
  Service::groupCreate(pb::GroupCreate & request,
                       pb::GroupInfo & response)
  {
    pb::Session session;
    std::string err;
    int64_t grp_id;

    if (!userGetSession(request.auth(), session)) {
      response.set_status(pb::kInvalidSession);
      response.set_msg("invalid session");
      return true;
    }

    if (!Db::groupCreate(request.grp(), request.desc(), session.user(), &err)) {
      response.set_status(pb::kFailed);
      response.set_msg(err);
      return true;
    }

    if (!Db::groupGetId(request.grp(), &grp_id)) {
      response.set_status(pb::kFailed);
      response.set_msg("failed to get group id");
      return true;
    }

    response.set_status(pb::kSucceed);
    response.set_grp_id(grp_id);
    response.set_size(1);
    response.set_grp(request.grp());
    response.set_desc(request.desc());
    return true;
  }

  static bool isGroupOwner(const pb::Session & session,
                           const std::string & grp)
  {
    pb::Role role;

    if (session.role() == pb::kOwner)
      return true;

    if (!Db::groupGetUserRole(grp, session.user(), &role))
      return false;

    return role == pb::kOwner;
  }

  bool
  Service::groupDelete(pb::GroupDelete & request,
                       pb::StatusMsg & response)
  {
    pb::Session session;

    if (!userGetSession(request.auth(), session)) {
      response.set_status(pb::kInvalidSession);
      response.set_msg("invalid session");
      return true;
    }

    if (!isGroupOwner(session, request.grp())) {
        response.set_status(pb::kFailed);
        response.set_msg("insufficient rights");
        return true;
    }

    if (!Db::groupDelete(request.grp())) {
      response.set_status(pb::kFailed);
      response.set_msg("failed to delete group");
      return true;
    }

    response.set_status(pb::kSucceed);
    return true;
  }

  bool
  Service::groupAddUser(pb::GroupAddUser & request,
                        pb::StatusMsg & response)
  {
    pb::Session session;

    if (!userGetSession(request.auth(), session)) {
      response.set_status(pb::kInvalidSession);
      response.set_msg("invalid session");
      return true;
    }

    if (!isGroupOwner(session, request.grp())) {
        response.set_status(pb::kFailed);
        response.set_msg("insufficient rights");
        return true;
    }

    if (!Db::groupAddUser(request.grp(), request.user(), request.role())) {
      response.set_status(pb::kFailed);
      response.set_msg("failed to add user to group");
      return true;
    }

    response.set_status(pb::kSucceed);
    return true;
  }

  bool
  Service::groupRemoveUser(pb::GroupRemoveUser & request,
                           pb::StatusMsg & response)
  {
    pb::Session session;

    if (!userGetSession(request.auth(), session)) {
      response.set_status(pb::kInvalidSession);
      response.set_msg("invalid session");
      return true;
    }

    if (!isGroupOwner(session, request.grp())) {
        response.set_status(pb::kFailed);
        response.set_msg("insufficient rights");
        return true;
    }

    Db::groupRemoveUser(request.grp(), request.user());
    response.set_status(pb::kSucceed);
    return true;
  }

  bool
  Service::groupsList(pb::GroupSelector & request,
                      pb::GroupList & response)
  {
    std::string grp;
    std::string desc;
    uint64_t    grp_id;
    uint64_t    size;
    auto stmt = Db::prepare(
      "select name, desc, group_id, count(user_id)"
      " from groups natural left outer join groups_users"
      " group by group_id");

    while (stmt.fetch(&grp, &desc, &grp_id, &size)) {
      auto msg = response.add_grps();
      msg->set_grp(grp);
      msg->set_grp_id(grp_id);
      msg->set_desc(desc);
      msg->set_size(size);
    }

    response.set_status(pb::kSucceed);
    return true;
  }

  bool
  Service::groupGetInfo(pb::GroupSelector & request,
                        pb::GroupInfo & response)
  {
    std::string grp;
    std::string desc;
    uint64_t    grp_id;
    uint64_t    size;
    auto stmt = Db::prepare(
      "select name, desc, group_id, count(*)"
      " from groups_users_view"
      " where group_id = ? or name = ?"
      " limit 1");

    if (!stmt.bind(request.grp_id(), request.grp())
        .fetch(&grp, &desc, &grp_id, &size)) {
      response.set_status(pb::kFailed);
      response.set_msg("group not found");
      return true;
    }
    response.set_grp(grp);
    response.set_grp_id(grp_id);
    response.set_size(size);
    response.set_desc(desc);
    response.set_status(pb::kSucceed);
    return true;
  }

  bool
  Service::groupUserList(pb::GroupUserSelector & request,
                         pb::GroupUserList & response)
  {
    std::string user;
    uint64_t    user_id;
    pb::Role    role;
    auto stmt = Db::prepare(
      "select user, user_id, role_id"
      " from groups_users_view"
      " where group_id = ? or name = ?");

    while (stmt.bind(request.grp_id(), request.grp())
           .fetch(&user, &user_id, reinterpret_cast<int*> (&role))) {
      auto msg = response.add_users();
      msg->set_user(user);
      msg->set_user_id(user_id);
      msg->set_role(role);
    }
    response.set_status(pb::kSucceed);
    return true;
  }
}
