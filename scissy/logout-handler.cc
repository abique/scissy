#include <mimosa/http/server-channel.hh>

#include "db.hh"
#include "logout-handler.hh"
#include "session.hh"

namespace scissy
{
  bool
  LogoutHandler::handle(mimosa::http::RequestReader &  request,
                        mimosa::http::ResponseWriter & response) const
  {
    // revoke auth token
    {
      Session session(request, response);
      auto stmt = Db::prepare("delete from users_auths where user_id = ? and token = ?");
      stmt.bind(session.userId(), session.token()).step();
    }

    bool secure = response.channel().isSsl();

    // set cookies
    auto cookie = new mimosa::http::Cookie;
    cookie->setKey("user");
    cookie->setValue("");
    cookie->setHttpOnly(true);
    cookie->setSecure(secure);
    cookie->setExpires("Tue Feb 11 21:42:42 CET 2000");
    response.cookies_.push(cookie);

    cookie = new mimosa::http::Cookie;
    cookie->setKey("token");
    cookie->setValue("");
    cookie->setHttpOnly(true);
    cookie->setSecure(secure);
    cookie->setExpires("Tue Feb 11 21:42:42 CET 2000");
    response.cookies_.push(cookie);

    return true;
  }
}
