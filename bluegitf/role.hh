#ifndef BLUEGITF_ROLE_HH
# define BLUEGITF_ROLE_HH

namespace bluegitf
{
  enum Role
  {
    kAdministrator = 0,
    kContributor = 1,
    kObserver = 2,
  };

  const char * roleName(Role role);
}

#endif /* !BLUEGITF_ROLE_HH */
