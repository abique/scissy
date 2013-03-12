#ifndef SCISSY_SETTINGS_HANDLER_HH
# define SCISSY_SETTINGS_HANDLER_HH

# include <mimosa/http/handler.hh>

namespace scissy
{
  class SettingsHandler : public mimosa::http::Handler
  {
  public:
    virtual bool handle(mimosa::http::RequestReader & request,
                        mimosa::http::ResponseWriter & response) const;
  };
}

#endif /* !SCISSY_SETTINGS_HANDLER_HH */
