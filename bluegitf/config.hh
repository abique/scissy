#ifndef BLUEGITF_CONFIG_HH
# define BLUEGITF_CONFIG_HH

# include <string>

namespace bluegitf
{
  class Config
  {
  public:
    Config();

    static Config & instance();

    inline const std::string & rootDir() { return root_dir_; }
    inline const std::string & wwwDir() { return www_dir_; }
    inline const std::string & repoDir() { return repo_dir_; }
    inline const std::string & dbDir() { return db_dir_; }
    inline const std::string & dbSocket() { return db_socket_; }
    inline const std::string & sshDir() { return ssh_dir_; }

  private:
    std::string root_dir_;
    std::string www_dir_;
    std::string repo_dir_;
    std::string db_dir_;
    std::string db_socket_;
    std::string ssh_dir_;
  };
}

#endif /* !BLUEGITF_CONFIG_HH */
