#ifndef BLUEGITF_GROUP_DETAIL_HANDLER_HH
# define BLUEGITF_GROUP_DETAIL_HANDLER_HH

# include <mimosa/http/handler.hh>

namespace bluegitf
{
  class GroupDetailHandler : public mimosa::http::Handler
  {
  public:
    virtual bool handle(mimosa::http::RequestReader &  request,
                        mimosa::http::ResponseWriter & response) const;
  };
}

#endif /* !BLUEGITF_GROUP_DETAIL_HANDLER_HH */
