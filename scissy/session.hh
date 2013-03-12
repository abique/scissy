#ifndef SCISSY_SESSION_HH
# define SCISSY_SESSION_HH

# include <mimosa/http/request.hh>

namespace scissy
{
  class Session : public mimosa::RefCountable<Session>
  {
  public:
    static Session::Ptr get(const mimosa::http::Request & request);

    std::string login_;
    std::string auth_;
  };
}

#endif /* !SCISSY_SESSION_HH */
