#ifndef SCISSY_SERVICE_HH
# define SCISSY_SERVICE_HH

# include "service.pb.h"

namespace scissy
{
  class Service : public pb::Service::Server
  {
  public:

      ////////////////
     // User stuff //
    ////////////////

    virtual bool userCreate(pb::UserCreate & request,
                            pb::StatusMsg &  response) override;

    virtual bool userAuth(pb::UserAuth & request,
                          pb::Session & response) override;

    virtual bool userCheckAuthToken(pb::UserAuthToken & request,
                                    pb::Session & response) override;

    virtual bool userRevokeAuthToken(pb::UserAuthToken & request,
                                     pb::StatusMsg & response) override;


      /////////////////
     // Group stuff //
    /////////////////

    virtual bool groupCreate(pb::GroupCreate & request,
                             pb::GroupInfo & response) override;

    virtual bool groupDelete(pb::GroupDelete & request,
                             pb::StatusMsg & response) override;

    virtual bool groupAddUser(pb::GroupAddUser & request,
                              pb::StatusMsg & response) override;

    virtual bool groupRemoveUser(pb::GroupRemoveUser & request,
                                 pb::StatusMsg & response) override;

  };
}

#endif /* !SCISSY_SERVICE_HH */