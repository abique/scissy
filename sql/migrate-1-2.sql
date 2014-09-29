begin transaction;
pragma user_version = 2;
pragma foreign_keys = OFF;

alter table users rename to users_1;

create table if not exists users
(
  user_id integer not null primary key autoincrement,
  login text not null unique,
  email text not null unique,
  password_type text not null,
  password_salt text not null,
  password_hash text not null,
  role_id integer not null default 2
);

insert into users (user_id, login, email, password_type, password_salt,
                   password_hash, role_id)
       select user_id, login, email, "NEW-PASS", "", "", role_id from users_1;

commit transaction;
