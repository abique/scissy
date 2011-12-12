#ifndef BLUEGITF_WEB_SERVER_LOAD_TPL_HH
# define BLUEGITF_WEB_SERVER_LOAD_TPL_HH

# include <mimosa/tpl/template.hh>

# include "session.hh"

namespace bluegitf
{
  namespace web_server
  {
    mimosa::tpl::Template::Ptr loadTpl(Session::Ptr        session,
                                       const std::string & path);
  }
}

#endif /* !BLUEGITF_WEB_SERVER_LOAD_TPL_HH */
