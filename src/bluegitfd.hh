#ifndef BLUEGITFD_HH
# define BLUEGITFD_HH

class Bluegitfd
{
public:
  static Bluegitfd & instance();

  inline const std::string & rootDir() { return root_dir_; }
  inline const std::string & wwwDir() { return www_dir_; }
  inline const std::string & repoDir() { return repo_dir_; }
  inline const std::string & dbDir() { return db_dir_; }
  inline const std::string & sshDir() { return ssh_dir_; }

private:
  Bluegitfd();

  static Bluegitfd * instance_;

  std::string root_dir_;
  std::string www_dir_;
  std::string repo_dir_;
  std::string db_dir_;
  std::string ssh_dir_;
};

#endif /* !BLUEGITFD_HH */
