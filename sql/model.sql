create table if not exists roles
(
  role_id integer not null primary key,
  description text not null
);

insert or replace into roles (role_id, description) values (0, "administrator");
insert or replace into roles (role_id, description) values (1, "contributor");
insert or replace into roles (role_id, description) values (2, "observer");

create table if not exists users
(
  user_id integer not null primary key autoincrement,
  login varchar(128) not null unique,
  email varchar(256) not null unique,
  password blob not null
);

create table if not exists users_keys
(
  key_id integer not null primary key autoincrement,
  user_id integer not null references users (user_id) on delete cascade on update cascade,
  `key` blob not null unique
);

create table if not exists users_auth
(
  cookie blob not null,
  user_id integer not null references users (user_id) on delete cascade on update cascade,
  ts_start integer not null,
  ts_end integer not null,
  primary key (cookie, user_id)
);

create table if not exists groups
(
  group_id integer not null primary key autoincrement,
  group_name varchar(32) not null unique
);

create table if not exists groups_users
(
  group_id integer not null references groups (group_id) on delete cascade on update cascade,
  user_id integer not null references users (user_id) on delete cascade on update cascade,
  role_id integer not null references roles (role_id) on delete cascade on update cascade,
  primary key (group_id, user_id)
);

create table if not exists repos
(
  repo_id integer not null primary key autoincrement,
  repo_name blob not null unique
);

create table if not exists repos_groups
(
  repo_id integer not null references repos (repo_id) on delete cascade on update cascade,
  group_id integer not null references groups (group_id) on delete cascade on update cascade,
  role_id integer not null references roles (role_id) on delete cascade on update cascade,
  primary key (repo_id, group_id)
);

create table if not exists repos_users
(
  repo_id integer not null references repos (repo_id) on delete cascade on update cascade,
  user_id integer not null references users (user_id) on delete cascade on update cascade,
  role_id integer not null references roles (role_id) on delete cascade on update cascade,
  primary key (repo_id, user_id)
);
