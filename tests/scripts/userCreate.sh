#! /bin/sh

user="$1"
email="$2"
pass="$3"

curl 'http://localhost:19043/api/userCreate' --data-binary '{"user":"'"$user"'","email":"'"$email"'","password":"'"$pass"'"}'
