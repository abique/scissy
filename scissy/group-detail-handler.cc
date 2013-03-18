#include <mimosa/stream/hash.hh>
#include <mimosa/log/log.hh>
#include <mimosa/sqlite/sqlite.hh>
#include <mimosa/tpl/dict.hh>
#include <mimosa/tpl/include.hh>
#include <mimosa/tpl/template.hh>
#include <mimosa/tpl/value.hh>
#include <mimosa/tpl/list.hh>
#include <mimosa/http/redirect.hh>

#include "log.hh"
#include "db.hh"
#include "groups.hh"
#include "load-tpl.hh"
#include "page-footer.hh"
#include "page-header.hh"
#include "group-detail-handler.hh"
#include "session.hh"

namespace scissy
{
  namespace
  {
    struct RegisterCtx
    {
      mimosa::http::RequestReader &  request_;
      mimosa::http::ResponseWriter & response_;
      Session::Ptr                   session_;

      std::string error_;
      std::string group_;
      std::string user_;        // the user to be added
      std::string user_role_;   // the user's role
      Role        current_role_; // the logged in user's role in the current grp

      RegisterCtx(mimosa::http::RequestReader &  request,
                  mimosa::http::ResponseWriter & response,
                  Session::Ptr                   session)
        : request_(request),
          response_(response),
          session_(session),
          error_(),
          group_(),
          user_(),
          user_role_(),
          current_role_(kObserver)
      {
        parseQuery();
        parseForm();

        Groups::instance().getUserRole(group_, session->login_, &current_role_);
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

        auto it = form.find("login");
        if (it != form.end())
          user_ = it->second;

        it = form.find("role");
        if (it != form.end())
          user_role_ = it->second;
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
        dict.append("body", tpl_body);
        if (!group_.empty())
          dict.append("group", group_);
        if (!error_.empty())
          dict.append("error", error_);

        // load group users
        {
          auto users = new mimosa::tpl::List("users");
          auto stmt  = Db::prepare(
            "select user, role_id from groups_users_view"
            " where name = ?");
          stmt.bind(group_);

          if (current_role_ == kAdministrator)
            dict.append("is-group-admin", true);

          std::string login;
          int role;
          while (stmt.fetch(&login, &role))
          {
            auto user = new mimosa::tpl::Dict("user");
            user->append("login", login);
            user->append("role", roleName(static_cast<Role> (role)));
            users->append(user);
          }
          dict.append(users);

          if (!error_.empty())
            dict.append("error", error_);
        }

        setPageHeader(session_, dict);
        setPageFooter(session_, dict);
        response_.status_ = mimosa::http::kStatusOk;
        response_.content_type_ = "text/html";
        response_.sendHeader();
        tpl->execute(&response_, dict);
        return true;
      }

      void addUpdateUser()
      {
        if (current_role_ != kAdministrator ||
            user_.empty() || user_role_.empty())
          return;

        if (user_role_ == "remove")
        {
          Groups::instance().removeUser(group_, user_);
          return;
        }

        Role role = kObserver;
        if (!parseRole(user_role_, &role))
        {
          error_ = "invalid role";
          return;
        }

        Groups::instance().addUser(group_, user_, role);
      }

      bool handle()
      {
        addUpdateUser();
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