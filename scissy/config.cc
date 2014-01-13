#include <sys/types.h>
#include <pwd.h>

#include <mimosa/options/options.hh>
#include <mimosa/stream/fd-stream.hh>
#include <mimosa/rpc/json.hh>

#include "config.hh"
#include "config.pb.h"

std::string & CONFIG = *mimosa::options::addOption<std::string>(
  "", "config", "path to the config file", "/etc/scissy/config.json");


bool & CRACK = *mimosa::options::addSwitch(
  "", "crack-password", "check password with cracklib");

namespace scissy
{
  Config::Config()
  {
    struct passwd *pw = getpwent();
    pb::Config cfg;
    mimosa::stream::Stream::Ptr in = mimosa::stream::FdStream::openFile(CONFIG.c_str());
    mimosa::rpc::jsonDecode(in, &cfg);

    www_ = cfg.has_www() ? cfg.www() : INSTALL_PREFIX "/share/scissy/www";
    css_   = www_ + "/css";
    js_    = www_ + "/js";
    fonts_ = www_ + "/fonts";
    html_  = www_ + "/html";

    ssl_key_ = cfg.has_ssl_key() ? cfg.ssl_key() : "/etc/scissy/server.key";
    ssl_cert_ = cfg.has_ssl_cert() ? cfg.ssl_cert() : "/etc/scissy/server.cert";
    repos_ = cfg.has_repos_prefix() ? cfg.repos_prefix() : "/var/lib/scissy";
    db_ = cfg.has_db() ? cfg.db() : "/var/lib/scissy/db.sqlite";
    auth_keys_ = "/var/lib/scissy/.ssh/authorized_keys";
    socket_ = cfg.has_socket() ? cfg.socket() : "/var/run/scissy/socket";
    crack_passwords_ = cfg.has_crack_passwords() ? cfg.crack_passwords() : false;
    is_secure_ = cfg.has_unsecure() ? !cfg.unsecure() : true;
    clone_user_ = cfg.has_clone_user() ? cfg.clone_user() : pw->pw_name;
    clone_host_ = cfg.has_clone_host() ? cfg.clone_host() : "localhost";
    auth_keys_ = pw->pw_dir;
    auth_keys_.append("/.ssh/authorized_keys");
  }
}
