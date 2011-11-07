#ifndef BLUEGITF_WEB_SERVER_PAGE_HEADER_HH
# define BLUEGITF_WEB_SERVER_PAGE_HEADER_HH

# include <ctemplate/template.h>

# include "session.hh"

namespace bluegitf
{
  namespace web_server
  {
    bool setPageHeader(Session::Ptr                    session,
                       ctemplate::TemplateDictionary * dict);
  }
}

#endif /* !BLUEGITF_WEB_SERVER_PAGE_HEADER_HH */
