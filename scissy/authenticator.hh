#ifndef SCISSY_AUTHENTICATOR_HH
# define SCISSY_AUTHENTICATOR_HH

# include "service.pb.h"

namespace scissy
{
  class Authenticator
  {
  public:
    inline virtual ~Authenticator() {}

    virtual bool auth(pb::UserAuth & request,
                      pb::Session & response) const = 0;
  };
}

#endif /* !SCISSY_AUTHENTICATOR_HH */
