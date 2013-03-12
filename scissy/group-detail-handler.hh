#ifndef SCISSY_GROUP_DETAIL_HANDLER_HH
# define SCISSY_GROUP_DETAIL_HANDLER_HH

# include <mimosa/http/handler.hh>

namespace scissy
{
  class GroupDetailHandler : public mimosa::http::Handler
  {
  public:
    virtual bool handle(mimosa::http::RequestReader &  request,
                        mimosa::http::ResponseWriter & response) const;
  };
}

#endif /* !SCISSY_GROUP_DETAIL_HANDLER_HH */
