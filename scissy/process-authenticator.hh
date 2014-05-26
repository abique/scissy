#ifndef SCISSY_PROCESS_AUTHENTICATOR_HH
# define SCISSY_PROCESS_AUTHENTICATOR_HH

# include <mimosa/process.hh>

# include "authenticator.hh"

namespace scissy
{
  class ProcessAuthenticator : public Authenticator
  {
  public:
    inline void setConfig(const mimosa::ProcessConfig & cfg) { cfg_ = cfg; }

    virtual bool auth(pb::UserAuth & request,
                      pb::Session & response) const;

  private:
    mimosa::ProcessConfig cfg_;
  };
}

#endif /* !SCISSY_PROCESS_AUTHENTICATOR_HH */
