#include <cerrno>

#include <mimosa/init.hh>
#include <mimosa/log/log.hh>
#include <mimosa/options/options.hh>
#include <mimosa/http/server.hh>
#include <mimosa/http/dispatch-handler.hh>
#include <mimosa/http/fs-handler.hh>
#include <mimosa/http/log-handler.hh>

#include "../config.hh"
#include "clear-cache-handler.hh"
#include "register-handler.hh"
#include "root-handler.hh"

uint16_t & PORT = *mimosa::options::addOption<uint16_t>("", "port", "the port to use", 19042);
bool & UNSECURE = *mimosa::options::addSwitch("", "unsecure", "disable https, usefull for dev");

int main(int argc, char ** argv)
{
  mimosa::init(argc, argv);

  auto dispatch = new mimosa::http::DispatchHandler;
  dispatch->registerHandler(
    "/css/*", new mimosa::http::FsHandler(
      bluegitf::Config::instance().cssDir(), 1, true));
  dispatch->registerHandler("/", new bluegitf::::RootHandler);
  dispatch->registerHandler("/clear-cache", new bluegitf::::ClearCacheHandler);
  dispatch->registerHandler("/register", new bluegitf::::RegisterHandler);

  auto log_handler = new mimosa::http::LogHandler;
  log_handler->setHandler(dispatch);

  mimosa::http::Server::Ptr server(new mimosa::http::Server);
  server->setHandler(log_handler);
  if (!UNSECURE)
    server->setSecure(bluegitf::Config::instance().certPem(),
                      bluegitf::Config::instance().keyPem());

  if (!server->listenInet4(PORT))
  {
    MIMOSA_LOG(Fatal, NULL, "failed to listen on the port %d: %s",
               PORT, ::strerror(errno));
    return 1;
  }

  while (true)
    server->serveOne();

  mimosa::deinit();
  return 0;
}
