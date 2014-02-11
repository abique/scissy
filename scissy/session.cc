#include "db.hh"
#include "session.hh"

namespace scissy
{
  Session::Session(mimosa::http::RequestReader &  request,
                   mimosa::http::ResponseWriter & response)
    : request_(request),
      response_(response),
      user_id_(0),
      role_(pb::kNone)
  {
    auth();
  }

  void
  Session::auth()
  {
    int      role_id;
    uint64_t user_id;

    auto & cookies = request_.cookies();
    auto it = cookies.find("user");
    if (it == cookies.end())
      return;
    std::string user = it->second;

    it = cookies.find("token");
    if (it == cookies.end())
      return;
    std::string token = it->second;
    std::string email;

    auto stmt = Db::prepare(
      "select role_id, user_id, email"
      "  from users natural join users_auths"
      "  where login = ? and token = ?");
    stmt.bind(user, token);
    if (!stmt.fetch(&role_id, &user_id, &email))
      return;

    user_    = user;
    user_id_ = user_id;
    role_    = (pb::Role)role_id;
    email_   = email;
    token_   = token;
  }
}
