#include <ctime>
#include <cstring>

#include <mimosa/http/redirect.hh>
#include <mimosa/stream/hash.hh>
#include <mimosa/stream/string-stream.hh>
#include <mimosa/stream/base16-encoder.hh>
#include <mimosa/log/log.hh>
#include <mimosa/sqlite/sqlite.hh>
#include <mimosa/tpl/dict.hh>
#include <mimosa/tpl/include.hh>
#include <mimosa/tpl/template.hh>
#include <mimosa/tpl/value.hh>

#include "db.hh"
#include "load-tpl.hh"
#include "login-handler.hh"
#include "page-footer.hh"
#include "page-header.hh"
#include "session.hh"

namespace bluegitf
{
  namespace
  {
    struct LoginCtx
    {
      mimosa::http::RequestReader & request_;
      mimosa::http::ResponseWriter & response_;

      std::string login_;
      std::string password_;
      std::string err_;
      bool        auth_;
      int         user_id_;

      LoginCtx(mimosa::http::RequestReader & request,
               mimosa::http::ResponseWriter & response)
        : request_(request),
          response_(response),
          login_(),
          password_(),
          err_(),
          auth_(false)
      {
        parseForm();
      }

      void parseForm()
      {
        const auto & form = request_.form();

        auto it = form.find("login");
        if (it == form.end())
          return;
        login_ = it->second;

        it = form.find("password");
        if (it == form.end())
          return;
        password_ = it->second;

        if (!login_.empty() && !password_.empty())
          auth_ = tryAuth();
      }

      bool tryAuth()
      {
        // load the password from the db
        mimosa::sqlite::Stmt stmt;
        int err = stmt.prepare(Db::handle(),
                               "select password, user_id from users where login = ?");
        assert(err == SQLITE_OK); // must pass
        err = stmt.bind(1, login_);
        assert(err == SQLITE_OK);

        err = stmt.step();

        if (err == SQLITE_DONE)
        {
          err_ = "user not found";
          return false;
        }

        if (err != SQLITE_ROW)
          return false;

        const void * blob = sqlite3_column_blob(stmt, 0);
        size_t blob_size = sqlite3_column_bytes(stmt, 0);

        user_id_ = sqlite3_column_int(stmt, 1);

        // compute the hash
        mimosa::stream::Sha512 sha512;
        sha512.write(password_.data(), password_.size());

        // chech if hashes equals
        if (blob_size != sha512.digestLen())
        {
          MIMOSA_LOG(Critical, NULL, "invalid password hash for user %s"
                     " (size in db: %d, expected size: %d)", login_, blob_size,
                     sha512.digestLen());
          err_ = "internal error, please contact your administrator";
          return false;
        }

        if (::memcmp(sha512.digest(), blob, blob_size))
        {
          err_ = "invalid password";
          return false;
        }

        return genAuthCookie();
      }

      bool genAuthCookie()
      {
        // generate a random value
        char auth[32];
        if (gnutls_rnd(GNUTLS_RND_NONCE, auth, sizeof (auth)) < 0)
        {
          err_ = "internal cryptographic error";
          return false;
        }

        mimosa::stream::StringStream::Ptr auth16 = new mimosa::stream::StringStream;
        mimosa::stream::Base16Encoder::Ptr b16 = new mimosa::stream::Base16Encoder(auth16);
        b16->write(auth, sizeof (auth));

        // retrieve client's ip

        // save the cookie in the db
        mimosa::sqlite::Stmt stmt;
        int err = stmt.prepare(Db::handle(),
                               "insert or fail into users_auths (user_id, cookie, ts_start)"
                               " values (?, ?, ?)");
        assert(err == SQLITE_OK); // must pass

        mimosa::stream::Sha512 sha512;
        sha512.write(password_.data(), password_.size());

        err = stmt.bind(1, user_id_);
        assert(err == SQLITE_OK);
        err = stmt.bindBlob(2, auth, sizeof (auth));
        assert(err == SQLITE_OK);
        err = stmt.bind(3, ::time(0));
        assert(err == SQLITE_OK);

        err = stmt.step();
        if (err != SQLITE_DONE)
        {
          MIMOSA_LOG(Debug, NULL, "error: %d", err);
          err_ = "got some troubles";
          return false;
        }

        // set the cookie to the response
        auto cookie_login = new mimosa::http::Cookie;
        cookie_login->setKey("login");
        cookie_login->setValue(login_);
        cookie_login->setDomain(request_.host());
        cookie_login->setPath("/");
        cookie_login->setSecure(true);
        cookie_login->setHttpOnly(true);

        auto cookie_auth = new mimosa::http::Cookie;
        cookie_auth->setKey("auth");
        cookie_auth->setValue(auth16->str());
        cookie_auth->setDomain(request_.host());
        cookie_auth->setPath("/");
        cookie_auth->setSecure(true);
        cookie_auth->setHttpOnly(true);

        response_.cookies_.push(cookie_login);
        response_.cookies_.push(cookie_auth);
        return true;
      }

      bool handle()
      {
        if (auth_)
        {
          MIMOSA_LOG(Debug, NULL, "%s", response_.toHttpHeader());
          return mimosa::http::redirect(response_, "/");
        }

        auto session = Session::get(request_);
        auto tpl = loadTpl(session, "page.html");
        if (!tpl)
          return false;

        mimosa::tpl::Dict dict;

        auto tpl_body = loadTpl(session, "login.html");
        if (!tpl_body)
          return false;
        dict.append(new mimosa::tpl::Include(tpl_body, "body"));

        setPageHeader(session, dict);
        setPageFooter(session, dict);

        if (!err_.empty())
          dict.append("error", err_);

        response_.status_ = mimosa::http::kStatusOk;
        response_.content_type_ = "text/html";
        response_.sendHeader(response_.writeTimeout());
        tpl->execute(&response_, dict, response_.writeTimeout());
        return true;
      }
    };
  }

  bool
  LoginHandler::handle(mimosa::http::RequestReader & request,
                       mimosa::http::ResponseWriter & response) const
  {
    LoginCtx ctx(request, response);
    return ctx.handle();
  }
}
