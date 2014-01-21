create table if not exists roles
(
  role_id integer not null primary key,
  description text not null
);

insert or replace into roles (role_id, description) values (0, "none");
insert or replace into roles (role_id, description) values (1, "reader");
insert or replace into roles (role_id, description) values (2, "writer");
insert or replace into roles (role_id, description) values (3, "owner");

create table if not exists users
(
  user_id integer not null primary key autoincrement,
  login text not null unique,
  email text not null unique,
  password blob not null, -- sha512(password)
  role_id integer not null default 2
);

create table if not exists users_ssh_keys
(
  ssh_key_id integer not null primary key autoincrement,
  user_id integer not null references users (user_id) on delete cascade on update cascade,
  `key_type` integer not null, 
  `key` text not null,
  `desc` text not null,
  ts_created integer not null,
  unique (user_id, `key`, `key_type`)
);

create table if not exists users_auths
(
  user_id integer not null references users (user_id) on delete cascade on update cascade,
  token text not null,
  ts_start integer not null,
  primary key (token, user_id)
);

create table if not exists groups
(
  group_id integer not null primary key autoincrement,
  `name` text not null unique,
  `desc` text not null default ''
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
  `name` text not null unique,
  `desc` text not null default '',
  is_public boolean not null default true
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
