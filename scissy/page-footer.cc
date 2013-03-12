#include <mimosa/tpl/include.hh>

#include "page-footer.hh"
#include "load-tpl.hh"

namespace scissy
{
  bool setPageFooter(Session::Ptr        session,
                     mimosa::tpl::Dict & dict)
  {
    auto tpl = loadTpl(session, "footer.html");
    if (!tpl)
      return false;

    dict.append("footer", tpl);
    return true;
  }
}
