#ifndef SCISSY_GROUPS_HANDLER_HH
# define SCISSY_GROUPS_HANDLER_HH

# include <mimosa/http/handler.hh>

namespace scissy
{
  class GroupsHandler : public mimosa::http::Handler
  {
  public:
    virtual bool handle(mimosa::http::RequestReader & request,
                        mimosa::http::ResponseWriter & response) const;
  };
}

#endif /* !SCISSY_GROUP_LIST_HANDLER_HH */
