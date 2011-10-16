#include <gflags/gflags.h>

#include <mimosa/init.hh>
#include <mimosa/http/server.hh>
#include <mimosa/http/dispatch-handler.hh>
#include <mimosa/http/fs-handler.hh>

#include "bluegitfd.hh"

DEFINE_int32(port, 4242, "the port to use");
DEFINE_string(root, "", "bluegitf root directory");

int main(int argc, char ** argv)
{
  return 0;
}
