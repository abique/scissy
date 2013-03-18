#include <mimosa/log/log.hh>
#include <mimosa/stream/string-stream.hh>
#include <mimosa/stream/base16-decoder.hh>
#include <mimosa/sqlite/sqlite.hh>

#include "db.hh"
#include "session.hh"

namespace scissy
{
  static bool check(const std::string & login,
                    const std::string & auth)
  {
    auto blob = new mimosa::stream::StringStream;
    mimosa::stream::Base16Decoder b16(blob);

    b16.write(auth.data(), auth.size());
    auto str = blob->str();

    auto stmt = Db::prepare(
      "select 1 from users join users_auths using (user_id)"
      " where login = ? and cookie = ?");

    return stmt.bind(login, (const void*)str.data(), str.size())
      .fetch();
  }

  Session::Ptr
  Session::get(const mimosa::http::Request & request)
  {
    std::string login;
    std::string auth;

    auto it = request.cookies().find("login");
    if (it == request.cookies().end() || it->second.empty())
      return nullptr;
    login = it->second;

    it = request.cookies().find("auth");
    if (it == request.cookies().end() || it->second.empty())
      return nullptr;
    auth = it->second;

    if (!check(login, auth))
    {
      mimosa::log::debug("Session::get(%s, %s) failed", login, auth);
      return nullptr;
    }

    mimosa::log::debug("Session::get(%s, %s) succeed", login, auth);

    auto session = new Session;
    session->login_ = login;
    session->auth_ = auth;
    return session;
  }
}