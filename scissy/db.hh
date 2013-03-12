#ifndef SCISSY_DB_HH
# define SCISSY_DB_HH

# include <mimosa/singleton.hh>
# include <mimosa/sqlite/sqlite.hh>

namespace scissy
{
  class Db : public mimosa::Singleton<Db>
  {
  public:
    Db();

    static mimosa::sqlite::Stmt prepare(const char *rq);
    static bool getUserId(const std::string & user, uint64_t * user_id);

  private:
    mimosa::sqlite::Db db_;
  };
}

#endif /* !SCISSY_DB_HH */
