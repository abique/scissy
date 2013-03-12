#ifndef SCISSY_PAGE_HEADER_HH
# define SCISSY_PAGE_HEADER_HH

# include <mimosa/tpl/dict.hh>

# include "session.hh"

namespace scissy
{
  bool setPageHeader(Session::Ptr        session,
                     mimosa::tpl::Dict & dict);
}

#endif /* !SCISSY_PAGE_HEADER_HH */
