#include "db.hh"

namespace bluegitf
{
  namespace db
  {
    void
    Db::registerUser(::mimosa::rpc::Call<
                       ::bluegitf::db::pb::User,
                       ::bluegitf::db::pb::StatusMsg>::Ptr call)
    {
      mimosa::sync::Mutex::Locker locker(lock_);
    }

    void
    Db::auth(::mimosa::rpc::Call<
               ::bluegitf::db::pb::AuthRequest,
               ::bluegitf::db::pb::StatusMsg>::Ptr call)
    {
      mimosa::sync::Mutex::Locker locker(lock_);
    }
  }
}
