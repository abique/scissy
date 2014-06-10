#! /bin/bash -e

ANGULARJS_VERSION=1.2.17
JQUERY_VERSION=2.1.1
BOOTSTRAP_VERSION=3.1.1
CODEMIRROR_VERSION=4.2

# install jquery
mkdir -p vendor/jquery
wget https://code.jquery.com/jquery-${JQUERY_VERSION}.min.js -O vendor/jquery/jquery.min.js
wget https://code.jquery.com/jquery-${JQUERY_VERSION}.js -O vendor/jquery/jquery.js

# install angularjs
wget https://code.angularjs.org/${ANGULARJS_VERSION}/angular-${ANGULARJS_VERSION}.zip
rm -rf vendor/angular
unzip angular-${ANGULARJS_VERSION}.zip
mv angular-${ANGULARJS_VERSION} vendor/angular
rm angular-${ANGULARJS_VERSION}.zip

# install bootstrap
wget https://github.com/twbs/bootstrap/releases/download/v${BOOTSTRAP_VERSION}/bootstrap-${BOOTSTRAP_VERSION}-dist.zip
rm -rf vendor/bootstrap
unzip bootstrap-${BOOTSTRAP_VERSION}-dist.zip
mv bootstrap-${BOOTSTRAP_VERSION}-dist vendor/bootstrap
rm bootstrap-${BOOTSTRAP_VERSION}-dist.zip

# install codemirror
wget http://codemirror.net/codemirror-${CODEMIRROR_VERSION}.zip
rm -rf vendor/codemirror
unzip codemirror-${CODEMIRROR_VERSION}.zip
mv codemirror-${CODEMIRROR_VERSION} vendor/codemirror
rm codemirror-${CODEMIRROR_VERSION}.zip
