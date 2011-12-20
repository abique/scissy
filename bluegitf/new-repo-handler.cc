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
#include "new-repo-handler.hh"
#include "session.hh"

namespace bluegitf
{
  namespace
  {
    struct RegisterCtx
    {
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
