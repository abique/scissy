#include <mimosa/tpl/template.hh>
#include <mimosa/tpl/dict.hh>
#include <mimosa/tpl/include.hh>

#include "settings-handler.hh"
#include "session.hh"
#include "page-header.hh"
#include "page-footer.hh"
#include "load-tpl.hh"

namespace bluegitf
{
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
    dict.append(new mimosa::tpl::Include(tpl_body, "body"));

    setPageHeader(session, dict);
    setPageFooter(session, dict);

    response.status_ = mimosa::http::kStatusOk;
    response.content_type_ = "text/html";
    response.sendHeader(response.writeTimeout());
    tpl->execute(&response, dict, response.writeTimeout());
    return true;
  }
}