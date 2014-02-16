#ifndef SCISSY_REPOSITORIES_HH
# define SCISSY_REPOSITORIES_HH

# include <mimosa/singleton.hh>

# include "service.pb.h"

namespace scissy
{
  class Repositories : public mimosa::Singleton<Repositories>
  {
  public:
    bool create(const std::string & name,
                const std::string & description,
                bool                is_public,
                int64_t             user_id,
                int64_t *           repo_id,
                std::string *       error);

    bool remove(int64_t repo_id);

    std::string getRepoPath(int64_t repo_id);

  private:
    Repositories();
    ~Repositories();

    friend class mimosa::Singleton<Repositories>;
  };
}

#endif /* !SCISSY_REPOSITORIES_HH */
