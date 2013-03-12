#ifndef SCISSY_CLEAR_CACHE_HANDLER_HH
# define SCISSY_CLEAR_CACHE_HANDLER_HH

# include <mimosa/http/handler.hh>

namespace scissy
{
  class ClearCacheHandler : public mimosa::http::Handler
  {
  public:
    virtual bool handle(mimosa::http::RequestReader & request,
                        mimosa::http::ResponseWriter & response) const;
  };
}

#endif /* !SCISSY_CLEAR_CACHE_HANDLER_HH */
