#include <sys/types.h>
#include <pwd.h>

#include <mimosa/options/options.hh>
#include <mimosa/stream/fd-stream.hh>
#include <mimosa/rpc/json.hh>
#include "log.hh"

#include "config.hh"
#include "config.pb.h"
#include "builtin-authenticator.hh"
#include "process-authenticator.hh"

std::string & CONFIG = *mimosa::options::addOption<std::string>(
  "", "config", "path to the config file", "/etc/scissy/config.json");

namespace scissy
{
  Config::Config()
  {
    struct passwd *pw = getpwent();
    pb::config::Config cfg;
    mimosa::stream::Stream::Ptr in = mimosa::stream::FdStream::openFile(CONFIG.c_str());
    if (in)
      mimosa::rpc::jsonDecode(in, &cfg);

    www_    = cfg.has_www_root() ? cfg.www_root() : INSTALL_PREFIX "/share/scissy/www";
    css_    = www_ + "/css";
    js_     = www_ + "/js";
    vendor_ = www_ + "/vendor";
    html_   = www_ + "/html";

    repos_ = cfg.has_repos_root() ? cfg.repos_root() : "/var/lib/scissy";
    db_ = cfg.has_db() ? cfg.db() : "/var/lib/scissy/db.sqlite";
    socket_ = cfg.has_socket() ? cfg.socket() : "/var/run/scissy/socket";
    crack_passwords_ = cfg.has_crack_passwords() ? cfg.crack_passwords() : false;
    clone_user_ = cfg.has_clone_user() ? cfg.clone_user() : pw->pw_name;
    clone_host_ = cfg.has_clone_host() ? cfg.clone_host() : "localhost";
    git_log_limit_ = cfg.has_git_log_limit() ? cfg.git_log_limit() : 200;
    user_ = cfg.user();
    group_ = cfg.group();
    cookie_duration_ = cfg.has_cookie_duration() ? cfg.cookie_duration() : 7;
    is_register_enabled_ = cfg.has_enable_register() ? cfg.enable_register() : false;

    if (cfg.has_authorized_keys())
      auth_keys_ = cfg.authorized_keys();
    else {
      auth_keys_ = pw->pw_dir;
      auth_keys_.append("/.ssh/authorized_keys");
    }

    for (int i = 0; i < cfg.listen_size(); ++i) {
      Listen listen;
      auto l           = cfg.listen(i);
      listen.addr_     = l.addr();
      listen.port_     = l.port();
      listen.ssl_key_  = l.ssl_key();
      listen.ssl_cert_ = l.ssl_cert();
      listens_.push_back(listen);
    }

    parseAuth(cfg);
  }

  void
  Config::parseAuth(const pb::config::Config & cfg)
  {
    if (!cfg.has_auth() || !cfg.auth().has_method() ||
        cfg.auth().method() == pb::config::kAuthBuiltin) {
      authenticator_.reset(new BuiltinAuthenticator);
    } else if (cfg.auth().method() == pb::config::kAuthProcess) {
      mimosa::ProcessConfig pcfg;
      pcfg.setExecutable(cfg.auth().process().program());
      pcfg.setWorkingDirectory(cfg.auth().process().working_directory());
      for (int i = 0; i < cfg.auth().process().env_size(); ++i) {
        auto & env = cfg.auth().process().env(i);
        if (env.key().empty()) {
          log->warning("empty key in auth process env");
          continue;
        }
        pcfg.setEnv(env.key(), env.value());
      }
      pcfg.addArg(cfg.auth().process().program());
      for (int i = 0; i < cfg.auth().process().args_size(); ++i)
        pcfg.addArg(cfg.auth().process().args(i));
      auto auth = new ProcessAuthenticator;
      auth->setConfig(pcfg);
      authenticator_.reset(auth);
    }
  }
}
