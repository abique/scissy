#ifndef BLUEGITF_ACL_HH
# define BLUEGITF_ACL_HH

# include "acl.pb.h"

namespace bluegitf
{
  class Acl : public pb::Acl::Server
  {
  public:
    virtual void get(::mimosa::rpc::Call<
                       ::bluegitf::pb::UserRepo,
                       ::bluegitf::pb::RepoRight>::Ptr call);
  };
}

#endif /* !BLUEGITF_ACL_HH */
