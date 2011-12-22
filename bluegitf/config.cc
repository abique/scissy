#include <mimosa/options/options.hh>

#include "config.hh"

std::string & ROOT = * mimosa::options::addOption<std::string>(
  "", "root", "bluegitf root directory", "/srv/bluegitf");

namespace bluegitf
{
  Config::Config()
    : root_dir_(ROOT),
      www_dir_(ROOT),
      css_dir_(ROOT),
      tpl_dir_(ROOT),
      key_pem_(ROOT),
      cert_pem_(ROOT),
      repo_dir_(ROOT),
      db_dir_(ROOT),
      db_path_(ROOT),
      ssh_dir_(ROOT)
  {
    www_dir_.append("/www");
    css_dir_.append("/www/css");
    tpl_dir_.append("/www/tpl");
    key_pem_.append("/conf/key.pem");
    cert_pem_.append("/conf/cert.pem");
    repo_dir_.append("/repos");
    db_dir_.append("/db");
    db_path_.append("/db/db.sqlite");
    ssh_dir_.append("/.ssh");
  }
}
