#include <mimosa/format/format.hh>
#include <mimosa/sqlite/sqlite.hh>

#include <git2.h>

#include "config.hh"
#include "db.hh"
#include "groups.hh"

namespace bluegitf
{
  Groups::Groups()
  {
  }

  Groups::~Groups()
  {
  }

  bool
  Groups::addUser(const std::string & group,
                  const std::string & user,
                  Role                role)
  {
    mimosa::sqlite::Stmt stmt;
    stmt.prepare(Db::handle(),
                 "insert into groups_users (group_id, user_id, role_id) values"
                 " ((select group_id from groups where name = ?),"
                 " (select user_id from users where login = ?),"
                 " ?)").bind(group, user, role);

    return stmt.step() == SQLITE_DONE;
  }

  bool
  Groups::removeUser(const std::string & group,
                     const std::string & user)
  {
    mimosa::sqlite::Stmt stmt;
    stmt.prepare(Db::handle(),
                 "delete from groups_users"
                 " where group_id = (select group_id from groups where name = ?)"
                 " and user_id = (select user_id from users where login = ?)");
    stmt.bind(group, user).exec();
    return true;
  }

  bool
  Groups::getUserRole(const std::string & group,
                      const std::string & user,
                      Role *              role)
  {
    mimosa::sqlite::Stmt stmt;
    stmt.prepare(Db::handle(),
                 "select role_id from groups_users_view"
                 " where name = ? and user = ?").bind(group, user);

    return stmt.fetch(reinterpret_cast<int *>(&role));
  }

  bool
  Groups::getId(const std::string & group,
                int64_t *           id)
  {
    mimosa::sqlite::Stmt stmt;
    stmt.prepare(Db::handle(),
                 "select group_id from groups where name = ?")
      .bind(group);
    return stmt.fetch(id);
  }

  bool
  Groups::create(const std::string & group,
                 const std::string & desc,
                 const std::string & owner,
                 std::string *       error)
  {
    // insert the data into sqlite
    {
      mimosa::sqlite::Stmt stmt;
      stmt.prepare(Db::handle(),
                   "insert or fail into groups (name, desc)"
                   " values (?, ?)");
      stmt.bind(group, desc);

      int err = stmt.step();
      if (err == SQLITE_CONSTRAINT)
      {
        *error = "name already used";
        return false;
      }

      if (err != SQLITE_DONE)
      {
        *error = "failed to register, internal error";
        return false;
      }
    }

    if (!addUser(group, owner, kAdministrator))
    {
      *error = "failed to set owner";
      // XXX remove the group
      return false;
    }

    return true;
  }
}
