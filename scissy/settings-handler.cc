#include <mimosa/tpl/template.hh>
#include <mimosa/tpl/dict.hh>
#include <mimosa/tpl/include.hh>

#include "settings-handler.hh"
#include "session.hh"
#include "page-header.hh"
#include "page-footer.hh"
#include "load-tpl.hh"

namespace scissy
{
  namespace
  {
    struct RegisterCtx
    {
      mimosa::http::RequestReader & request_;
      mimosa::http::ResponseWriter & response_;

      RegisterCtx(mimosa::http::RequestReader & request,
                  mimosa::http::ResponseWriter & response)
        : request_(request),
          response_(response)
      {
        // parseForm();
        // checkForm();
      }

    };
  }

  bool
  SettingsHandler::handle(mimosa::http::RequestReader & request,
                          mimosa::http::ResponseWriter & response) const
  {
    auto session = Session::get(request);
    auto tpl = loadTpl(session, "page.html");

    if (!tpl)
      return false;

    mimosa::tpl::Dict dict;

    auto tpl_body = loadTpl(session, "settings.html");
    if (!tpl_body)
      return false;
    dict.append("body", tpl_body);

    setPageHeader(session, dict);
    setPageFooter(session, dict);

    response.status_ = mimosa::http::kStatusOk;
    response.content_type_ = "text/html";
    response.sendHeader();
    tpl->execute(&response, dict);
    return true;
  }
}
