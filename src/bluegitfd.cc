#include <gflags/gflags.h>

#include <mimosa/init.hh>
#include <mimosa/http/server.hh>
#include <mimosa/http/dispatch-handler.hh>
#include <mimosa/http/fs-handler.hh>

#include "bluegitfd.hh"

DEFINE_int32(port, 4242, "the port to use");
DEFINE_string(root, "", "bluegitf root directory");

Bluegitfd * Bluegitfd::instance_ = nullptr;

Bluegitfd::Bluegitfd()
  : root_dir_(FLAGS_root),
    www_dir_(FLAGS_root),
    repo_dir_(FLAGS_root),
    db_dir_(FLAGS_root),
    ssh_dir_(FLAGS_root),
    web_server_()
{
  instance_ = this;
  www_dir_.append("/www");
  repo_dir_.append("/repo");
  db_dir_.append("/db");
  ssh_dir_.append("/.ssh");
}

void
Bluegitfd::run()
{
}

int main(int argc, char ** argv)
{
  mimosa::init(argc, argv);

  Bluegitfd bluegitfd;
  bluegitfd.run();

  return 0;
}
