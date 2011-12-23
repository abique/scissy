#ifndef BLUEGITF_GROUPS_HH
# define BLUEGITF_GROUPS_HH

# include <mimosa/container/singleton.hh>

# include "role.hh"

namespace bluegitf
{
  class Groups : public mimosa::container::Singleton<Groups>
  {
  public:
    bool addMember(const std::string & group,
                   const std::string & user,
                   Role                role);

    bool create(const std::string & group,
                const std::string & desc,
                const std::string & owner,
                std::string &       error);

    bool getId(const std::string & group,
               int64_t &           id);

  private:
    Groups();
    ~Groups();

    friend class mimosa::container::Singleton<Groups>;
  };
}

#endif /* !BLUEGITF_GROUPS_HH */
