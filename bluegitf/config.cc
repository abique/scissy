#include <gflags/gflags.h>

#include "config.hh"

DEFINE_int32(port, 4242, "the port to use");
DEFINE_string(root, "", "bluegitf root directory");

namespace bluegitf
{
  Config::Config()
    : root_dir_(FLAGS_root),
      www_dir_(FLAGS_root),
      repo_dir_(FLAGS_root),
      db_dir_(FLAGS_root),
      ssh_dir_(FLAGS_root)
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
