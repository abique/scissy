#include <sys/types.h>
#include <pwd.h>

#include <mimosa/options/options.hh>
#include <mimosa/stream/fd-stream.hh>
#include <mimosa/rpc/json.hh>

#include "config.hh"
#include "config.pb.h"

std::string & CONFIG = *mimosa::options::addOption<std::string>(
  "", "config", "path to the config file", "/etc/scissy/config.json");

namespace scissy
{
  Config::Config()
  {
    struct passwd *pw = getpwent();
    pb::Config cfg;
    mimosa::stream::Stream::Ptr in = mimosa::stream::FdStream::openFile(CONFIG.c_str());
    if (in)
      mimosa::rpc::jsonDecode(in, &cfg);

    www_ = cfg.has_www_root() ? cfg.www_root() : INSTALL_PREFIX "/share/scissy/www";
    css_   = www_ + "/css";
    js_    = www_ + "/js";
    fonts_ = www_ + "/fonts";
    html_  = www_ + "/html";
    cm_  = www_ + "/codemirror";

    repos_ = cfg.has_repos_root() ? cfg.repos_root() : "/var/lib/scissy";
    db_ = cfg.has_db() ? cfg.db() : "/var/lib/scissy/db.sqlite";
    socket_ = cfg.has_socket() ? cfg.socket() : "/var/run/scissy/socket";
    crack_passwords_ = cfg.has_crack_passwords() ? cfg.crack_passwords() : false;
    clone_user_ = cfg.has_clone_user() ? cfg.clone_user() : pw->pw_name;
    clone_host_ = cfg.has_clone_host() ? cfg.clone_host() : "localhost";
    git_log_limit_ = cfg.has_git_log_limit() ? cfg.git_log_limit() : 200;

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
  }
}
