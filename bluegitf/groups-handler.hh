#ifndef BLUEGITF_GROUPS_HANDLER_HH
# define BLUEGITF_GROUPS_HANDLER_HH

# include <mimosa/http/handler.hh>

namespace bluegitf
{
  class GroupsHandler : public mimosa::http::Handler
  {
  public:
    virtual bool handle(mimosa::http::RequestReader & request,
                        mimosa::http::ResponseWriter & response) const;
  };
}

#endif /* !BLUEGITF_GROUP_LIST_HANDLER_HH */
