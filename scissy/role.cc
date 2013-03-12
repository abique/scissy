#include <cstring>

#include "role.hh"

namespace scissy
{
  const char * roleName(Role role)
  {
    switch (role)
    {
    case kAdministrator: return "admin";
    case kContributor: return "contrib";
    case kObserver: return "observer";
    default: return "(unknown)";
    }
  }

  bool parseRole(const std::string & str,
                 Role *              role)
  {
    if (!strcasecmp("observer", str.c_str()))
    {
      *role = kObserver;
      return true;
    }

    if (!strcasecmp("contributor", str.c_str()))
    {
      *role = kContributor;
      return true;
    }

    if (!strcasecmp("administrator", str.c_str()))
    {
      *role = kAdministrator;
      return true;
    }

    return false;
  }
}
