begin transaction;
alter table repos add column last_commit_ts integer default null;
pragma user_version = 1;
commit transaction;
