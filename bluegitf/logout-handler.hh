#ifndef BLUEGITF_LOGOUT_HANDLER_HH
# define BLUEGITF_LOGOUT_HANDLER_HH

# include <mimosa/http/handler.hh>

namespace bluegitf
{
  class LogoutHandler : public mimosa::http::Handler
  {
  public:
    virtual bool handle(mimosa::http::RequestReader &  request,
                        mimosa::http::ResponseWriter & response) const;
  };
}

#endif /* !BLUEGITF_LOGOUT_HANDLER_HH */
