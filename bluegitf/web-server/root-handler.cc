#include <mimosa/tpl/template.hh>
#include <mimosa/tpl/dict.hh>

#include "root-handler.hh"
#include "session.hh"
#include "page-header.hh"
#include "page-footer.hh"
#include "load-tpl.hh"

namespace bluegitf
{
  namespace web_server
  {
    bool
    RootHandler::handle(mimosa::http::RequestReader & request,
                        mimosa::http::ResponseWriter & response) const
    {
      auto session = Session::get(request);
      auto tpl = loadTpl(session, "page.html");

      if (!tpl)
        return false;

      mimosa::tpl::Dict dict;

      setPageHeader(session, dict);
      setPageFooter(session, dict);

      response.status_ = mimosa::http::kStatusOk;
      response.content_type_ = "text/html";
      response.sendHeader(response.writeTimeout());
      tpl->execute(&response, dict, response.writeTimeout());
      return true;
    }
  }
}
