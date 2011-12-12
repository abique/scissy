#ifndef BLUEGITF_WEB_SERVER_DB_HH
# define BLUEGITF_WEB_SERVER_DB_HH

# include "../db/service.pb.h"

namespace bluegitf
{
  namespace web_server
  {
    db::pb::Db::Client::Ptr dbService();
  }
}

#endif /* !BLUEGITF_WEB_SERVER_DB_HH */
