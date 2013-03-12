#include <mimosa/tpl/template.hh>
#include <mimosa/http/fs-handler.hh>

#include "root-handler.hh"
#include "config.hh"

namespace scissy
{
  bool
  RootHandler::handle(mimosa::http::RequestReader & request,
                      mimosa::http::ResponseWriter & response) const
  {
    return mimosa::http::FsHandler::streamFile(
      request, response, Config::instance().htmlDir() + "/page.html");
  }
}
