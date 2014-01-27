'use strict';

var scissy_module = angular.module('scissy', ['ngRoute']);
scissy_module.config(function($routeProvider) {
    $routeProvider
        .when('/', {controller:indexCtrl, templateUrl:'html/index.html'})
        .when('/groups', {controller:groupsCtrl, templateUrl:'html/groups.html'})
        .when('/group-create', {controller:groupCreateCtrl, templateUrl:'html/group-create.html'})
        .when('/group/:grp_id', {controller:groupCtrl, templateUrl:'html/group.html'})
        .when('/new-repo', {controller:loginCtrl, templateUrl:'html/new-repo.html'})
        .when('/login', {controller:loginCtrl, templateUrl:'html/login.html'})
        .when('/register', {controller:registerCtrl, templateUrl:'html/register.html'})
        .when('/repos', {controller:reposCtrl, templateUrl:'html/repos.html'})
        .when('/repo-create', {controller:repoCreateCtrl, templateUrl:'html/repo-create.html'})
        .when('/repo/summary/:repo_id', {controller:repoSummaryCtrl, templateUrl:'html/repo.html'})
        .when('/repo/tree/:repo_id/:revision/:path', {controller:repoTreeCtrl, templateUrl:'html/repo.html'})
        .when('/repo/log/:repo_id', {controller:repoLogCtrl, templateUrl:'html/repo.html'})
        .when('/repo/admin/:repo_id', {controller:repoAdminCtrl, templateUrl:'html/repo.html'})
        .when('/repo/commit/:repo_id/:revision', {controller:repoCommitCtrl, templateUrl:'html/repo.html'})
        .when('/settings/account', {controller:settingsAccountCtrl, templateUrl:'html/settings.html'})
        .when('/settings/ssh-keys', {controller:settingsKeysCtrl, templateUrl:'html/settings.html'});
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

function getServerInfo($rootScope, $http) {
    $http.post('/api/serviceInfo', {})
            .success(function (data, status, headers, config) {
                $rootScope.service_info = data;
            })
            .error(function (data, status, headers, config) {
                rpcGenericError(data, status, headers, config);
            });
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
        "auth": {
            "user": localStorage.auth_user,
            "token": localStorage.auth_token,
        },
        "email": null,
        "role": null
    };
    $rootScope.service_info = {
        "clone_user" : "XXX",
        "clone_host" : "XXX",
        "version" : "XXX",
        "uptime" : 0
    };

    userCheckAuthToken($rootScope, $http);
    getServerInfo($rootScope, $http);
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
    $scope.reset();

    $scope.login = function(user) {
        $http.post('/api/userAuth',
                   { 'user':$scope.user.login,
                     'password':$scope.user.password })
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
        $scope.user = {
            login:"",
            email:""
        };
    }
    $scope.reset();

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

function reposCtrl($scope, $rootScope, $http) {
    $scope.repos = [];

    $http.post('/api/reposList', {})
            .success(function (data, status, headers, config) {
                if (data.status == "kSucceed")
                    $scope.repos = data.repos;
            })
        .error(rpcGenericError);
}

function repoCreateCtrl($scope, $rootScope, $http, $location) {
    $scope.reset = function() {
        $scope.repo = {};
    }
    $scope.reset();

    $scope.create = function(repo) {
        $http.post('/api/repoCreate',
                   { 'auth':$rootScope.session.auth,
                     'name':repo.name,
                     'desc':repo.desc,
                     'is_public':repo.is_public})
            .success(function (data, status, headers, config) {
                repo.errmsg = null;
                if (data.status == "kSucceed") {
                    $location.path("/repo/summary/"+data.id)
                    return;
                }
                repo.errmsg = data.msg;
            })
            .error(rpcGenericError);
    }
}

function repoSummaryCtrl($scope, $rootScope, $http, $location, $routeParams) {
    $scope.summary_class = "active";
    $scope.content = "html/repo-summary.html";
    $scope.repo = {"name":"", "desc":"", "is_public":true,
                   "id":parseInt($routeParams.repo_id)};

    $scope.refresh = function() {
        $http.post('/api/repoGetInfo', {'auth':$rootScope.session.auth,
                                        "repo_id":$scope.repo.id})
            .success(function (data, status, headers, config) {
                if (data.status == "kSucceed")
                    $scope.repo = data;
            })
            .error(rpcGenericError);
        $http.post('/api/repoListBranches', {'auth':$rootScope.session.auth,
                                             "repo_id":$scope.repo.id})
            .success(function (data, status, headers, config) {
                if (data.status == "kSucceed")
                    $scope.branches = data.branches;
            })
            .error(rpcGenericError);
    }

    $scope.refresh();
}

function repoTreeCtrl($scope, $rootScope, $http, $location, $routeParams) {
    $scope.tree_class = "active";
    $scope.content = "html/repo-tree.html";
    $scope.repo = {"name":"", "desc":"", "is_public":true,
                   "id":parseInt($routeParams.repo_id)};

    $scope.refresh = function() {
        $http.post('/api/repoGetInfo', {
            'auth':$rootScope.session.auth,
            "repo_id":$scope.repo.id})
            .success(function (data, status, headers, config) {
                if (data.status == "kSucceed")
                    $scope.repo = data;
            })
            .error(rpcGenericError);
        $http.post('/api/repoGetTree', {
            'auth':$rootScope.session.auth,
            "repo_id":$scope.repo.id,
            "revision":$routeParams.revision,
            "directory":$routeParams.directory})
            .success(function (data, status, headers, config) {
                if (data.status == "kSucceed")
                    $scope.entries = data.entries;
            })
            .error(rpcGenericError);
    }

    $scope.refresh();
}

function repoLogCtrl($scope, $rootScope, $http, $location, $routeParams) {
    $scope.log_class = "active";
    $scope.content = "html/repo-log.html";
    $scope.repo = {"name":"", "desc":"", "is_public":true,
                   "id":parseInt($routeParams.repo_id)};

    $scope.refresh = function() {
        $http.post('/api/repoGetInfo', {
            'auth':$rootScope.session.auth,
            "repo_id":$scope.repo.id})
            .success(function (data, status, headers, config) {
                if (data.status == "kSucceed")
                    $scope.repo = data;
            })
            .error(rpcGenericError);
        $http.post('/api/repoGetLog', {
            'auth':$rootScope.session.auth,
            "repo_id":$scope.repo.id})
            .success(function (data, status, headers, config) {
                if (data.status == "kSucceed")
                    $scope.commits = data.commits;
            })
            .error(rpcGenericError);
    }

    $scope.refresh();
}

function repoCommitCtrl($scope, $rootScope, $http, $location, $routeParams) {
    $scope.commit_class = "active";
    $scope.content = "html/repo-commit.html";
    $scope.repo = {"name":"", "desc":"", "is_public":true,
                   "id":parseInt($routeParams.repo_id)};
    $scope.revision = $routeParams.revision;
    $scope.commit = {
    };

    $scope.refresh = function() {
        $http.post('/api/repoGetInfo', {
            'auth':$rootScope.session.auth,
            "repo_id":$scope.repo.id})
            .success(function (data, status, headers, config) {
                if (data.status == "kSucceed")
                    $scope.repo = data;
            })
            .error(rpcGenericError);
        $http.post('/api/repoGetCommit', {
            'auth':$rootScope.session.auth,
            "repo_id":$scope.repo.id,
            "revision":$scope.revision})
            .success(function (data, status, headers, config) {
                $scope.commit = data;
            })
            .error(rpcGenericError);
    }

    $scope.refresh();
}

function repoAdminCtrl($scope, $rootScope, $http, $location, $routeParams) {
    $scope.admin_class = "active";
    $scope.content = "html/repo-admin.html";
    $scope.repo = {"name":"", "id":parseInt($routeParams.repo_id),
                   "desc":"", "is_public": false};
    $scope.members = [];
    $scope.users = [];
    $scope.groups = [];
    $scope.new_user = {"user":"","role":"kReader"};
    $scope.new_group = {"group":"","role":"kReader"};
    $scope.is_admin = false;

    $scope.refresh = function() {
        $http.post('/api/repoGetInfo', {
            'auth':$rootScope.session.auth,
            "repo_id":$scope.repo.id})
            .success(function (data, status, headers, config) {
                if (data.status == "kSucceed")
                    $scope.repo = data;
            })
            .error(rpcGenericError);

        $http.post('/api/repoListMembers', {
            'auth':$rootScope.session.auth,
            "repo_id":$scope.repo.id})
            .success(function (data, status, headers, config) {
                if (data.status == "kSucceed") {
                    $scope.users = data.users;
                    $scope.groups = data.groups;
                    var i;
                    for (i in data.users) {
                        if (data.users[i].user == $rootScope.session.auth.user)
                            $scope.is_admin = (data.users[i].role == "kOwner");
                    }
                }
            })
            .error(rpcGenericError);
    }

    $scope.update = function(repo) {
        $http.post('/api/repoUpdate', {
            'auth':$rootScope.session.auth,
            "repo_id":$scope.repo.id,
            "name":repo.name,
            "desc":repo.desc,
            "is_public":repo.is_public})
            .success(function (data, status, headers, config) {
                $scope.repo.errmsg = data.errmsg;
                $scope.refresh();
            })
            .error(rpcGenericError);
    }

    $scope.destroy = function(repo) {
        $http.post('/api/repoDelete', {
            'auth':$rootScope.session.auth,
            "repo_id":$scope.repo.id})
            .success(function (data, status, headers, config) {
                if (data.status == "kSucceed") {
                    $location.path("/repos");
                } else {
                    $scope.repo.errmsg = data.errmsg;
                    $scope.refresh();
                }
            })
            .error(rpcGenericError);
    }

    $scope.addUser = function(new_user) {
        $http.post('/api/repoAddUser',
                   { 'auth':$rootScope.session.auth,
                     'repo_id':$scope.repo.id,
                     'user':new_user.user,
                     'role':new_user.role })
            .success(function (data, status, headers, config) {
                new_user.errmsg = null;
                if (data.status == "kSucceed") {
                    $scope.refresh();
                    return;
                }
                new_user.errmsg = data.msg;
            })
            .error(rpcGenericError);
    }

    $scope.addGroup = function(new_group) {
        $http.post('/api/repoAddGroup',
                   { 'auth':$rootScope.session.auth,
                     'repo_id':$scope.repo.id,
                     'grp':new_group.group,
                     'role':new_group.role })
            .success(function (data, status, headers, config) {
                new_group.errmsg = null;
                if (data.status == "kSucceed") {
                    $scope.refresh();
                    return;
                }
                new_group.errmsg = data.msg;
            })
            .error(rpcGenericError);
    }

    $scope.userUpdate = function(user, role) {
    }

    $scope.removeUser = function(user) {
        $http.post('/api/repoRemoveUser',
                   { 'auth':$rootScope.session.auth,
                     'repo_id':$scope.repo.id,
                     'user':user.user })
            .success(function (data, status, headers, config) {
                $scope.new_user.errmsg = null;
                if (data.status == "kSucceed") {
                    $scope.refresh();
                    return;
                }
                new_user.errmsg = data.msg;
            })
            .error(rpcGenericError);
    }

    $scope.refresh();
}

function settingsAccountCtrl($scope, $rootScope) {
    $scope.account_class = "active";
    $scope.content = "html/settings-account.html";
}

function settingsKeysCtrl($scope, $rootScope, $http) {
    $scope.keys_class = "active";
    $scope.content = "html/settings-ssh-keys.html";
    $scope.new_key = "";
    $scope.keys = []

    $scope.getKeys = function() {
        $http.post('/api/userGetSshKeys',
                   { 'auth': $rootScope.session.auth,
                     'user': $rootScope.session.auth.user
                   })
            .success(function (data, status, headers, config) {
                if (data.status == "kSucceed")
                    $scope.keys = data.keys;
            })
            .error(rpcGenericError);
    }
    $scope.getKeys();

    $scope.addKey = function(new_key) {
        var re = /^(ssh-rsa|ssh-dsa) ([A-Za-z0-9+\/=]+) (.*)?$/g;
        var matches = re.exec(new_key);

        if (matches[1] == "ssh-rsa")
            var type = "kSshKeyRsa";
        else if (matches[1] == "ssh-rsa")
            var type = "kSshKeyDsa";
        else {
            alert("Invalid type: " + matches[1]);
            return;
        }

        $http.post('/api/userAddSshKey',
                   { 'auth': $rootScope.session.auth,
                     'key': {
                         'type': type,
                         'key': matches[2],
                         'desc': matches[3],
                     }
                   })
            .success(function (data, status, headers, config) {
                $scope.getKeys();
                $scope.errmsg = data.msg;
            })
            .error(rpcGenericError);
    }

    $scope.removeKey = function(key) {
        $http.post('/api/userRemoveSshKey',
                   { 'auth': $rootScope.session.auth,
                     'key': key
                   })
            .success(function (data, status, headers, config) {
                $scope.getKeys();
                $scope.errmsg = data.msg;
            })
            .error(rpcGenericError);
    }
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
    $scope.reset();

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
    $scope.group = {"grp":""};
    $scope.new_user = {"user":"","role":"kReader"};
    $scope.is_admin = false;

    $scope.refresh = function() {
        $http.post('/api/groupGetInfo', {"grp_id":parseInt($routeParams.grp_id)})
            .success(function (data, status, headers, config) {
                if (data.status == "kSucceed")
                    $scope.group = data;
            })
            .error(rpcGenericError);

        $http.post('/api/groupListMembers', {"grp_id":parseInt($routeParams.grp_id)})
            .success(function (data, status, headers, config) {
                if (data.status == "kSucceed") {
                    $scope.users = data.users;
                    var i;
                    for (i in data.users) {
                        if (data.users[i].user == $rootScope.session.auth.user)
                            $scope.is_admin = (data.users[i].role == "kOwner");
                    }
                }
            })
            .error(rpcGenericError);
    }

    $scope.addUser = function(new_user) {
        $http.post('/api/groupAddUser',
                   { 'auth':$rootScope.session.auth,
                     'grp':$scope.group.grp,
                     'user':new_user.user,
                     'role':new_user.role })
            .success(function (data, status, headers, config) {
                new_user.errmsg = null;
                if (data.status == "kSucceed") {
                    $scope.refresh();
                    return;
                }
                new_user.errmsg = data.msg;
            })
            .error(rpcGenericError);
    }

    $scope.userUpdate = function(user, role) {
    }

    $scope.removeUser = function(user) {
        $http.post('/api/groupRemoveUser',
                   { 'auth':$rootScope.session.auth,
                     'grp':$scope.group.grp,
                     'user':user.user })
            .success(function (data, status, headers, config) {
                $scope.new_user.errmsg = null;
                if (data.status == "kSucceed") {
                    $scope.refresh();
                    return;
                }
                new_user.errmsg = data.msg;
            })
            .error(rpcGenericError);
    }

    $scope.refresh();
}
