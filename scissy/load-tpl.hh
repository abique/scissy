#ifndef SCISSY_LOAD_TPL_HH
# define SCISSY_LOAD_TPL_HH

# include <mimosa/tpl/template.hh>

# include "session.hh"

namespace scissy
{
  mimosa::tpl::Template::ConstPtr loadTpl(Session::Ptr        session,
                                          const std::string & path);
}

#endif /* !SCISSY_LOAD_TPL_HH */
