#include <mimosa/options/options.hh>

#include "config.hh"

std::string & REPOS = *mimosa::options::addOption<std::string>(
  "", "repos", "repositories prefix", "/var/lib/scissy");
bool & UNSECURE = *mimosa::options::addSwitch(
  "", "unsecure", "disable https, usefull for dev");
std::string & SSL_KEY = *mimosa::options::addOption<std::string>(
  "", "ssl-key", "ssl key", "/etc/scissy/server.key");
std::string & SSL_CERT = *mimosa::options::addOption<std::string>(
  "", "ssl-cert", "ssl certificate", "/etc/scissy/server.crt");
std::string & WWW = *mimosa::options::addOption<std::string>(
  "", "www", "www data", INSTALL_PREFIX "/share/scissy/www");
std::string & DB = *mimosa::options::addOption<std::string>(
  "", "db", "sqlite database", "/var/lib/scissy/db.sqlite");
std::string & AUTH_KEYS = *mimosa::options::addOption<std::string>(
  "", "auth-keys", "path to ~/.ssh/authorized_keys",
  "/var/lib/scissy/.ssh/authorized_keys");
std::string & SOCKET = *mimosa::options::addOption<std::string>(
  "", "socket", "local socket for scissy-shell",
  "/var/run/scissy/socket");
bool & CRACK = *mimosa::options::addSwitch(
  "", "crack-password", "check password with cracklib");

namespace scissy
{
  Config::Config()
    : www_(WWW),
      ssl_key_(SSL_KEY),
      ssl_cert_(SSL_CERT),
      repos_(REPOS),
      db_(DB),
      auth_keys_(AUTH_KEYS),
      socket_(SOCKET),
      is_secure_(!UNSECURE),
      crack_passwords_(CRACK)
  {
    css_   = www_ + "/css";
    js_    = www_ + "/js";
    fonts_ = www_ + "/fonts";
    html_  = www_ + "/html";
  }
}
