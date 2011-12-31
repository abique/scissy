#ifndef BLUEGITF_LOAD_TPL_HH
# define BLUEGITF_LOAD_TPL_HH

# include <mimosa/tpl/template.hh>

# include "session.hh"

namespace bluegitf
{
  mimosa::tpl::Template::ConstPtr loadTpl(Session::Ptr        session,
                                          const std::string & path);
}

#endif /* !BLUEGITF_LOAD_TPL_HH */
