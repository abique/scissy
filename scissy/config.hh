#ifndef SCISSY_CONFIG_HH
# define SCISSY_CONFIG_HH

# include <string>
# include <vector>

# include <mimosa/singleton.hh>

namespace scissy
{
  class Config : public mimosa::Singleton<Config>
  {
  public:
    Config();

    struct Listen {
      inline bool isSslEnabled() const { return !ssl_key_.empty() && !ssl_cert_.empty(); }

      std::string addr_;
      uint16_t port_;
      std::string ssl_key_;
      std::string ssl_cert_;
    };

    inline const std::string & www() const { return www_; }
    inline const std::string & css() const { return css_; }
    inline const std::string & js() const { return js_; }
    inline const std::string & vendor() const { return vendor_; }
    inline const std::string & html() const { return html_; }
    inline const std::string & repos() const { return repos_; }
    inline const std::string & db() const { return db_; }
    inline const std::string & authKeys() const { return auth_keys_; }
    inline const std::string & socket() const { return socket_; }
    inline bool crackPasswords() const { return crack_passwords_; }
    inline const std::string & cloneUser() const { return clone_user_; }
    inline const std::string & cloneHost() const { return clone_host_; }
    inline const std::vector<Listen> & listens() const { return listens_; }
    inline uint32_t gitLogLimit() const { return git_log_limit_; }
    inline const std::string & user() const { return user_; }
    inline const std::string & group() const { return group_; }
    // duration in hours
    inline uint32_t cookieDuration() const { return cookie_duration_; }

  private:
    std::string www_;
    std::string css_;
    std::string js_;
    std::string vendor_;
    std::string html_;
    std::string cm_;
    std::string repos_;
    std::string db_;
    std::string auth_keys_;
    std::string socket_;
    bool crack_passwords_;
    std::string clone_user_;
    std::string clone_host_;
    std::vector<Listen> listens_;
    uint32_t git_log_limit_;
    std::string user_;
    std::string group_;
    uint32_t cookie_duration_;
  };
}

#endif /* !SCISSY_CONFIG_HH */
