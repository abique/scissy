#ifndef BLUEGITF_CLEAR_CACHE_HANDLER_HH
# define BLUEGITF_CLEAR_CACHE_HANDLER_HH

# include <mimosa/http/handler.hh>

namespace bluegitf
{
  class ClearCacheHandler : public mimosa::http::Handler
  {
  public:
    virtual bool handle(mimosa::http::RequestReader & request,
                        mimosa::http::ResponseWriter & response) const;
  };
}

#endif /* !BLUEGITF_CLEAR_CACHE_HANDLER_HH */
