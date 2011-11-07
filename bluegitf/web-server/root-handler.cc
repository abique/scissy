#include <ctemplate/template.h>

#include "root-handler.hh"

namespace bluegitf
{
  namespace web_server
  {
    bool
    RootHandler::handle(mimosa::http::RequestReader & request,
                        mimosa::http::ResponseWriter & response) const
    {
      ctemplate::TemplateDictionary dict("");

      setPageHeader(session, dict);
      auto hdr_dict = dict.AddIncludeDictionary("HEADER");
      hdr_dict->SetFilename("header.html");

      std::string output;
      ctemplate::ExpandTemplate("page.html", ctemplate::DO_NOT_STRIP, &dict, &output);

      response.status_ = mimosa::http::kStatusOk;
      response.content_type_ = "text/html";
      response.write(output.data(), output.size());
      return true;
    }
  }
}
