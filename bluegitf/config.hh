#ifndef BLUEGITF_CONFIG_HH
# define BLUEGITF_CONFIG_HH

# include <string>

# include <mimosa/tpl/template.hh>

namespace bluegitf
{
  class Config
  {
  public:
    Config();

    static Config & instance();

    inline const std::string & rootDir() const { return root_dir_; }
    inline const std::string & wwwDir() const { return www_dir_; }
    inline const std::string & cssDir() const { return css_dir_; }
    inline const std::string & tplDir() const { return tpl_dir_; }
    inline const std::string & keyPem() const { return key_pem_; }
    inline const std::string & certPem() const { return cert_pem_; }
    inline const std::string & repoDir() const { return repo_dir_; }
    inline const std::string & dbDir() const { return db_dir_; }
    inline const std::string & dbSocket() const { return db_socket_; }
    inline const std::string & sshDir() const { return ssh_dir_; }

    mimosa::tpl::Template::Ptr loadTpl(const std::string & rel_path) const;

  private:
    std::string root_dir_;
    std::string www_dir_;
    std::string css_dir_;
    std::string tpl_dir_;
    std::string key_pem_;
    std::string cert_pem_;
    std::string repo_dir_;
    std::string db_dir_;
    std::string db_socket_;
    std::string ssh_dir_;
  };
}

#endif /* !BLUEGITF_CONFIG_HH */
