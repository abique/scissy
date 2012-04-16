#! /bin/bash

LESSC=$HOME/.npm/bin/lessc
UGLIFYJS=$HOME/.npm/bin/uglifyjs

cp bluegitf.less bootstrap/less/ &&
cd bootstrap/less &&
$LESSC bluegitf.less >../../css/bluegitf.css &&
cd ../.. &&
cp bootstrap/img/* img/ &&
cat bootstrap/js/{bootstrap-transition,bootstrap-alert,bootstrap-button,bootstrap-carousel,bootstrap-collapse,bootstrap-dropdown,bootstrap-modal,bootstrap-tooltip,bootstrap-popover,bootstrap-scrollspy,bootstrap-tab,bootstrap-typeahead}.js >js/bootstrap.js &&
$UGLIFYJS js/bootstrap.js >js/bootstrap.min.js &&
wget http://code.jquery.com/jquery-1.7.2.min.js -O js/jquery.min.js &&
wget http://code.jquery.com/jquery-1.7.2.js -O js/jquery.js
