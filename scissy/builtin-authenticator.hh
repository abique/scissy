#ifndef SCISSY_BUILTIN_AUTHENTICATOR_HH
# define SCISSY_BUILTIN_AUTHENTICATOR_HH

# include "authenticator.hh"

namespace scissy
{
  class BuiltinAuthenticator : public Authenticator
  {
  public:
    virtual bool auth(pb::UserAuth & request,
                      pb::Session & response) const;
  };
}

#endif /* !SCISSY_BUILTIN_AUTHENTICATOR_HH */
