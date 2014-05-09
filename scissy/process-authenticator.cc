#include <mimosa/process.hh>

#include "log.hh"

namespace scissy
{
  bool
  DbAuthenticator::auth(pb::UserAuth & request,
                        pb::Session & response) const
  {
    mimosa::Process process;

    process.setExecutable(program_);
    process.setEnv("USERNAME", request.user());
    process.setEnv("PASSWORD", request.password());
    if (!process.start())
      return false;
    return process.wait() == 0;
  }
}
