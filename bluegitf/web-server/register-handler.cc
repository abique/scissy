#include <iostream>
#include <regex>

#include <mimosa/log/log.hh>
#include <mimosa/tpl/template.hh>
#include <mimosa/tpl/dict.hh>
#include <mimosa/tpl/include.hh>
#include <mimosa/tpl/value.hh>

#include "register-handler.hh"
#include "session.hh"
#include "page-header.hh"
#include "page-footer.hh"
#include "load-tpl.hh"
#include "db.hh"

namespace bluegitf
{
  namespace web_server
  {
    namespace
    {
      struct RegisterCtx
      {
        mimosa::http::RequestReader & request_;
        mimosa::http::ResponseWriter & response_;

        std::string login_;
        std::string login_err_;
        std::string email_;
        std::string email_err_;
        std::string password_;
        std::string password_err_;
        std::string password2_;

        std::string register_err_;

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

          if (password_ != password2_)
            password_err_ = "passwords doesn't match";

          if (password_.size() < 5)
            password_err_ = "password too short (5 characters at least)";

          if (password_.size() > 128)
            password_err_ = "password too big (128 characters at most)";

          // if (!std::regex_match(login_, login_match))
          //   login_err_ = "the login must match ^[[:alnum:]]+$";

          if (login_.size() > 64)
            password_err_ = "login too big (64 characters at most)";

          // if (!std::regex_match(email_, email_match))
          //   email_err_ = "invalid email";

          if (email_.size() > 256)
            password_err_ = "email too big (256 characters at most)";
        }

        bool tryRegister()
        {
          MIMOSA_LOG(Error, NULL, "trying to register");

          auto db = dbService();
          if (!db)
          {
            register_err_ = "database unavailable";
            return false;
          }

          auto user = new db::pb::User;
          user->set_email(email_);
          user->set_login(login_);
          user->set_password(password_);

          auto call = db->addUser(user);
          call->wait();
          if (call->isCanceled())
            return false;

          switch (call->response().status())
          {
          case db::pb::kSucceed:
            // TODO redirect
            return true;

          case db::pb::kInternalError:
            register_err_ = "internal error";
            return false;

          case db::pb::kDuplicateUser:
            register_err_ = "duplicate user";
            return false;

          default:
            register_err_ = db::pb::Status_Name(call->response().status());
            return false;
          }
        }

        bool handle()
        {
          if (!login_.empty() && !email_.empty() && !password_.empty() &&
              password_err_.empty() && login_err_.empty() && email_err_.empty() &&
              tryRegister())
            return true;

          auto session = Session::get(request_);
          auto tpl = loadTpl(session, "page.html");
          if (!tpl)
            return false;

          mimosa::tpl::Dict dict;

          auto tpl_body = loadTpl(session, "register.html");
          if (!tpl_body)
            return false;
          dict.append(new mimosa::tpl::Include(tpl_body, "body"));

          setPageHeader(session, dict);
          setPageFooter(session, dict);

          if (!login_.empty())
            dict.append(new mimosa::tpl::Value<std::string>(login_, "register_login"));
          if (!email_.empty())
            dict.append(new mimosa::tpl::Value<std::string>(email_, "register_email"));

          if (!login_.empty() || !email_.empty() || !password_.empty())
          {
            if (!login_err_.empty())
              dict.append(new mimosa::tpl::Value<std::string>(login_err_, "register_login_err"));
            if (!password_err_.empty())
              dict.append(new mimosa::tpl::Value<std::string>(password_err_, "register_password_err"));
            if (!email_err_.empty())
              dict.append(new mimosa::tpl::Value<std::string>(email_err_, "register_email_err"));
            if (!register_err_.empty())
              dict.append(new mimosa::tpl::Value<std::string>(register_err_, "register_err"));
          }

          response_.status_ = mimosa::http::kStatusOk;
          response_.content_type_ = "text/html";
          response_.sendHeader(response_.writeTimeout());
          tpl->execute(&response_, dict, response_.writeTimeout());
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
}