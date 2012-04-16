#ifndef BLUEGITF_CONFIG_HH
# define BLUEGITF_CONFIG_HH

# include <string>

# include <mimosa/container/singleton.hh>

namespace bluegitf
{
  class Config : public mimosa::container::Singleton<Config>
  {
  public:
    Config();

    inline const std::string & rootDir() const { return root_dir_; }
    inline const std::string & wwwDir() const { return www_dir_; }
    inline const std::string & cssDir() const { return css_dir_; }
    inline const std::string & jsDir() const { return css_dir_; }
    inline const std::string & imgDir() const { return css_dir_; }
    inline const std::string & tplDir() const { return tpl_dir_; }
    inline const std::string & keyPem() const { return key_pem_; }
    inline const std::string & certPem() const { return cert_pem_; }
    inline const std::string & repoDir() const { return repo_dir_; }
    inline const std::string & dbDir() const { return db_dir_; }
    inline const std::string & dbPath() const { return db_path_; }
    inline const std::string & sshDir() const { return ssh_dir_; }

  private:
    std::string root_dir_;
    std::string www_dir_;
    std::string css_dir_;
    std::string js_dir_;
    std::string img_dir_;
    std::string tpl_dir_;
    std::string key_pem_;
    std::string cert_pem_;
    std::string repo_dir_;
    std::string db_dir_;
    std::string db_path_;
    std::string ssh_dir_;
  };
}

#endif /* !BLUEGITF_CONFIG_HH */
