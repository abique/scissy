#ifndef BLUEGITF_DB_HH
# define BLUEGITF_DB_HH

# include <sqlite3.h>
# include <mimosa/container/singleton.hh>

namespace bluegitf
{
  class Db : public mimosa::container::Singleton<Db>
  {
  public:
    Db();
    ~Db();

    static sqlite3 * handle();

  private:
    sqlite3 * db_;
  };
}

#endif /* !BLUEGITF_DB_HH */
