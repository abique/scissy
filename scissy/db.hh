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

    static bool userGetId(const std::string & user, int64_t * user_id);
    static bool userIsAdmin(int64_t user_id, bool * is_admin);

    /***************/
    /* group stuff */
    /***************/

    static bool groupGetUserRole(int64_t group_id, int64_t user_id, pb::Role * role);
    static bool groupAddUser(int64_t grp_id, int64_t user_id, pb::Role role);
    static void groupRemoveUser(int64_t grp_id, int64_t user_id);
    static bool groupCreate(const std::string & grp,
                            const std::string & desc,
                            std::string *       error);
    static bool groupDelete(int64_t grp_id);
    static bool groupGetId(const std::string & group, int64_t * grp_id);

    /**************/
    /* repo stuff */
    /**************/

    static bool repoGetId(const std::string & repo, int64_t * repo_id);
    static bool repoAddUser(int64_t repo_id, int64_t user_id, pb::Role role);
    static bool repoRemoveUser(int64_t repo_id, int64_t user_id);
    static bool repoAddGroup(int64_t repo_id, int64_t group_id, pb::Role role);
    static bool repoRemoveGroup(int64_t repo_id, int64_t group_id);
    static bool repoGetUserRole(int64_t repo_id, int64_t user_id, pb::Role * role);
    static bool repoIsPublic(int64_t repo_id, bool * is_public);


  private:
    mimosa::sqlite::Db db_;
  };
}

#endif /* !SCISSY_DB_HH */
