#! /bin/bash -e

ANGULARJS_VERSION=1.2.19
JQUERY_VERSION=2.1.1
BOOTSTRAP_VERSION=3.2.0
CODEMIRROR_VERSION=4.3
UI_CODEMIRROR_VERSION=0.1.1

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

# ui-codemirror
rm -rf vendor/ui-codemirror
mkdir vendor/ui-codemirror
wget https://raw.githubusercontent.com/angular-ui/ui-codemirror/v${UI_CODEMIRROR_VERSION}/ui-codemirror.js -O vendor/ui-codemirror/ui-codemirror.js

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
