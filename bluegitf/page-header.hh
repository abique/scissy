#ifndef BLUEGITF_PAGE_HEADER_HH
# define BLUEGITF_PAGE_HEADER_HH

# include <mimosa/tpl/dict.hh>

# include "session.hh"

namespace bluegitf
{
  bool setPageHeader(Session::Ptr        session,
                     mimosa::tpl::Dict & dict);
}

#endif /* !BLUEGITF_PAGE_HEADER_HH */
