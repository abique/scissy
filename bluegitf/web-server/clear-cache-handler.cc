#include <ctemplate/template.h>

#include <mimosa/http/redirect.hh>

#include "clear-cache-handler.hh"

namespace bluegitf
{
  namespace web_server
  {
    bool
    ClearCacheHandler::handle(mimosa::http::RequestReader & request,
                        mimosa::http::ResponseWriter & response) const
    {
      ctemplate::Template::ClearCache();
      return mimosa::http::redirect(response, "/");
    }
  }
}
