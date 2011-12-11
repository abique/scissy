#include <mimosa/options/options.hh>

#include "config.hh"

std::string & ROOT = * mimosa::options::addOption<std::string>(
  "", "root", "bluegitf root directory", "/srv/bluegitf");

namespace bluegitf
{
  Config::Config()
    : root_dir_(ROOT),
      www_dir_(ROOT),
      tpl_dir_(ROOT),
      key_pem_(ROOT),
      cert_pem_(ROOT),
      repo_dir_(ROOT),
      db_dir_(ROOT),
      db_socket_(ROOT),
      ssh_dir_(ROOT)
  {
    www_dir_.append("/www");
    tpl_dir_.append("/www/tpl");
    key_pem_.append("/conf/key.pem");
    cert_pem_.append("/conf/cert.pem");
    repo_dir_.append("/repo");
    db_dir_.append("/db");
    db_socket_.append("/db/socket");
    ssh_dir_.append("/.ssh");
  }

  Config &
  Config::instance()
  {
    static Config instance;
    return instance;
  }

  mimosa::tpl::Template::Ptr
  Config::loadTpl(const std::string & rel_path) const
  {
    std::string path(www_dir_);
    path += "/" + rel_path;
    return mimosa::tpl::Template::parseFile(path);
  }
}
