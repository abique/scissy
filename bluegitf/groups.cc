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
    auto stmt = Db::prepare(
      "insert into groups_users (group_id, user_id, role_id) values"
      " ((select group_id from groups where name = ?),"
      " (select user_id from users where login = ?),"
      " ?)");
    return stmt.bind(group, user, role).step() == SQLITE_DONE;
  }

  void
  Groups::removeUser(const std::string & group,
                     const std::string & user)
  {
    auto stmt = Db::prepare(
      "delete from groups_users"
      " where group_id = (select group_id from groups where name = ?)"
      " and user_id = (select user_id from users where login = ?)");
    stmt.bind(group, user).exec();
  }

  bool
  Groups::getUserRole(const std::string & group,
                      const std::string & user,
                      Role *              role)
  {
    auto stmt = Db::prepare(
      "select role_id from groups_users_view"
      " where name = ? and user = ?");
    return stmt.bind(group, user).fetch(reinterpret_cast<int *>(role));
  }

  bool
  Groups::getId(const std::string & group,
                int64_t *           id)
  {
    auto stmt = Db::prepare("select group_id from groups where name = ?");
    return stmt.bind(group).fetch(id);
  }

  bool
  Groups::create(const std::string & group,
                 const std::string & desc,
                 const std::string & owner,
                 std::string *       error)
  {
    // insert the data into sqlite
    {
      auto stmt = Db::prepare("insert or fail into groups (name, desc) values (?, ?)");
      int err = stmt.bind(group, desc).step();

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
      auto stmt = Db::prepare("delete from groups where name = ?");
      stmt.bind(group).exec();
      return false;
    }

    return true;
  }
}
