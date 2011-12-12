#include <mimosa/log/log.hh>

#include "db.hh"

namespace bluegitf
{
  namespace db
  {
    void
    Db::addUser(::mimosa::rpc::Call<
                       ::bluegitf::db::pb::User,
                       ::bluegitf::db::pb::StatusMsg>::Ptr call)
    {
      mimosa::sync::Mutex::Locker locker(lock_);
      MIMOSA_LOG(Info, NULL, "addUser({%s, %s, %s})",
                 call->request().login(),
                 call->request().email(),
                 call->request().password());
      call->response().set_status(pb::kInternalError);
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
