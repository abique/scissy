#include <mimosa/sqlite/sqlite.hh>
#include <mimosa/tpl/template.hh>
#include <mimosa/tpl/dict.hh>
#include <mimosa/tpl/include.hh>
#include <mimosa/tpl/list.hh>
#include <mimosa/tpl/value.hh>

#include "db.hh"
#include "load-tpl.hh"
#include "page-footer.hh"
#include "page-header.hh"
#include "groups-handler.hh"
#include "session.hh"

namespace bluegitf
{
  bool
  GroupsHandler::handle(mimosa::http::RequestReader & request,
                        mimosa::http::ResponseWriter & response) const
  {
    auto session = Session::get(request);
    auto tpl = loadTpl(session, "page.html");

    if (!tpl)
      return false;

    mimosa::tpl::Dict dict;

    auto tpl_body = loadTpl(session, "groups.html");
    if (!tpl_body)
      return false;
    dict.append("body", tpl_body);

    setPageHeader(session, dict);
    setPageFooter(session, dict);

    {
      auto stmt = Db::prepare("select name, desc from groups");

      auto groups = new mimosa::tpl::List("groups");
      mimosa::string::StringRef name, desc;
      while (stmt.fetch(&name, &desc))
      {
        auto group = new mimosa::tpl::Dict("group");
        group->append("name", name);
        group->append("desc", desc);
        groups->append(group);
      }
      dict.append(groups);
    }

    response.status_ = mimosa::http::kStatusOk;
    response.content_type_ = "text/html";
    response.sendHeader(response.writeTimeout());
    tpl->execute(&response, dict, response.writeTimeout());
    return true;
  }
}
