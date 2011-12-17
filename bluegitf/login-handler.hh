#ifndef BLUEGITF_LOGIN_HANDLER_HH
# define BLUEGITF_LOGIN_HANDLER_HH

# include <mimosa/http/handler.hh>

namespace bluegitf
{
  class LoginHandler : public mimosa::http::Handler
  {
  public:
    virtual bool handle(mimosa::http::RequestReader & request,
                        mimosa::http::ResponseWriter & response) const;
  };
}

#endif /* !BLUEGITF_LOGIN_HANDLER_HH */
