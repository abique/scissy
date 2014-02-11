#ifndef SCISSY_SESSION_HH
# define SCISSY_SESSION_HH

# include <mimosa/http/handler.hh>

# include "service.pb.h"

namespace scissy
{
  class Session
  {
  public:
    Session(mimosa::http::RequestReader & request,
            mimosa::http::ResponseWriter & response);

    inline mimosa::http::RequestReader & httpRequest() { return request_; }
    inline mimosa::http::ResponseWriter & httpResponse() { return response_; }

    inline const std::string & user() const { return user_; }
    inline uint64_t userId() const { return user_id_; }
    inline pb::Role role() const { return role_; }
    inline const std::string & token() const { return token_; }

  private:

    void auth();

    mimosa::http::RequestReader &  request_;
    mimosa::http::ResponseWriter & response_;

    std::string user_;
    uint64_t    user_id_;
    pb::Role    role_;
    std::string email_;
    std::string token_;
  };
}

#endif /* !SCISSY_SESSION_HH */
