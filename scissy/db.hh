#ifndef SCISSY_DB_HH
# define SCISSY_DB_HH

# include <mimosa/singleton.hh>
# include <mimosa/sqlite/sqlite.hh>

# include "service.pb.h"

namespace scissy
{
  class Db : public mimosa::Singleton<Db>
  {
  public:
    Db();

    static int64_t lastInsertRowid();

    static mimosa::sqlite::Stmt prepare(const char *rq);

    /**************/
    /* user stuff */
    /**************/

    static bool userGetId(const std::string & user, uint64_t * user_id);

    /***************/
    /* group stuff */
    /***************/

    static bool groupGetUserRole(const std::string & group,
                                 const std::string & user,
                                 pb::Role *          role);

    static bool groupAddUser(const std::string & group,
                             const std::string & user,
                             pb::Role            role);

    static void groupRemoveUser(const std::string & group,
                                const std::string & user);

    static bool groupCreate(const std::string & group,
                            const std::string & desc,
                            const std::string & owner,
                            std::string *       error);

    static bool groupDelete(const std::string & group);

    static bool groupGetId(const std::string & group,
                           int64_t *           id);

  private:
    mimosa::sqlite::Db db_;
  };
}

#endif /* !SCISSY_DB_HH */
