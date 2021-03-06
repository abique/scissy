#ifndef SCISSY_GROUPS_HH
# define SCISSY_GROUPS_HH

# include <mimosa/singleton.hh>

# include "role.hh"

namespace scissy
{
  class Groups : public mimosa::Singleton<Groups>
  {
  public:
    bool getUserRole(const std::string & group,
                     int64_t             user_id,
                     Role *              role);

    bool addUser(const std::string & group,
                 const std::string & user,
                 Role                role);

    void removeUser(const std::string & group,
                    const std::string & user);

    bool create(const std::string & group,
                const std::string & desc,
                const std::string & owner,
                std::string *       error);

    bool getId(const std::string & group,
               int64_t *           id);

  private:
    Groups();
    ~Groups();

    friend class mimosa::Singleton<Groups>;
  };
}

#endif /* !SCISSY_GROUPS_HH */
