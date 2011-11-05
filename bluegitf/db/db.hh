#ifndef BLUEGITF_DB_DB_HH
# define BLUEGITF_DB_DB_HH

# include <mimosa/sync/mutex.hh>

# include "service.pb.h"

namespace bluegitf
{
  namespace db
  {
    class Db : public pb::Db::Server
    {
    public:
      virtual void registerUser(::mimosa::rpc::Call<
                                  ::bluegitf::db::pb::User,
                                  ::bluegitf::db::pb::StatusMsg>::Ptr call);

      virtual void auth(::mimosa::rpc::Call<
                          ::bluegitf::db::pb::AuthRequest,
                          ::bluegitf::db::pb::StatusMsg>::Ptr call);

    private:
      mimosa::sync::Mutex lock_;
    };
  }
}

#endif /* !BLUEGITF_DB_DB_HH */
