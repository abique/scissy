#include <mimosa/stream/hash.hh>
#include <mimosa/log/log.hh>
#include <mimosa/sqlite/sqlite.hh>
#include <mimosa/tpl/dict.hh>
#include <mimosa/tpl/include.hh>
#include <mimosa/tpl/template.hh>
#include <mimosa/tpl/value.hh>
#include <mimosa/tpl/list.hh>
#include <mimosa/http/redirect.hh>

#include "db.hh"
#include "groups.hh"
#include "load-tpl.hh"
#include "page-footer.hh"
#include "page-header.hh"
#include "group-detail-handler.hh"
#include "session.hh"

namespace bluegitf
{
  namespace
  {
    struct RegisterCtx
    {
      mimosa::http::RequestReader &  request_;
      mimosa::http::ResponseWriter & response_;
      Session::Ptr                   session_;

      std::string                    error_;
      std::string                    group_;
      std::string                    user_;
      std::string                    role_;

      RegisterCtx(mimosa::http::RequestReader &  request,
                  mimosa::http::ResponseWriter & response,
                  Session::Ptr                   session)
        : request_(request),
          response_(response),
          session_(session),
          error_(),
          group_(),
          user_(),
          role_()
      {
        parseQuery();
        parseForm();
      }

      void parseQuery()
      {
        auto & query = request_.query();

        auto it = query.find("group");
        if (it != query.end())
          group_ = it->second;
      }

      void parseForm()
      {
        auto & form = request_.form();

        auto it = form.find("user");
        if (it != form.end())
          user_ = it->second;

        it = form.find("role");
        if (it != form.end())
          role_ = it->second;
      }

      bool showForm()
      {
        auto tpl = loadTpl(session_, "page.html");
        if (!tpl)
          return false;

        mimosa::tpl::Dict dict;

        auto tpl_body = loadTpl(session_, "group-detail.html");
        if (!tpl_body)
          return false;
        dict.append(new mimosa::tpl::Include(tpl_body, "body"));
        if (!group_.empty())
          dict.append(new mimosa::tpl::Value<std::string>(group_, "group"));
        if (!error_.empty())
          dict.append(new mimosa::tpl::Value<std::string>(error_, "error"));

        // load group users
        {
          MIMOSA_LOG(Debug, NULL, "%s", group_);

          mimosa::sqlite::Stmt stmt;
          int err = stmt.prepare(Db::handle(),
                                 "select login, groups_users.role_id"
                                 " from groups join groups_users using (group_id)"
                                 " join users using (user_id)"
                                 " where groups.name = ?");
          assert(err == SQLITE_OK);

          err = stmt.bind(1, group_);
          assert(err == SQLITE_OK);

          auto users = new mimosa::tpl::List("users");
          while (stmt.step() == SQLITE_ROW)
          {
            if (session_->login_ == (const char *)sqlite3_column_text(stmt, 0))
              dict.append(new mimosa::tpl::Value<bool>(true, "is-group-admin"));

            auto user = new mimosa::tpl::Dict("user");
            user->append(new mimosa::tpl::Value<std::string>(
                            (const char*)sqlite3_column_text(stmt, 0), "login"));
            user->append(new mimosa::tpl::Value<std::string>(
                            (const char*)sqlite3_column_text(stmt, 1), "role"));
            users->append(user);
          }
          dict.append(users);
        }

        setPageHeader(session_, dict);
        setPageFooter(session_, dict);
        response_.status_ = mimosa::http::kStatusOk;
        response_.content_type_ = "text/html";
        response_.sendHeader(response_.writeTimeout());
        tpl->execute(&response_, dict, response_.writeTimeout());
        return true;
      }

      bool handle()
      {
        // if (!name_.empty() &&
        //     Groups::instance().create(name_, desc_, session_->login_, error_))
        //   return mimosa::http::redirect(response_, "/");
        return showForm();
      }
    };
  }

  bool
  GroupDetailHandler::handle(mimosa::http::RequestReader &  request,
                             mimosa::http::ResponseWriter & response) const
  {
    auto session = Session::get(request);
    if (!session || session->login_.empty())
      return mimosa::http::redirect(response, "/login");

    RegisterCtx ctx(request, response, session);
    return ctx.handle();
  }
}
