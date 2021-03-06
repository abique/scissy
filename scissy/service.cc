#include <ctime>
#include <sstream>

#include <re2/re2.h>

#include <crack.h>

#include <gnutls/gnutls.h>
#include <gnutls/crypto.h>

#include <mimosa/git/blob.hh>
#include <mimosa/git/commit.hh>
#include <mimosa/git/diff.hh>
#include <mimosa/git/patch.hh>
#include <mimosa/git/reference.hh>
#include <mimosa/git/repository.hh>
#include <mimosa/git/revwalk.hh>
#include <mimosa/git/tree-entry.hh>
#include <mimosa/git/tree.hh>
#include <mimosa/http/mime-db.hh>
#include <mimosa/http/server-channel.hh>
#include <mimosa/http/time.hh>
#include <mimosa/log/log.hh>
#include <mimosa/stream/base16-decoder.hh>
#include <mimosa/stream/base16-encoder.hh>
#include <mimosa/stream/hash.hh>
#include <mimosa/stream/string-stream.hh>
#include <mimosa/uptime.hh>

#include "config.hh"
#include "db.hh"
#include "service.hh"
#include "repositories.hh"
#include "gen-authorized-keys.hh"
#include "log.hh"
#include "session-handler.hh"
#include "helpers.hh"

#define AUTHENTICATE_USER()                                     \
  pb::Session session;                                          \
                                                                \
  if (!scissy::userGetSession(session))                         \
    throwStatusMsg(pb::kInvalidSession, "invalid session");

#define SET_GRP_ID(Msg)                                                 \
  do {                                                                  \
    if ((Msg).has_grp_id())                                             \
      break;                                                            \
    if (!(Msg).has_grp())                                               \
      throwStatusMsg(pb::kFailed, "need to specify grp or grp_id");     \
    int64_t grp_id__;                                                   \
    if (!Db::groupGetId((Msg).grp(), &grp_id__))                        \
      throwStatusMsg(pb::kNotFound, "group not found");                 \
    (Msg).set_grp_id(grp_id__);                                         \
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
    do {                                                \
      pb::Role role;                                    \
                                                        \
      if (!Db::repoGetUserRole(                         \
            RepoId, UserId, &role)) {                   \
        response.set_msg("database error");             \
        response.set_status(pb::kFailed);               \
        return false;                                   \
      }                                                 \
                                                        \
      if (role < RoleId) {                              \
        log->warning("insufficient rights: repo_id: %d, user_id: %d, "  \
                     "role: %d, expected: %d", RepoId, UserId, role, RoleId); \
        response.set_msg("insufficient rights");        \
        response.set_status(pb::kInsufficientRight);    \
        return true;                                    \
      }                                                 \
    } while (0)

#define CHECK_PUBLIC_REPO(Msg)                                          \
    do {                                                                \
      bool is_public;                                                   \
                                                                        \
      SET_REPO_ID(request);                                             \
                                                                        \
      if (!Db::repoIsPublic(request.repo_id(), &is_public)) {           \
        response.set_status(pb::kNotFound);                             \
        response.set_msg("repository not found");                       \
        return true;                                                    \
      }                                                                 \
                                                                        \
      if (!is_public) {                                                 \
        AUTHENTICATE_USER();                                            \
        CHECK_REPO_ROLE(request.repo_id(), session.user_id(),           \
                        pb::kReader);                                   \
      }                                                                 \
    } while (0)


namespace scissy
{

  /////////////
  // Helpers //
  /////////////

  static void
  throwStatusMsg(pb::Status status, const std::string & msg)
  {
    pb::StatusMsg err;
    err.set_msg(msg);
    err.set_status(status);
    throw err;
  }

  static bool
  userGetSession(pb::Session & response)
  {
    auto & session = SessionHandler::threadSession();
    response.set_user(session.user());
    response.set_role(session.role());
    response.set_user_id(session.userId());
    return true;
  }

  static bool
  userCheckPassword(const std::string &password, pb::StatusMsg &response)
  {
    if (password.size() < 5) {
      response.set_status(pb::kFailed);
      response.set_msg("password too short (5 characters at least)");
      return false;
    }

    if (password.size() > 32) {
      response.set_status(pb::kFailed);
      response.set_msg("password too long (32 characters at most)");
      return false;
    }

    // useful check in dev mode
    if (scissy::Config::instance().crackPasswords()) {
      const char * errmsg = ::FascistCheck(password.c_str(), ::GetDefaultCracklibDict());
      if (errmsg) {
        response.set_status(pb::kFailed);
        response.set_msg(errmsg);
        return false;
      }
    }

    return true;
  }

  /////////////
  // Service //
  /////////////

  bool
  Service::serviceInfo(pb::Void & /*request*/,
                       pb::ServiceInfo & response)
  {
    response.set_clone_user(Config::instance().cloneUser());
    response.set_clone_host(Config::instance().cloneHost());
    response.set_version(SCISSY_VERSION);
    response.set_start_time(
      (mimosa::realTimeCoarse() - (mimosa::uptime())) / mimosa::millisecond);
    response.set_is_register_enabled(Config::instance().isRegisterEnabled());
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

    const std::string &login = request.user();
    const std::string &email = request.email();

    if (!Config::instance().isRegisterEnabled()) {
      response.set_status(pb::kForbidden);
      response.set_msg("registration disabled");
      return true;
    }

    if (login.size() > 64) {
      response.set_status(pb::kFailed);
      response.set_msg("login too long (64 characters at most)");
      return true;
    }

    if (!userCheckPassword(request.password(), response))
      return true;

    if (!RE2::FullMatch(login, login_match)) {
      response.set_status(pb::kFailed);
      response.set_msg("login contains invalid characters");
      return true;
    }

    if (!RE2::FullMatch(email, email_match)) {
      response.set_status(pb::kFailed);
      response.set_msg("invalid email");
      return true;
    }

    // generate a salt
    char salt[16];
    if (::gnutls_rnd(GNUTLS_RND_NONCE, salt, sizeof (salt)) < 0) {
      response.set_status(pb::kFailed);
      response.set_msg("internal error, please contact your administrator");
      return true;
    }

    std::string salt16 = mimosa::stream::filter<mimosa::stream::Base16Encoder>(
      salt, sizeof (salt));

    // compute hash
    mimosa::stream::Sha3_512 hash;
    hash.write(salt, sizeof (salt));
    hash.write(request.password().data(), request.password().size());

    std::string hash16 = mimosa::stream::filter<mimosa::stream::Base16Encoder>(
      hash.digest(), hash.digestLen());

    static const std::string hash_name("SHA3-512");

    // save user
    auto stmt = Db::prepare(
      "insert or fail into users (login, email, password_type,"
      "                           password_salt, password_hash)"
      " values (?, ?, ?, ?, ?)");

    int err = stmt.bind(login, email, hash_name, salt16, hash16).step();

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
  Service::userSetPassword(pb::UserPasswordChange &request, pb::StatusMsg &response)
  {
    if (!userCheckPassword(request.new_password(), response))
      return true;

    AUTHENTICATE_USER();

    int64_t user_id = 0;
    if (!Db::userGetId(request.user(), &user_id)) {
      response.set_status(pb::kNotFound);
      return true;
    }

    if (session.role() != pb::kOwner &&
        (true /* TODO: same user and old_password matches */)) {
      response.set_status(pb::kForbidden);
      return true;
    }

    // TODO: set user password
    return true;
  }

  bool
  Service::userAuth(pb::UserAuth & request, pb::Session & response)
  {
    int64_t    user_id   = 0;

    // Check if auth works
    if (!Config::instance().authenticator().auth(request, response)) {
      response.set_status(pb::kFailed);
      return true;
    }

    // Ensure the user has an account
    if (!Db::userGetId(request.user(), &user_id)) {
      response.set_status(pb::kFailed);
      response.set_msg("2 internal error, please contact your administrator");
      return true;
    }

    //////////////////////////////////
    // auth succeed, generate token //
    //////////////////////////////////

    // generate a random value as token
    char auth[32];
    if (::gnutls_rnd(GNUTLS_RND_NONCE, auth, sizeof (auth)) < 0) {
      response.set_status(pb::kFailed);
      response.set_msg("internal error, please contact your administrator");
      return true;
    }

    std::string auth16 = mimosa::stream::filter<mimosa::stream::Base16Encoder>(
      auth, sizeof (auth));

    {
      // save the token in the db
      auto stmt = Db::prepare(
        "insert or fail into users_auths (user_id, token, ts_start)"
        " values (?, ?, ?)");
      stmt.bind((int)user_id, auth16, ::time(0));
      if (stmt.step() != SQLITE_DONE) {
        response.set_status(pb::kFailed);
        response.set_msg("3 internal error, please contact your administrator");
        return true;
      }
    }

    {
      auto & session = SessionHandler::threadSession();
      bool secure = session.httpResponse().channel().isSsl();

      std::string expire = mimosa::http::time(
        ::time(nullptr) + 7 * 60 * 60 * Config::instance().cookieDuration());

      // set cookies
      auto cookie = new mimosa::http::Cookie;
      cookie->setKey("user");
      cookie->setValue(request.user());
      cookie->setHttpOnly(true);
      cookie->setSecure(secure);
      cookie->setPath("/");
      cookie->setExpires(expire);
      session.httpResponse().addCookie(cookie);

      cookie = new mimosa::http::Cookie;
      cookie->setKey("token");
      cookie->setValue(auth16);
      cookie->setHttpOnly(true);
      cookie->setSecure(secure);
      cookie->setPath("/");
      cookie->setExpires(expire);
      session.httpResponse().addCookie(cookie);
    }

    response.set_user(request.user());
    response.set_status(pb::kSucceed);
    return true;
  }

  bool
  Service::userGetSession(pb::Void & /*request*/,
                          pb::Session & response)
  {
    AUTHENTICATE_USER();

    response.set_user(session.user());
    response.set_email(session.email());
    response.set_role(session.role());
    response.set_user_id(session.user_id());
    response.set_status(pb::kSucceed);
    return true;
  }

  bool
  Service::userAddSshKey(pb::UserSshKey & request,
                         pb::StatusMsg & response)
  {
    AUTHENTICATE_USER();

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
    AUTHENTICATE_USER();

    auto stmt = Db::prepare("delete from users_ssh_keys"
                            " where user_id = ? and ssh_key_id = ?");
    if (!stmt.bind(session.user_id(), request.key().key_id()).step()) {
      response.set_status(pb::kFailed);
      response.set_msg("SQL error");
      return true;
    }

    genAuthorizedKeys();
    response.set_status(pb::kSucceed);
    return true;
  }

  bool
  Service::userGetSshKeys(pb::UserSshKeySelector & /*request*/,
                          pb::UserSshKeys & response)
  {
    uint64_t       key_id;
    pb::SshKeyType type;
    std::string    key;
    std::string    desc;

    AUTHENTICATE_USER();

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

    if (!Repositories::instance().create(
          request.name(), request.desc(), request.is_public(),
          session.user_id(), &repo_id, &errmsg)) {
      response.set_status(pb::kFailed);
      response.set_msg(errmsg);
      return true;
    }

    response.set_name(request.name());
    response.set_desc(request.desc());
    response.set_id(repo_id);
    response.set_is_public(request.is_public());
    response.set_status(pb::kSucceed);
    return true;
  }

  bool
  Service::repoUpdate(pb::RepoUpdate & request,
                      pb::StatusMsg & response)
  {
    AUTHENTICATE_USER();
    SET_REPO_ID(request);
    CHECK_REPO_ROLE(request.repo_id(), session.user_id(), pb::kOwner);

    auto stmt = Db::prepare("update repos set"
                            " name = ?,"
                            " desc = ?,"
                            " is_public = ?"
                            " where repo_id = ?");
    stmt.bind(request.name(), request.desc(), request.is_public(),
              request.repo_id()).step();

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

    if (!Db::repoAddUser(
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

    if (!Db::repoRemoveUser(
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

    if (!Db::repoAddGroup(
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

    if (!Db::repoRemoveGroup(
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

      mimosa::git::Repository repo(Repositories::instance().getRepoPath(id));
      if (repo)
        msg->set_last_commit_time(repo.lastCommitTime());
    }

    response.set_status(pb::kSucceed);
    return true;
  }

  bool
  Service::repoGetInfo(pb::RepoSelector & request,
                       pb::RepoInfo & response)
  {
    CHECK_PUBLIC_REPO(request);

    std::string name;
    std::string desc;
    int         is_public;

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
    mimosa::git::Repository repo(Repositories::instance().getRepoPath(request.repo_id()));
    if (repo)
      response.set_last_commit_time(repo.lastCommitTime());
    response.set_status(pb::kSucceed);
    return true;
  }

  bool
  Service::repoListMembers(pb::RepoSelector & request,
                           pb::MemberList & response)
  {
    CHECK_PUBLIC_REPO(request);

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

  bool
  Service::repoListBranches(pb::RepoSelector & request,
                            pb::GitBranches & response)
  {
    CHECK_PUBLIC_REPO(request);

    mimosa::git::Repository repo(Repositories::instance().getRepoPath(request.repo_id()));
    if (!repo) {
      response.set_status(pb::kInternalError);
      response.set_msg("internal error");
      return true;
    }

    git_branch_iterator *it  = nullptr;
    git_reference       *ref = nullptr;
    git_branch_t        branch_type;
    git_branch_iterator_new(&it, repo, GIT_BRANCH_LOCAL);
    while (!git_branch_next(&ref, &branch_type, it)) {
      auto branch = response.add_branches();
      branch->set_name(git_reference_name(ref));
      git_reference_free(ref);
    }
    git_branch_iterator_free(it);

    response.set_status(pb::kSucceed);
    return true;
  }

  bool
  Service::repoGetCommit(pb::CommitSelector & request,
                         pb::GitCommit & response)
  {
    CHECK_PUBLIC_REPO(request);

    mimosa::git::Repository repo(Repositories::instance().getRepoPath(request.repo_id()));
    if (!repo) {
      response.set_status(pb::kInternalError);
      response.set_msg("internal error");
      return true;
    }

    git_oid oid;
    if (!mimosa::git::referenceToOid(repo, request.revision(), &oid)) {
      response.set_status(pb::kNotFound);
      response.set_msg("commit not found");
      return true;
    }

    mimosa::git::Commit commit(repo, &oid);
    if (!commit) {
      response.set_status(pb::kNotFound);
      response.set_msg("commit not found");
      return true;
    }

    copyCommit(commit, &response);
    response.set_status(pb::kSucceed);
    return true;
  }

  bool
  Service::repoGetLog(pb::LogSelector & request,
                      pb::GitLog & response)
  {
    CHECK_PUBLIC_REPO(request);

    mimosa::git::Repository repo(Repositories::instance().getRepoPath(request.repo_id()));
    if (!repo) {
      response.set_status(pb::kInternalError);
      response.set_msg("internal error");
      return true;
    }

    mimosa::git::Revwalk walk(repo);
    git_oid    oid;

    if (!walk) {
      response.set_status(pb::kInternalError);
      response.set_msg("failed to initialize revision walker");
      return true;
    }

    if (request.has_revision()) {
      if (!mimosa::git::referenceToOid(repo, request.revision(), &oid)) {
        response.set_status(pb::kNotFound);
        response.set_msg("commit not found");
        return true;
      }
      git_revwalk_push(walk, &oid);
    } else {
      git_revwalk_push_head(walk);
    }

    git_revwalk_sorting(walk, GIT_SORT_TIME);

    if (!request.has_limit() || request.limit() == 0 ||
        request.limit() > Config::instance().gitLogLimit())
      request.set_limit(Config::instance().gitLogLimit());

    if (!request.has_offset())
      request.set_offset(0);

    for (uint32_t i = 0; i < request.limit() + request.offset(); ++i) {
      if (git_revwalk_next(&oid, walk))
        break;
      if (i < request.offset())
        continue;
      mimosa::git::Commit commit(repo, &oid);
      if (!commit)
        continue;
      copyCommit(commit, response.add_commits());
    }

    response.set_status(pb::kSucceed);
    return true;
  }

  bool
  Service::repoGetTree(pb::TreeSelector & request,
                       pb::GitTree & response)
  {
    CHECK_PUBLIC_REPO(request);

    mimosa::git::Repository repo(Repositories::instance().getRepoPath(request.repo_id()));
    if (!repo) {
      response.set_status(pb::kInternalError);
      response.set_msg("internal error");
      return true;
    }

    git_oid oid;
    if (!mimosa::git::referenceToOid(repo, request.revision(), &oid)) {
      response.set_status(pb::kNotFound);
      response.set_msg("commit not found");
      return true;
    }

    mimosa::git::Commit commit(repo, &oid);
    if (!commit) {
      response.set_status(pb::kNotFound);
      response.set_msg("commit not found");
      return true;
    }

    mimosa::git::Tree tree(repo, git_commit_tree_id(commit), request.directory());
    if (!tree) {
      response.set_status(pb::kNotFound);
      response.set_msg("tree not found");
      return true;
    }

    char oid_str[GIT_OID_HEXSZ + 1];
    for (size_t i = 0; i < git_tree_entrycount(tree); ++i) {
      const git_tree_entry *entry = git_tree_entry_byindex(tree, i);
      if (!entry)
        break;
      auto e = response.add_entries();

      e->set_name(git_tree_entry_name(entry));
      e->set_type((pb::GitOtype)git_tree_entry_type(entry));
      e->set_mode((pb::GitFileMode)git_tree_entry_filemode(entry));

      auto entry_id = git_tree_entry_id(entry);
      git_oid_tostr(oid_str, sizeof (oid_str), entry_id);
      e->set_id(oid_str);
    }

    response.set_status(pb::kSucceed);
    return true;
  }

  bool
  Service::repoGetBlob(pb::BlobSelector & request,
                       pb::GitBlob & response)
  {
    CHECK_PUBLIC_REPO(request);

    mimosa::git::Repository repo(Repositories::instance().getRepoPath(request.repo_id()));
    if (!repo) {
      response.set_status(pb::kInternalError);
      response.set_msg("internal error");
      return true;
    }

    git_oid oid;
    if (!mimosa::git::referenceToOid(repo, request.revision(), &oid)) {
      response.set_status(pb::kNotFound);
      response.set_msg("commit not found");
      return true;
    }

    mimosa::git::Commit commit(repo, &oid);
    if (!commit) {
      response.set_status(pb::kNotFound);
      response.set_msg("commit not found");
      return true;
    }

    mimosa::git::Tree tree(repo, git_commit_tree_id(commit));
    if (!tree) {
      response.set_status(pb::kNotFound);
      response.set_msg("tree not found");
      return true;
    }

    mimosa::git::TreeEntry entry;
    if (git_tree_entry_bypath(entry.ref(), tree, request.path().c_str())) {
      response.set_status(pb::kNotFound);
      response.set_msg("tree entry not found");
      return true;
    }

    mimosa::git::Blob blob;
    if (git_blob_lookup(blob.ref(), repo, git_tree_entry_id(entry))) {
      response.set_status(pb::kNotFound);
      response.set_msg("blob not found");
      return true;
    }

    if (git_blob_is_binary(blob)) {
      response.set_is_binary(true);
    } else {
      response.set_is_binary(false);
      response.set_data(git_blob_rawcontent(blob), git_blob_rawsize(blob));
    }

    response.set_content_type(mimosa::http::MimeDb::instance().mimeType(
                                request.path()));

    response.set_status(pb::kSucceed);
    return true;
  }

  bool
  Service::repoGetPatch(pb::DiffSelector & request,
                        pb::GitPatch & response)
  {
    CHECK_PUBLIC_REPO(request);

    mimosa::git::Repository repo(Repositories::instance().getRepoPath(request.repo_id()));
    if (!repo) {
      response.set_status(pb::kInternalError);
      response.set_msg("internal error");
      return true;
    }

    git_oid oid;
    if (!mimosa::git::referenceToOid(repo, request.revision_new(), &oid)) {
      response.set_status(pb::kNotFound);
      response.set_msg("commit not found");
      return true;
    }

    mimosa::git::Commit commit_new(repo, &oid);
    if (!commit_new) {
      response.set_status(pb::kNotFound);
      response.set_msg("commit not found");
      return true;
    }

    mimosa::git::Tree tree_new(repo, git_commit_tree_id(commit_new));
    if (!tree_new) {
      response.set_status(pb::kNotFound);
      response.set_msg("tree not found");
      return true;
    }

    mimosa::git::Commit commit_old;

    if (request.has_revision_old()) {
      if (!mimosa::git::referenceToOid(repo, request.revision_old(), &oid) ||
          git_commit_lookup(commit_old.ref(), repo, &oid)) {
        response.set_status(pb::kNotFound);
        response.set_msg("commit not found");
        return true;
      }
    } else {
      // set the old revision to the first parent?
      if (git_commit_parentcount(commit_new) > 0 &&
          git_commit_parent(commit_old.ref(), commit_new, 0)) {
        response.set_status(pb::kNotFound);
        response.set_msg("commit not found");
        return true;
      }
    }

    mimosa::git::Tree tree_old(
      repo, commit_old ? git_commit_tree_id(commit_old) : nullptr);

    mimosa::git::Diff diff;
    if (git_diff_tree_to_tree(diff.ref(), repo, tree_old, tree_new, NULL))
      throwStatusMsg(pb::kInternalError, "diff error");

    std::ostringstream patch_ss;
    for (size_t i = 0; i < git_diff_num_deltas(diff); ++i) {
      mimosa::git::Patch patch;
      if (git_patch_from_diff(patch.ref(), diff, i))
        continue;

      git_buf buf = { nullptr, 0, 0};
      if (git_patch_to_buf(&buf, patch) || !buf.ptr)
        continue;
      patch_ss << buf.ptr;
      git_buf_free(&buf);
    }

    response.set_status(pb::kSucceed);
    response.set_data(patch_ss.str());
    return true;
  }

  static int
  getTagsCb(const char *name, git_oid *oid, void *payload)
  {
    char oid_str[GIT_OID_HEXSZ + 1];
    pb::GitTags * response = reinterpret_cast<pb::GitTags *>(payload);
    auto tag = response->add_tags();
    tag->set_name(name);
    git_oid_tostr(oid_str, sizeof (oid_str), oid);
    tag->set_revision(oid_str);
    return 0;
  }

  bool
  Service::repoGetTags(pb::RepoSelector & request,
                       pb::GitTags & response)
  {
    CHECK_PUBLIC_REPO(request);

    mimosa::git::Repository repo(Repositories::instance().getRepoPath(request.repo_id()));
    if (!repo) {
      response.set_status(pb::kInternalError);
      response.set_msg("internal error");
      return true;
    }
    git_tag_foreach(repo, getTagsCb, &response);
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
    bool     is_public = false;
    bool     is_admin = false;

    if (!Db::repoGetId(request.repo_name(), &repo_id)) {
      response.set_status(pb::kNotFound);
      return true;
    }

    if (!Db::userIsAdmin(request.user_id(), &is_admin))
      is_admin = false;
    if (!Db::repoGetUserRole(repo_id, request.user_id(), &role))
      role = pb::kNone;
    if (!Db::repoIsPublic(repo_id, &is_public))
      is_public = false;

    response.set_repo_path(Repositories::instance().getRepoPath(repo_id));
    response.set_role(role);
    response.set_is_public(is_public);
    response.set_is_admin(is_admin);
    response.set_status(pb::kSucceed);
    return true;
  }
}
