#include <mimosa/tpl/cache.hh>
#include <mimosa/log/log.hh>

#include "load-tpl.hh"
#include "config.hh"

namespace bluegitf
{
  mimosa::tpl::Template::ConstPtr loadTpl(Session::Ptr        /*session*/,
                                          const std::string & path)
  {
    static mimosa::tpl::Cache cache;

    std::string real_path(Config::instance().wwwDir() + "/tpl/" + path);

    // auto tpl = cache.get(real_path)->get();
    auto tpl = mimosa::tpl::Template::parseFile(real_path);
    if (tpl)
      return tpl.get();

    mimosa::log::error("failed to load template: %s", real_path);
    return nullptr;
  }
}
