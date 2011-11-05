#include <mimosa/options/options.hh>

#include "config.hh"

std::string & ROOT = * mimosa::options::addOption<std::string>(
  "", "root", "bluegitf root directory", "");

namespace bluegitf
{
  Config::Config()
    : root_dir_(ROOT),
      www_dir_(ROOT),
      repo_dir_(ROOT),
      db_dir_(ROOT),
      ssh_dir_(ROOT)
  {
    www_dir_.append("/www");
    repo_dir_.append("/repo");
    db_dir_.append("/db");
    ssh_dir_.append("/.ssh");
  }

  Config &
  Config::instance()
  {
    static Config instance;
    return instance;
  }
}
