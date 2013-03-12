#include <mimosa/stream/hash.hh>
#include <mimosa/log/log.hh>
#include <mimosa/sqlite/sqlite.hh>
#include <mimosa/tpl/dict.hh>
#include <mimosa/tpl/include.hh>
#include <mimosa/tpl/template.hh>
#include <mimosa/tpl/value.hh>
#include <mimosa/http/redirect.hh>

#include "db.hh"
#include "load-tpl.hh"
#include "page-footer.hh"
#include "page-header.hh"
#include "new-repo-handler.hh"
#include "session.hh"
#include "repositories.hh"

namespace scissy
{
  namespace
  {
    struct RegisterCtx
    {
      mimosa::http::RequestReader &  request_;
      mimosa::http::ResponseWriter & response_;
      Session::Ptr                   session_;

      std::string                    error_;
      std::string                    name_;
      std::string                    desc_;

      RegisterCtx(mimosa::http::RequestReader &  request,
                  mimosa::http::ResponseWriter & response,
                  Session::Ptr                   session)
        : request_(request),
          response_(response),
          session_(session)
      {
        parseForm();
      }

      void parseForm()
      {
        auto & form = request_.form();

        auto it = form.find("name");
        if (it != form.end())
          name_ = it->second;

        it = form.find("desc");
        if (it != form.end())
          desc_ = it->second;
      }

      bool showForm()
      {
        auto tpl = loadTpl(session_, "page.html");
        if (!tpl)
          return false;

        mimosa::tpl::Dict dict;

        auto tpl_body = loadTpl(session_, "new-repo.html");
        if (!tpl_body)
          return false;
        dict.append("body", tpl_body);
        if (!name_.empty())
          dict.append("name", name_);
        if (!desc_.empty())
          dict.append("desc", desc_);
        if (!error_.empty())
          dict.append("error", error_);

        setPageHeader(session_, dict);
        setPageFooter(session_, dict);
        response_.status_ = mimosa::http::kStatusOk;
        response_.content_type_ = "text/html";
        response_.sendHeader();
        tpl->execute(&response_, dict);
        return true;
      }

      bool handle()
      {
        if (!name_.empty() &&
            Repositories::instance().create(name_, desc_, session_->login_, &error_))
          return mimosa::http::redirect(response_, "/");
        return showForm();
      }
    };
  }

  bool
  NewRepoHandler::handle(mimosa::http::RequestReader & request,
                          mimosa::http::ResponseWriter & response) const
  {
    auto session = Session::get(request);
    if (!session || session->login_.empty())
      return mimosa::http::redirect(response, "/login");

    RegisterCtx ctx(request, response, session);
    return ctx.handle();
  }
}
