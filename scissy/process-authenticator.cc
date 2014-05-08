#include <mimosa/process.hh>

#include "log.hh"

namespace scissy
{
  bool
  DbAuthenticator::auth(pb::UserAuth & request,
                        pb::Session & response) const
  {
    mimosa::Process process;
    return true;
  }
}
