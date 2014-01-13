#ifndef SCISSY_CONFIG_HH
# define SCISSY_CONFIG_HH

# include <string>

# include <mimosa/singleton.hh>

namespace scissy
{
  class Config : public mimosa::Singleton<Config>
  {
  public:
    Config();

    inline const std::string & www() const { return www_; }
    inline const std::string & css() const { return css_; }
    inline const std::string & js() const { return js_; }
    inline const std::string & fonts() const { return fonts_; }
    inline const std::string & html() const { return html_; }
    inline const std::string & sslKey() const { return ssl_key_; }
    inline const std::string & sslCert() const { return ssl_cert_; }
    inline const std::string & repos() const { return repos_; }
    inline const std::string & db() const { return db_; }
    inline const std::string & authKeys() const { return auth_keys_; }
    inline const std::string & socket() const { return socket_; }
    inline bool isSecure() const { return is_secure_; }
    inline bool crackPasswords() const { return crack_passwords_; }
    inline const std::string & cloneUser() const { return clone_user_; }
    inline const std::string & cloneHost() const { return clone_host_; }

  private:
    std::string www_;
    std::string css_;
    std::string js_;
    std::string fonts_;
    std::string html_;
    std::string ssl_key_;
    std::string ssl_cert_;
    std::string repos_;
    std::string db_;
    std::string auth_keys_;
    std::string socket_;
    bool is_secure_;
    bool crack_passwords_;
    std::string clone_user_;
    std::string clone_host_;
  };
}

#endif /* !SCISSY_CONFIG_HH */
