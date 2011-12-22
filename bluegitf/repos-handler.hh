#ifndef BLUEGITF_REPOS_HANDLER_HH
# define BLUEGITF_REPOS_HANDLER_HH

# include <mimosa/http/handler.hh>

namespace bluegitf
{
  class ReposHandler : public mimosa::http::Handler
  {
  public:
    virtual bool handle(mimosa::http::RequestReader & request,
                        mimosa::http::ResponseWriter & response) const;
  };
}

#endif /* !BLUEGITF_REPOS_HANDLER_HH */
