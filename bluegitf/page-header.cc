#include <mimosa/tpl/include.hh>

#include "page-header.hh"
#include "load-tpl.hh"

namespace bluegitf
{
  bool setPageHeader(Session::Ptr        session,
                     mimosa::tpl::Dict & dict)
  {
    auto tpl = loadTpl(session, "header.html");
    if (!tpl)
      return false;

    auto inc = new mimosa::tpl::Include(tpl, "header");
    dict.append(inc);

    return true;
  }
}
