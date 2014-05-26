#include "config.hh"
#include "log.hh"
#include "process-authenticator.hh"

namespace scissy
{
  bool
  ProcessAuthenticator::auth(pb::UserAuth & request,
                             pb::Session & response) const
  {
    mimosa::Process process(cfg_);

    process.setEnv("USERNAME", request.user());
    process.setEnv("PASSWORD", request.password());
    if (!process.start()) {
      response.set_status(pb::kFailed);
      response.set_msg("auth failed");
      return false;
    }
    return process.wait() == 0;
  }
}
