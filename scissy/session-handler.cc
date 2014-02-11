#include "session-handler.hh"

namespace scissy
{
  thread_local Session * thread_session = nullptr;

  SessionHandler::SessionHandler(mimosa::http::Handler::Ptr handler)
    : handler_(handler)
  {
  }

  Session &
  SessionHandler::threadSession()
  {
    return *thread_session;
  }

  bool
  SessionHandler::handle(mimosa::http::RequestReader & request,
                         mimosa::http::ResponseWriter & response) const
  {
    Session session(request, response);
    thread_session = &session;
    return handler_->handle(request, response);
  }
}
