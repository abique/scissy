#include "role.hh"

namespace bluegitf
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
}
