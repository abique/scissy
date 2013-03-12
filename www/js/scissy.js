var scissy_module = angular.module('scissy', ['ngCookies']);
scissy_module.config(function($routeProvider) {
    $routeProvider
        .when('/', {controller:indexCtrl, templateUrl:'html/index.html'})
        .when('/login', {controller:loginCtrl, templateUrl:'html/login.html'})
        .when('/register', {controller:registerCtrl, templateUrl:'html/register.html'})
        .when('/settings', {controller:settingsCtrl, templateUrl:'html/settings.html'});
});

function resetSession($rootScope, $cookies) {
    $rootScope.session = {
        user: null,
        token: null,
        email: null,
        role: null
    };
    $cookies.auth_user  = null;
    $cookies.auth_token = null;
}

function userCheckAuthToken($rootScope, $cookies, $http) {
    if ($cookies.auth_user && $cookies.auth_token) {
        // check the session
        $http.post('/api/userCheckAuthToken',
                   { 'user':$cookies.auth_user,
                     'token':$cookies.auth_token })
            .success(function (data, status, headers, config) {
                if (data.status == "kSucceed") {
                    $rootScope.session.user  = data.user;
                    $rootScope.session.token = data.token;
                    $rootScope.session.email = data.email;
                    $rootScope.session.role  = data.role;
                    $cookies.auth_user       = data.user;
                    $cookies.auth_token      = data.token;
                    return;
                } else
                    resetSession($rootScope, $cookies);
            })
            .error(function (data, status, headers, config) {
                resetSession($rootScope, $cookies);
                rpcGenericError(data, status, headers, config);
            });
    } else
        resetSession($rootScope, $cookies);
}

scissy_module.run(function($rootScope, $cookies, $http) {
    $rootScope.session = {
        user: $cookies.auth_user,
        token: $cookies.auth_token,
        email: null,
        role: null
    };

    userCheckAuthToken($rootScope, $cookies, $http);
});

function rpcGenericError(data, status, headers, config) {
    alert('rpc error: ' + data);
}

function indexCtrl($scope) {
}

function loginCtrl($scope, $rootScope, $http, $location, $cookies) {
    $scope.reset = function() {
        $scope.user = {};
    }

    $scope.login = function(user) {
        $http.post('/api/userAuth',
                   { 'user':user.login,
                     'password':user.password })
            .success(function (data, status, headers, config) {
                user.errmsg = null;
                if (data.status == "kSucceed") {
                    $rootScope.session.user  = data.user;
                    $rootScope.session.token = data.token;
                    $rootScope.session.email = data.email;
                    $rootScope.session.role  = data.role;
                    $cookies.auth_user       = data.user;
                    $cookies.auth_token      = data.token;
                    $location.path("/");
                    return;
                }
                user.errmsg = data.msg;
            })
            .error(rpcGenericError);
    }
}

function registerCtrl($scope, $http, $location) {
    $scope.reset = function() {
        $scope.user = {};
    }

    $scope.register = function(user) {
        $http.post('/api/userCreate',
                   { 'user':user.login,
                     'email':user.email,
                     'password':user.password })
            .success(function (data, status, headers, config) {
                user.errmsg = null;
                if (data.status == "kSucceed") {
                    $location.path("/login")
                    return;
                }
                user.errmsg = data.msg;
            })
            .error(rpcGenericError);
    }
}

function settingsCtrl($scope) {

}

function settingsAccountCtrl($scope, $rootScope) {
}

function headerCtrl($scope, $rootScope, $http, $cookies) {
    $scope.logout = function() {
        $http.post('/api/userRevokeAuthToken',
                   { 'user':$rootScope.session.user,
                     'token':$rootScope.session.token });
        resetSession($rootScope, $cookies);
    };
}
