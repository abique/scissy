#! /bin/bash

LESSC=$HOME/.npm/bin/lessc

cp bluegitf.less bootstrap/less/ &&
cd bootstrap/less &&
$LESSC bluegitf.less >../../css/bluegitf.css &&
cd ../.. &&
cp bootstrap/img/* img/
