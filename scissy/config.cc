#include <mimosa/options/options.hh>

#include "config.hh"

std::string & ROOT = * mimosa::options::addOption<std::string>(
  "", "root", "scissy root directory", "/srv/scissy");
bool & UNSECURE = *mimosa::options::addSwitch(
  "", "unsecure", "disable https, usefull for dev");

namespace scissy
{
  Config::Config()
    : root_dir_(ROOT),
      www_dir_(ROOT),
      css_dir_(ROOT),
      js_dir_(ROOT),
      img_dir_(ROOT),
      html_dir_(ROOT),
      key_pem_(ROOT),
      cert_pem_(ROOT),
      repo_dir_(ROOT),
      db_dir_(ROOT),
      db_path_(ROOT),
      ssh_dir_(ROOT),
      authorized_keys_path_(ROOT),
      unix_socket_path_(ROOT),
      is_secure_(!UNSECURE)
  {
    www_dir_.append("/www");
    css_dir_.append("/www/css");
    js_dir_.append("/www/js");
    img_dir_.append("/www/img");
    html_dir_.append("/www/html");
    key_pem_.append("/conf/key.pem");
    cert_pem_.append("/conf/cert.pem");
    repo_dir_.append("/repos");
    db_dir_.append("/db");
    db_path_.append("/db/db.sqlite");
    ssh_dir_.append("/.ssh");
    authorized_keys_path_.append("/.ssh/authorized_keys");
    unix_socket_path_.append("/socket");
  }
}
