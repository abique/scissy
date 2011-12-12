#ifndef BLUEGITF_WEB_SERVER_PAGE_FOOTER_HH
# define BLUEGITF_WEB_SERVER_PAGE_FOOTER_HH

# include <mimosa/tpl/dict.hh>

# include "session.hh"

namespace bluegitf
{
  namespace web_server
  {
    bool setPageFooter(Session::Ptr        session,
                       mimosa::tpl::Dict & dict);
  }
}

#endif /* !BLUEGITF_WEB_SERVER_PAGE_FOOTER_HH */
