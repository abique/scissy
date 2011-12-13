#include <mimosa/tpl/include.hh>

#include "page-footer.hh"
#include "load-tpl.hh"

namespace bluegitf
{
  bool setPageFooter(Session::Ptr        session,
                     mimosa::tpl::Dict & dict)
  {
    auto tpl = loadTpl(session, "footer.html");
    if (!tpl)
      return false;

    auto inc = new mimosa::tpl::Include(tpl, "footer");
    dict.append(inc);

    return true;
  }
}
