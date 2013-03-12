#ifndef SCISSY_ROLE_HH
# define SCISSY_ROLE_HH

# include <string>

namespace scissy
{
  enum Role
  {
    kAdministrator = 0,
    kContributor = 1,
    kObserver = 2,
  };

  const char * roleName(Role         role);
  bool parseRole(const std::string & str,
                 Role *              role);
}

#endif /* !SCISSY_ROLE_HH */
