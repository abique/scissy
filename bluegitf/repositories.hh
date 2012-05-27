#ifndef BLUEGITF_REPOSITORIES_HH
# define BLUEGITF_REPOSITORIES_HH

# include <mimosa/singleton.hh>

namespace bluegitf
{
  class Repositories : public mimosa::Singleton<Repositories>
  {
  public:
    bool addOwner(const std::string & name,
                  const std::string & owner);

    bool create(const std::string & name,
                const std::string & description,
                const std::string & owner,
                std::string *       error);

    bool getId(const std::string & name,
               int64_t *           id);

    bool getRepoPath(const std::string & name,
                     std::string *       path);

  private:
    Repositories();
    ~Repositories();

    friend class mimosa::Singleton<Repositories>;
  };
}

#endif /* !BLUEGITF_REPOSITORIES_HH */
