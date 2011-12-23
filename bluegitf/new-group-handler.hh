#ifndef BLUEGITF_NEW_GROUP_HANDLER_HH
# define BLUEGITF_NEW_GROUP_HANDLER_HH

# include <mimosa/http/handler.hh>

namespace bluegitf
{
  class NewGroupHandler : public mimosa::http::Handler
  {
  public:
    virtual bool handle(mimosa::http::RequestReader & request,
                        mimosa::http::ResponseWriter & response) const;
  };
}

#endif /* !BLUEGITF_NEW_GROUP_HANDLER_HH */
