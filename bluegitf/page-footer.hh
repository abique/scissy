#ifndef BLUEGITF_PAGE_FOOTER_HH
# define BLUEGITF_PAGE_FOOTER_HH

# include <mimosa/tpl/dict.hh>

# include "session.hh"

namespace bluegitf
{
  bool setPageFooter(Session::Ptr        session,
                     mimosa::tpl::Dict & dict);
}

#endif /* !BLUEGITF_PAGE_FOOTER_HH */
