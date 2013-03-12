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
#include "repos-handler.hh"
#include "session.hh"

namespace scissy
{
  bool
  ReposHandler::handle(mimosa::http::RequestReader & request,
                       mimosa::http::ResponseWriter & response) const
  {
    auto session = Session::get(request);
    auto tpl = loadTpl(session, "page.html");

    if (!tpl)
      return false;

    mimosa::tpl::Dict dict;

    auto tpl_body = loadTpl(session, "repos.html");
    if (!tpl_body)
      return false;
    dict.append("body", tpl_body);

    setPageHeader(session, dict);
    setPageFooter(session, dict);

    auto repos = new mimosa::tpl::List("repos");
    {
      auto stmt = Db::prepare("select name, desc from repos");

      while (stmt.step() == SQLITE_ROW)
      {
        auto repo = new mimosa::tpl::Dict("repo");
        repo->append("name", (const char*)sqlite3_column_text(stmt, 0));
        repo->append("desc", (const char*)sqlite3_column_text(stmt, 1));
        repos->append(repo);
      }
    }
    dict.append(repos);

    response.status_ = mimosa::http::kStatusOk;
    response.content_type_ = "text/html";
    response.sendHeader();
    tpl->execute(&response, dict);
    return true;
  }
}
