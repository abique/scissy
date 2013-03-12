#ifndef SCISSY_REGISTER_HANDLER_HH
# define SCISSY_REGISTER_HANDLER_HH

# include <mimosa/http/handler.hh>

namespace scissy
{
  class RegisterHandler : public mimosa::http::Handler
  {
  public:
    virtual bool handle(mimosa::http::RequestReader & request,
                        mimosa::http::ResponseWriter & response) const;
  };
}

#endif /* !SCISSY_REGISTER_HANDLER_HH */
