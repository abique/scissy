#ifndef BLUEGITF_ROLE_HH
# define BLUEGITF_ROLE_HH

# include <string>

namespace bluegitf
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

#endif /* !BLUEGITF_ROLE_HH */
