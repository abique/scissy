#! /bin/bash

LESSC=$HOME/.npm/bin/recess
UGLIFYJS=$HOME/.npm/bin/uglifyjs
ANGULARJS_VERSION=1.2.7
JQUERY_VERSION=1.10.2

cp scissy.less bootstrap/less/ &&
cd bootstrap/less &&
$LESSC --compile scissy.less >../../css/scissy.css &&
cd ../.. &&
cp bootstrap/fonts/* fonts/ &&
cat bootstrap/js/{transition,alert,button,carousel,collapse,dropdown,modal,tooltip,popover,scrollspy,tab,affix}.js >js/bootstrap.js &&
$UGLIFYJS js/bootstrap.js >js/bootstrap.min.js &&
wget http://code.jquery.com/jquery-${JQUERY_VERSION}.min.js -O js/jquery.min.js &&
wget http://code.jquery.com/jquery-${JQUERY_VERSION}.js -O js/jquery.js &&
wget http://code.angularjs.org/${ANGULARJS_VERSION}/angular.min.js -O js/angular.min.js &&
wget http://code.angularjs.org/${ANGULARJS_VERSION}/angular.js -O js/angular.js &&
wget http://code.angularjs.org/${ANGULARJS_VERSION}/angular-cookies.min.js -O js/angular-cookies.min.js &&
wget http://code.angularjs.org/${ANGULARJS_VERSION}/angular-cookies.js -O js/angular-cookies.js
