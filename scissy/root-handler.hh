#ifndef SCISSY_ROOT_HANDLER_HH
# define SCISSY_ROOT_HANDLER_HH

# include <mimosa/http/handler.hh>

namespace scissy
{
  class RootHandler : public mimosa::http::Handler
  {
  public:
    virtual bool handle(mimosa::http::RequestReader &  request,
                        mimosa::http::ResponseWriter & response) const;
  };
}

#endif /* !SCISSY_ROOT_HANDLER_HH */
