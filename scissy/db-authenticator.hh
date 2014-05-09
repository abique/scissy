#ifndef SCISSY_DB_AUTHENTICATOR_HH
# define SCISSY_DB_AUTHENTICATOR_HH

# include "authenticator.hh"

namespace scissy
{
  class DbAuthenticator : public Authenticator
  {
  public:
    virtual bool auth(pb::UserAuth & request,
                      pb::Session & response) const;
  };
}

#endif /* !SCISSY_DB_AUTHENTICATOR_HH */
