#ifndef BLUEGITF_DB_HH
# define BLUEGITF_DB_HH

# include <mimosa/container/singleton.hh>
# include <mimosa/sqlite/sqlite.hh>

namespace bluegitf
{
  class Db : public mimosa::container::Singleton<Db>
  {
  public:
    Db();

    static mimosa::sqlite::Stmt&& prepare(const char *rq);

  private:
    mimosa::sqlite::Db db_;
  };
}

#endif /* !BLUEGITF_DB_HH */
