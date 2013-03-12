#ifndef SCISSY_SERVICE_HH
# define SCISSY_SERVICE_HH

# include "service.pb.h"

namespace scissy
{
  class Service : public pb::Service::Server
  {
  public:
    virtual bool userCreate(pb::UserCreate & request,
                            pb::StatusMsg &  response) override;

    virtual bool userAuth(pb::UserAuth & request,
                          pb::Session & response) override;

    virtual bool userCheckAuthToken(pb::UserCheckAuthToken & request,
                                    pb::Session & response) override;

    virtual bool userRevokeAuthToken(pb::UserRevokeAuthToken & request,
                                     pb::StatusMsg & response) override;
  };
}

#endif /* !SCISSY_SERVICE_HH */
