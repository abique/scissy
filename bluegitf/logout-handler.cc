#include <mimosa/http/redirect.hh>
#include <mimosa/tpl/template.hh>
#include <mimosa/tpl/dict.hh>
#include <mimosa/sqlite/sqlite.hh>

#include "logout-handler.hh"
#include "session.hh"
#include "page-header.hh"
#include "page-footer.hh"
#include "load-tpl.hh"
#include "db.hh"

namespace bluegitf
{
  bool
  LogoutHandler::handle(mimosa::http::RequestReader & request,
                        mimosa::http::ResponseWriter & response) const
  {
    auto cookie_auth = new mimosa::http::Cookie;
    cookie_auth->setKey("auth");
    cookie_auth->setValue("");
    cookie_auth->setDomain(request.host());
    cookie_auth->setPath("/");
    cookie_auth->setSecure(true);
    cookie_auth->setHttpOnly(true);
    response.cookies_.push(cookie_auth);

    auto cookie_login = new mimosa::http::Cookie;
    cookie_login->setKey("login");
    cookie_login->setValue("");
    cookie_login->setDomain(request.host());
    cookie_login->setPath("/");
    cookie_login->setSecure(true);
    cookie_login->setHttpOnly(true);
    response.cookies_.push(cookie_login);

    auto session = Session::get(request);
    if (session) {
      auto stmt = Db::prepare("delete from users_auths where cookie = ?");
      stmt.bind(session->auth_).exec();
    }

    return mimosa::http::redirect(response, "/");
  }
}
