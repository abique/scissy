var scissy_module = angular.module('scissy', []);
scissy_module.config(function($routeProvider) {
    $routeProvider
        .when('/', {controller:indexCtrl, templateUrl:'html/index.html'})
        .when('/groups', {controller:groupsCtrl, templateUrl:'html/groups.html'})
        .when('/group-create', {controller:groupCreateCtrl, templateUrl:'html/group-create.html'})
        .when('/group/:grp_id', {controller:groupCtrl, templateUrl:'html/group.html'})
        .when('/new-repo', {controller:loginCtrl, templateUrl:'html/new-repo.html'})
        .when('/login', {controller:loginCtrl, templateUrl:'html/login.html'})
        .when('/register', {controller:registerCtrl, templateUrl:'html/register.html'})
        .when('/repos', {controller:loginCtrl, templateUrl:'html/repos.html'})
        .when('/settings', {controller:settingsCtrl, templateUrl:'html/settings.html'});
});

function resetSession($rootScope) {
    $rootScope.session = {
        auth: {
            user: null,
            token: null
        },
        email: null,
        role: null
    };
    localStorage.auth_user  = null;
    localStorage.auth_token = null;
}

function userCheckAuthToken($rootScope, $http) {
    if (localStorage.auth_user && localStorage.auth_token) {
        // check the session
        $http.post('/api/userCheckAuthToken',
                   { 'user':localStorage.auth_user,
                     'token':localStorage.auth_token })
            .success(function (data, status, headers, config) {
                if (data.status == "kSucceed") {
                    $rootScope.session.auth.user  = data.user;
                    $rootScope.session.auth.token = data.token;
                    $rootScope.session.email      = data.email;
                    $rootScope.session.role       = data.role;
                    localStorage.auth_user        = data.user;
                    localStorage.auth_token       = data.token;
                    return;
                } else
                    resetSession($rootScope);
            })
            .error(function (data, status, headers, config) {
                resetSession($rootScope);
                rpcGenericError(data, status, headers, config);
            });
    } else
        resetSession($rootScope);
}

scissy_module.run(function($rootScope, $http) {
    $rootScope.session = {
        auth: {
            user: localStorage.auth_user,
            token: localStorage.auth_token,
        },
        email: null,
        role: null
    };

    userCheckAuthToken($rootScope, $http);
});

function rpcGenericError(data, status, headers, config) {
    alert('rpc error: ' + data);
}

function indexCtrl($scope) {
}

function loginCtrl($scope, $rootScope, $http, $location) {
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
                    $rootScope.session.auth.user  = data.user;
                    $rootScope.session.auth.token = data.token;
                    $rootScope.session.email      = data.email;
                    $rootScope.session.role       = data.role;
                    localStorage.auth_user        = data.user;
                    localStorage.auth_token       = data.token;
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

function headerCtrl($scope, $rootScope, $http) {
    $scope.logout = function() {
        $http.post('/api/userRevokeAuthToken', $rootScope.session.auth);
        resetSession($rootScope);
    };
}

function groupCreateCtrl($scope, $rootScope, $http, $location) {
    $scope.reset = function() {
        $scope.grp = {};
    }

    $scope.create = function(grp) {
        $http.post('/api/groupCreate',
                   { 'auth':$rootScope.session.auth,
                     'grp':grp.name,
                     'desc':grp.desc })
            .success(function (data, status, headers, config) {
                grp.errmsg = null;
                if (data.status == "kSucceed") {
                    $location.path("/group/"+data.grp_id)
                    return;
                }
                grp.errmsg = data.msg;
            })
            .error(rpcGenericError);
    }
}

function groupsCtrl($scope, $rootScope, $http, $location) {
    $scope.groups = [];

    $http.post('/api/groupsList', {})
            .success(function (data, status, headers, config) {
                if (data.status == "kSucceed")
                    $scope.groups = data.grps;
            })
        .error(rpcGenericError);
}

function groupCtrl($scope, $rootScope, $http, $routeParams) {
    $scope.groups = [];
    $scope.group = {"grp":"suce"};

    $http.post('/api/groupGetInfo', {"grp_id":parseInt($routeParams.grp_id)})
            .success(function (data, status, headers, config) {
                if (data.status == "kSucceed")
                    $scope.group = data;
            })
        .error(rpcGenericError);

    $http.post('/api/groupUserList', {"grp_id":parseInt($routeParams.grp_id)})
            .success(function (data, status, headers, config) {
                if (data.status == "kSucceed")
                    $scope.users = data.users;
            })
        .error(rpcGenericError);
}
