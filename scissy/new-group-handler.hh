#ifndef SCISSY_NEW_GROUP_HANDLER_HH
# define SCISSY_NEW_GROUP_HANDLER_HH

# include <mimosa/http/handler.hh>

namespace scissy
{
  class NewGroupHandler : public mimosa::http::Handler
  {
  public:
    virtual bool handle(mimosa::http::RequestReader & request,
                        mimosa::http::ResponseWriter & response) const;
  };
}

#endif /* !SCISSY_NEW_GROUP_HANDLER_HH */
