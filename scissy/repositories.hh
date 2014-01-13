#ifndef SCISSY_REPOSITORIES_HH
# define SCISSY_REPOSITORIES_HH

# include <mimosa/singleton.hh>

# include "service.pb.h"

namespace scissy
{
  class Repositories : public mimosa::Singleton<Repositories>
  {
  public:
    bool addUser(int64_t repo_id, int64_t user_id, pb::Role role);
    bool removeUser(int64_t repo_id, int64_t user_id);

    bool addGroup(int64_t repo_id, int64_t group_id, pb::Role role);
    bool removeGroup(int64_t repo_id, int64_t group_id);

    bool create(const std::string & name,
                const std::string & description,
                int64_t             user_id,
                int64_t *           repo_id,
                std::string *       error);

    bool remove(int64_t repo_id);

    bool getId(const std::string & name,
               int64_t *           id);

    std::string getRepoPath(int64_t repo_id);

    bool getUserRole(int64_t repo_id, int64_t user_id, pb::Role * role);

    bool isPublic(int64_t repo_id, bool * is_public);

  private:
    Repositories();
    ~Repositories();

    friend class mimosa::Singleton<Repositories>;
  };
}

#endif /* !SCISSY_REPOSITORIES_HH */
