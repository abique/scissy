#ifndef BLUEGITF_REGISTER_HANDLER_HH
# define BLUEGITF_REGISTER_HANDLER_HH

# include <mimosa/http/handler.hh>

namespace bluegitf
{
  class RegisterHandler : public mimosa::http::Handler
  {
  public:
    virtual bool handle(mimosa::http::RequestReader & request,
                        mimosa::http::ResponseWriter & response) const;
  };
}

#endif /* !BLUEGITF_REGISTER_HANDLER_HH */
