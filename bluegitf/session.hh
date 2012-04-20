#ifndef BLUEGITF_SESSION_HH
# define BLUEGITF_SESSION_HH

# include <mimosa/http/request.hh>

namespace bluegitf
{
  class Session : public mimosa::RefCountable<Session>
  {
  public:
    static Session::Ptr get(const mimosa::http::Request & request);

    std::string login_;
    std::string auth_;
  };
}

#endif /* !BLUEGITF_SESSION_HH */
