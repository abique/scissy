#include <iostream>
#include <regex>

#include <crack.h>

#include <mimosa/stream/hash.hh>
#include <mimosa/log/log.hh>
#include <mimosa/sqlite/sqlite.hh>
#include <mimosa/tpl/dict.hh>
#include <mimosa/tpl/include.hh>
#include <mimosa/tpl/template.hh>
#include <mimosa/tpl/value.hh>

#include "db.hh"
#include "load-tpl.hh"
#include "page-footer.hh"
#include "page-header.hh"
#include "register-handler.hh"
#include "session.hh"

namespace bluegitf
{
  namespace
  {
    struct RegisterCtx
    {
      mimosa::http::RequestReader & request_;
      mimosa::http::ResponseWriter & response_;

      std::string login_;
      std::string email_;
      std::string password_;
      std::string password2_;

      std::string error_;

      RegisterCtx(mimosa::http::RequestReader & request,
                  mimosa::http::ResponseWriter & response)
        : request_(request),
          response_(response)
      {
        parseForm();
        checkForm();
      }

      void parseForm()
      {
        const auto & form = request_.form();

        auto it = form.find("login");
        if (it != form.end())
          login_ = it->second;

        it = form.find("email");
        if (it != form.end())
          email_ = it->second;

        it = form.find("password");
        if (it != form.end())
          password_ = it->second;

        it = form.find("password2");
        if (it != form.end())
          password2_ = it->second;
      }

      void checkForm()
      {
        // XXX wait for std::regex to work
        // static const std::regex login_match("^[[:alnum:]]+$", std::regex_constants::extended);
        // static const std::regex email_match("^[-._a-zA-Z0-9]+@[-._a-zA-Z0-9]+$", std::regex_constants::extended);
        if (login_.empty() && email_.empty() &&
            password_.empty() && password2_.empty())
          return;

        if (password_ != password2_)
        {
          error_ = "passwords doesn't match";
          return;
        }

        if (password_.size() < 5)
        {
          error_ = "password too short (5 characters at least)";
          return;
        }

        if (password_.size() > 128)
        {
          error_ = "password too big (128 characters at most)";
          return;
        }

        error_ = ::FascistCheck(password_.c_str(),
                                ::GetDefaultCracklibDict()) ? : "";
        if (!error_.empty())
        {
          error_ = "password: " + error_;
          return;
        }

        // if (!std::regex_match(login_, login_match))
        // {
        //   error_ = "the login must match ^[[:alnum:]]+$";
        //   return;
        // }

        if (login_.size() > 64)
        {
          error_ = "login too big (64 characters at most)";
          return;
        }

        // if (!std::regex_match(email_, email_match))
        // {
        //   error_ = "invalid email";
        //   return;
        // }

        if (email_.size() > 256)
        {
          error_ = "email too big (256 characters at most)";
          return;
        }
      }

      bool tryRegister()
      {
        mimosa::stream::Sha512 sha512;
        sha512.write(password_.data(), password_.size());

        auto stmt = Db::prepare(
          "insert or fail into users (login, email, password)"
          " values (?, ?, ?)");
        int err = stmt.bind(login_, email_, (void*)sha512.digest(), sha512.digestLen())
          .step();

        if (err == SQLITE_CONSTRAINT)
        {
          error_ = "failed to register, try another username";
          return false;
        }

        if (err != SQLITE_DONE)
        {
          error_ = "failed to register, internal error";
          return false;
        }

        return registrationSucceed();
      }

      bool registrationSucceed()
      {
        auto session = Session::get(request_);
        auto tpl = loadTpl(session, "page.html");
        if (!tpl)
          return false;

        mimosa::tpl::Dict dict;

        auto tpl_body = loadTpl(session, "registration-succeed.html");
        if (!tpl_body)
          return false;
        dict.append("body", tpl_body);

        setPageHeader(session, dict);
        setPageFooter(session, dict);

        response_.status_ = mimosa::http::kStatusOk;
        response_.content_type_ = "text/html";
        response_.sendHeader();
        tpl->execute(&response_, dict);
        return true;
      }

      bool handle()
      {
        if (!login_.empty() && error_.empty() && tryRegister())
          return true;

        auto session = Session::get(request_);
        auto tpl = loadTpl(session, "page.html");
        if (!tpl)
          return false;

        mimosa::tpl::Dict dict;

        auto tpl_body = loadTpl(session, "register.html");
        if (!tpl_body)
          return false;
        dict.append("body", tpl_body);

        setPageHeader(session, dict);
        setPageFooter(session, dict);

        if (!login_.empty())
          dict.append("login", login_);
        if (!email_.empty())
          dict.append("email", email_);
        if (!error_.empty())
          dict.append("error", error_);

        response_.status_ = mimosa::http::kStatusOk;
        response_.content_type_ = "text/html";
        response_.sendHeader();
        tpl->execute(&response_, dict);
        return true;
      }
    };
  }

  bool
  RegisterHandler::handle(mimosa::http::RequestReader & request,
                          mimosa::http::ResponseWriter & response) const
  {
    RegisterCtx ctx(request, response);
    return ctx.handle();
  }
}
