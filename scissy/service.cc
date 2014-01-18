#include <ctime>

#include <re2/re2.h>

#include <crack.h>

#include <mimosa/stream/hash.hh>
#include <mimosa/stream/string-stream.hh>
#include <mimosa/stream/base16-decoder.hh>
#include <mimosa/stream/base16-encoder.hh>
#include <mimosa/log/log.hh>
#include <mimosa/uptime.hh>

#include "config.hh"
#include "db.hh"
#include "service.hh"
#include "repositories.hh"
#include "gen-authorized-keys.hh"
#include "log.hh"

#define AUTHENTICATE_USER()                             \
  pb::Session session;                                  \
                                                        \
  if (!userGetSession(request.auth(), session)) {       \
    response.set_status(pb::kInvalidSession);           \
    response.set_msg("invalid session");                \
    return true;                                        \
  }

#define SET_GRP_ID(Msg)                                         \
  do {                                                          \
    if ((Msg).has_grp_id())                                     \
      break;                                                    \
    if (!(Msg).has_grp()) {                                     \
      response.set_status(pb::kFailed);                         \
      response.set_msg("need to specify grp or grp_id");        \
      return true;                                              \
    }                                                           \
    int64_t grp_id__;                                           \
    if (!Db::groupGetId((Msg).grp(), &grp_id__)) {              \
      response.set_status(pb::kNotFound);                       \
      response.set_msg("group not found");                      \
      return true;                                              \
    }                                                           \
    (Msg).set_grp_id(grp_id__);                                 \
  } while (0)

#define SET_USER_ID(Msg)                                        \
  do {                                                          \
    if ((Msg).has_user_id())                                    \
      break;                                                    \
    if (!(Msg).has_user()) {                                    \
      response.set_status(pb::kFailed);                         \
      response.set_msg("need to specify user or user_id");      \
      return true;                                              \
    }                                                           \
    int64_t user_id__;                                          \
    if (!Db::userGetId((Msg).user(), &user_id__)) {             \
      response.set_status(pb::kNotFound);                       \
      response.set_msg("user not found");                       \
      return true;                                              \
    }                                                           \
    (Msg).set_user_id(user_id__);                               \
  } while (0)

#define SET_REPO_ID(Msg)                                        \
  do {                                                          \
    if ((Msg).has_repo_id())                                    \
      break;                                                    \
    if (!(Msg).has_repo()) {                                    \
      response.set_status(pb::kFailed);                         \
      response.set_msg("need to specify repo or repo_id");      \
      return true;                                              \
    }                                                           \
    int64_t repo_id__;                                          \
    if (!Db::repoGetId((Msg).repo(), &repo_id__)) {             \
      response.set_status(pb::kNotFound);                       \
      response.set_msg("repository not found");                 \
      return true;                                              \
    }                                                           \
    (Msg).set_repo_id(repo_id__);                               \
  } while (0)

#define CHECK_REPO_ROLE(RepoId, UserId, RoleId)         \
      do {                                              \
        pb::Role role;                                  \
                                                        \
        if (!Repositories::instance().getUserRole(      \
              RepoId, UserId, &role)) {                 \
          response.set_msg("database error");           \
          response.set_status(pb::kFailed);             \
          return false;                                 \
        }                                               \
                                                        \
        if (role < RoleId) {                            \
          response.set_msg("insufficient rights");      \
          response.set_status(pb::kInsufficientRight);  \
          return true;                                  \
        }                                               \
      } while (0)

namespace scissy
{
  /////////////
  // Service //
  /////////////

  bool
  Service::serviceInfo(pb::Void & request,
                       pb::ServiceInfo & response)
  {
    response.set_clone_user(Config::instance().cloneUser());
    response.set_clone_host(Config::instance().cloneHost());
    response.set_version(SCISSY_VERSION);
    response.set_uptime(mimosa::uptime());
    return true;
  }

  ////////////////
  // User stuff //
  ////////////////

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
    if (scissy::Config::instance().crackPasswords()) {
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
    int64_t user_id;

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
  Service::userAddSshKey(pb::UserSshKey & request,
                         pb::StatusMsg & response)
  {
    pb::Session session;

    if (!userCheckAuthToken(*request.mutable_auth(), session) ||
        session.status() != pb::kSucceed) {
      response.set_status(pb::kInvalidSession);
      response.set_msg("invalid session");
      return false;
    }

    auto stmt = Db::prepare("insert into users_ssh_keys"
                            " (user_id, `key_type`, `key`, `desc`, ts_created)"
                            " values (?, ?, ?, ?, datetime('now'))");
    if (!stmt.bind(session.user_id(), request.key().type(), request.key().key(),
                   request.key().desc()).step()) {
      response.set_status(pb::kFailed);
      response.set_msg("SQL error");
      return true;
    }

    genAuthorizedKeys();
    response.set_status(pb::kSucceed);
    return true;
  }

  bool
  Service::userRemoveSshKey(pb::UserSshKey & request,
                            pb::StatusMsg & response)
  {
    pb::Session session;

    if (!userCheckAuthToken(*request.mutable_auth(), session) ||
        session.status() != pb::kSucceed) {
      response.set_status(pb::kInvalidSession);
      response.set_msg("invalid session");
      return false;
    }

    auto stmt = Db::prepare("delete from users_ssh_keys"
                            " where user_id = ? and ssh_key_id = ?");
    if (!stmt.bind(session.user_id(), request.key().key_id()).step()) {
      response.set_status(pb::kFailed);
      response.set_msg("SQL error");
      return true;
    }

    response.set_status(pb::kSucceed);
    return true;
  }

  bool
  Service::userGetSshKeys(pb::UserSshKeySelector & request,
                          pb::UserSshKeys & response)
  {
    pb::Session    session;
    uint64_t       key_id;
    pb::SshKeyType type;
    std::string    key;
    std::string    desc;

    if (!userCheckAuthToken(*request.mutable_auth(), session) ||
        session.status() != pb::kSucceed) {
      response.set_status(pb::kInvalidSession);
      response.set_msg("invalid session");
      return false;
    }

    auto stmt = Db::prepare("select ssh_key_id, `key_type`, `key`, `desc`"
                            " from users_ssh_keys"
                            " where user_id = ?");
    stmt.bind(session.user_id());
    while (stmt.fetch(&key_id, (int*)&type, &key, &desc)) {
      auto new_key = response.add_keys();
      new_key->set_key_id(key_id);
      new_key->set_type(type);
      new_key->set_key(key);
      new_key->set_desc(desc);
    }

    response.set_status(pb::kSucceed);
    return true;
  }

  ////////////////////////////
  // Group management stuff //
  ////////////////////////////

  bool
  Service::groupCreate(pb::GroupCreate & request,
                       pb::GroupInfo & response)
  {
    std::string err;
    int64_t grp_id;

    AUTHENTICATE_USER();

    if (!Db::groupCreate(request.grp(), request.desc(), &err)) {
      response.set_status(pb::kFailed);
      response.set_msg(err);
      return true;
    }

    if (!Db::groupGetId(request.grp(), &grp_id)) {
      response.set_status(pb::kFailed);
      response.set_msg("failed to get group id");
      return true;
    }

    if (!Db::groupGetId(request.grp(), &grp_id)) {
      response.set_status(pb::kFailed);
      response.set_msg("failed to get group id");
      return true;
    }

    if (!Db::groupAddUser(grp_id, session.user_id(), pb::kOwner)) {
      response.set_status(pb::kFailed);
      response.set_msg("failed to set owner");
      Db::groupDelete(grp_id);
      return true;
    }

    response.set_status(pb::kSucceed);
    response.set_grp_id(grp_id);
    response.set_size(1);
    response.set_grp(request.grp());
    response.set_desc(request.desc());
    return true;
  }

  static bool isGroupOwner(const pb::Session & session, int64_t grp_id)
  {
    pb::Role role = pb::kNone;

    if (session.role() == pb::kOwner)
      return true;

    if (!Db::groupGetUserRole(grp_id, session.user_id(), &role))
      return false;
    return role == pb::kOwner;
  }

  bool
  Service::groupDelete(pb::GroupDelete & request,
                       pb::StatusMsg & response)
  {
    AUTHENTICATE_USER();
    SET_GRP_ID(request);

    if (!isGroupOwner(session, request.grp_id())) {
      response.set_status(pb::kFailed);
      response.set_msg("insufficient rights");
      return true;
    }

    if (!Db::groupDelete(request.grp_id())) {
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
    AUTHENTICATE_USER();

    log->critical("has_grp_id: %d", request.has_grp_id());

    SET_GRP_ID(request);
    SET_USER_ID(request);

    if (!isGroupOwner(session, request.grp_id())) {
      response.set_status(pb::kFailed);
      response.set_msg("insufficient rights");
      return true;
    }

    if (!Db::groupAddUser(request.grp_id(), request.user_id(), request.role())) {
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
    AUTHENTICATE_USER();
    SET_GRP_ID(request);
    SET_USER_ID(request);

    if (!isGroupOwner(session, request.grp_id())) {
      response.set_status(pb::kFailed);
      response.set_msg("insufficient rights");
      return true;
    }

    Db::groupRemoveUser(request.grp_id(), request.user_id());
    response.set_status(pb::kSucceed);
    return true;
  }

  bool
  Service::groupsList(pb::GroupSelector & /*request*/,
                      pb::GroupList & response)
  {
    std::string grp;
    std::string desc;
    uint64_t    grp_id;
    uint64_t    size;
    auto stmt = Db::prepare(
      "select name, desc, group_id, count(user_id)"
      " from groups natural left outer join groups_users"
      " group by group_id"
      " order by name");

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
      " from groups natural join groups_users"
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
  Service::groupListMembers(pb::GroupMemberSelector & request,
                            pb::MemberList & response)
  {
    std::string user;
    uint64_t    user_id;
    pb::Role    role;
    auto stmt = Db::prepare(
      "select login, user_id, groups_users.role_id"
      " from groups natural join groups_users join users using (user_id)"
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

  ///////////////////////////
  // Repository management //
  ///////////////////////////

  bool
  Service::repoCreate(pb::RepoCreate & request,
                      pb::RepoInfo & response)
  {
    AUTHENTICATE_USER();

    std::string errmsg;
    int64_t repo_id;

    if (!Repositories::instance().create(request.name(), request.desc(),
                                         session.user_id(), &repo_id, &errmsg)) {
      response.set_status(pb::kFailed);
      response.set_msg(errmsg);
      return true;
    }

    response.set_name(request.name());
    response.set_desc(request.desc());
    response.set_id(repo_id);
    response.set_status(pb::kSucceed);
    return true;
  }

  bool
  Service::repoDelete(pb::RepoDelete & request,
                      pb::StatusMsg & response)
  {
    AUTHENTICATE_USER();
    SET_REPO_ID(request);
    CHECK_REPO_ROLE(request.repo_id(), session.user_id(), pb::kOwner);

    if (!Repositories::instance().remove(request.repo_id())) {
      response.set_msg("database error");
      response.set_status(pb::kFailed);
      return true;
    }

    response.set_status(pb::kSucceed);
    return true;
  }

  bool
  Service::repoAddUser(pb::RepoAddUser & request,
                       pb::StatusMsg & response)
  {
    AUTHENTICATE_USER();
    SET_REPO_ID(request);
    SET_USER_ID(request);
    CHECK_REPO_ROLE(request.repo_id(), session.user_id(), pb::kOwner);

    if (!Repositories::instance().addUser(
          request.repo_id(), request.user_id(), request.role())) {
      response.set_msg("");
      response.set_status(pb::kFailed);
      return true;
    }

    response.set_status(pb::kSucceed);
    return true;
  }

  bool
  Service::repoRemoveUser(pb::RepoRemoveUser & request,
                          pb::StatusMsg & response)
  {
    AUTHENTICATE_USER();
    SET_REPO_ID(request);
    SET_USER_ID(request);
    CHECK_REPO_ROLE(request.repo_id(), session.user_id(), pb::kOwner);

    if (!Repositories::instance().removeUser(
          request.repo_id(), request.user_id())) {
      response.set_msg("");
      response.set_status(pb::kFailed);
      return true;
    }

    response.set_status(pb::kSucceed);
    return true;
  }

  bool
  Service::repoAddGroup(pb::RepoAddGroup & request,
                        pb::StatusMsg & response)
  {
    AUTHENTICATE_USER();
    SET_REPO_ID(request);
    SET_GRP_ID(request);
    CHECK_REPO_ROLE(request.repo_id(), session.user_id(), pb::kOwner);

    if (!Repositories::instance().addGroup(
          request.repo_id(), request.grp_id(), request.role())) {
      response.set_msg("");
      response.set_status(pb::kFailed);
      return true;
    }

    response.set_status(pb::kSucceed);
    return true;
  }

  bool
  Service::repoRemoveGroup(pb::RepoRemoveGroup & request,
                           pb::StatusMsg & response)
  {
    AUTHENTICATE_USER();
    SET_REPO_ID(request);
    SET_GRP_ID(request);
    CHECK_REPO_ROLE(request.repo_id(), session.user_id(), pb::kOwner);

    if (!Repositories::instance().removeGroup(
          request.repo_id(), request.grp_id())) {
      response.set_msg("");
      response.set_status(pb::kFailed);
      return true;
    }

    response.set_status(pb::kSucceed);
    return true;
  }

  /////////////////////
  // Repository view //
  /////////////////////

  bool
  Service::reposList(pb::RepoSelector & /*request*/,
                     pb::RepoList & response)
  {
    std::string name;
    std::string desc;
    uint64_t    id;
    int         is_public;
    auto stmt = Db::prepare("select name, desc, repo_id, is_public"
                            " from repos order by name");

    while (stmt.fetch(&name, &desc, &id, &is_public)) {
      auto msg = response.add_repos();
      msg->set_name(name);
      msg->set_id(id);
      msg->set_desc(desc);
      msg->set_is_public(is_public);
    }

    response.set_status(pb::kSucceed);
    return true;
  }

  bool
  Service::repoGetInfo(pb::RepoSelector & request,
                       pb::RepoInfo & response)
  {
    std::string name;
    std::string desc;
    int         is_public;

    SET_REPO_ID(request);

    auto stmt = Db::prepare("select `name`, `desc`, is_public from repos"
                            " where repo_id = ?");
    stmt.bind(request.repo_id());

    if (!stmt.fetch(&name, &desc, &is_public)) {
      response.set_msg("repository not found");
      response.set_status(pb::kNotFound);
      return true;
    }

    response.set_id(request.repo_id());
    response.set_name(name);
    response.set_desc(desc);
    response.set_is_public(is_public);
    response.set_status(pb::kSucceed);
    return true;
  }

  bool
  Service::repoListMembers(pb::RepoSelector & request,
                           pb::MemberList & response)
  {
    int64_t id;
    std::string name;
    pb::Role role;

    {
      auto stmt = Db::prepare("select group_id, `name`, repos_groups.role_id"
                              " from repos_groups join groups using (group_id)"
                              " where repo_id = ?"
                              " order by `name`");
      stmt.bind(request.repo_id());
      while (stmt.fetch(&id, &name, reinterpret_cast<int *>(&role))) {
        auto msg = response.add_groups();
        msg->set_grp_id(id);
        msg->set_grp(name);
        msg->set_role(role);
      }
    }

    {
      auto stmt = Db::prepare("select user_id, `login`, repos_users.role_id"
                              " from repos_users join users using (user_id)"
                              " where repo_id = ?"
                              " order by `login`");
      stmt.bind(request.repo_id());
      while (stmt.fetch(&id, &name, reinterpret_cast<int *>(&role))) {
        auto msg = response.add_users();
        msg->set_user_id(id);
        msg->set_user(name);
        msg->set_role(role);
      }
    }

    response.set_status(pb::kSucceed);
    return true;
  }

  ///////////////
  // Ssh shell //
  ///////////////

  bool
  Service::shellControl(pb::ShellControl & request,
                        pb::ShellStatus & response)
  {
    int64_t  repo_id;
    pb::Role role;
    bool     is_public;

    if (!Repositories::instance().getId(request.repo_name(), &repo_id) ||
        !Repositories::instance().getUserRole(repo_id, request.user_id(), &role) ||
        !Repositories::instance().isPublic(repo_id, &is_public)) {
      response.set_status(pb::kNotFound);
      return true;
    }

    response.set_repo_path(Repositories::instance().getRepoPath(repo_id));
    response.set_role(role);
    response.set_is_public(is_public);
    response.set_status(pb::kSucceed);
    return true;
  }
}
