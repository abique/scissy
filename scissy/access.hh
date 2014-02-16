#ifndef SCISSY_ACCESS_HH
# define SCISSY_ACCESS_HH

# include "session.hh"

namespace scissy
{
  bool accessCheckRepo(const Session & session,
                       uint64_t        repo_id,
                       pb::Role        role);
}

#endif /* !SCISSY_ACCESS_HH */
