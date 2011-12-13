#ifndef BLUEGITF_WEB_SERVER_SESSION_HH
# define BLUEGITF_WEB_SERVER_SESSION_HH

# include <mimosa/http/request.hh>

namespace bluegitf
{
  namespace web_server
  {
    class Session : public mimosa::RefCountable<Session>
    {
    public:
      static Session::Ptr get(const mimosa::http::Request & request);

      std::string user_;
      std::string email_;
    };
  }
}

#endif /* !BLUEGITF_WEB_SERVER_SESSION_HH */
