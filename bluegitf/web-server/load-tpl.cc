#include <mimosa/tpl/cache.hh>

#include "load-tpl.hh"
#include "../config.hh"

namespace bluegitf
{
  namespace web_server
  {
    mimosa::tpl::Template::Ptr loadTpl(Session::Ptr        session,
                                       const std::string & path)
    {
      static mimosa::tpl::Cache cache;

      std::string real_path(Config::instance().wwwDir() + "/" + path);

      return cache.get(real_path)->get();
    }
  }
}
