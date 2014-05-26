#! /bin/bash

{
  echo "$USERNAME"
  echo "$PASSWORD"
  echo "$1"
  echo "$2"
  echo "$ENV1"
  echo "$ENV2"
} >/tmp/scissy-auth-debug
