#include <mimosa/tpl/include.hh>
#include <mimosa/tpl/value.hh>

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

    if (session)
    {
      auto id = new mimosa::tpl::Dict("id");
      id->append(new mimosa::tpl::Value<std::string>(session->login_, "login"));

      dict.append(id);
    }

    return true;
  }
}
