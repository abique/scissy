#include "acl.hh"

namespace bluegitf
{
  void
  Acl::get(::mimosa::rpc::Call<
             ::bluegitf::pb::UserRepo,
             ::bluegitf::pb::RepoRight>::Ptr call)
  {
    auto & r = call->response();
    r.set_right(pb::kNone);

    // XXX select, and reply
  }
}
