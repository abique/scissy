#ifndef BLUEGITF_WEB_SERVER_CLEAR_CACHE_HANDLER_HH
# define BLUEGITF_WEB_SERVER_CLEAR_CACHE_HANDLER_HH

# include <mimosa/http/handler.hh>

namespace bluegitf
{
  namespace web_server
  {
    class ClearCacheHandler : public mimosa::http::Handler
    {
    public:
      virtual bool handle(mimosa::http::RequestReader & request,
                          mimosa::http::ResponseWriter & response) const;
    };
  }
}

#endif /* !BLUEGITF_WEB_SERVER_CLEAR_CACHE_HANDLER_HH */
