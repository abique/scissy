#ifndef SCISSY_PROCESS_AUTHENTICATOR_HH
# define SCISSY_PROCESS_AUTHENTICATOR_HH

# include "authenticator.hh"

namespace scissy
{
  class ProcessAuthenticator : public Authenticator
  {
  public:
    virtual bool auth(pb::UserAuth & request,
                      pb::Session & response) const;
  };
}

#endif /* !SCISSY_PROCESS_AUTHENTICATOR_HH */
