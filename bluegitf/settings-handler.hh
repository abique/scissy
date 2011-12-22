#ifndef BLUEGITF_SETTINGS_HANDLER_HH
# define BLUEGITF_SETTINGS_HANDLER_HH

# include <mimosa/http/handler.hh>

namespace bluegitf
{
  class SettingsHandler : public mimosa::http::Handler
  {
  public:
    virtual bool handle(mimosa::http::RequestReader & request,
                        mimosa::http::ResponseWriter & response) const;
  };
}

#endif /* !BLUEGITF_SETTINGS_HANDLER_HH */
