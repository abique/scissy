#ifndef SCISSY_ARCHIVE_HANDLER_HH
# define SCISSY_ARCHIVE_HANDLER_HH

# include <mimosa/http/handler.hh>

namespace scissy
{
  /**
   * url: /repo/archive/:repo_id/:rev/filename.tar.xz
   */
  class ArchiveHandler : public mimosa::http::Handler
  {
  public:
    virtual bool handle(mimosa::http::RequestReader &  request,
                        mimosa::http::ResponseWriter & response) const;
  };
}

#endif /* !SCISSY_ARCHIVE_HANDLER_HH */
