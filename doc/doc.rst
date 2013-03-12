========
Scissy
========

.. contents::

--------
Features
--------

Goals
=====

- provide a git repository hosting service
- provide CRUD APIs to:

  - create/update/delete a user account
  - create/update/delete a group
  - create/update/delete a repository
  - browse a repository:

    - history
    - branches/tags
    - tree
    - download a tarball of a revision

  - manage a repository's ACL
  - list repositories
  - list users
  - list groups

- have a web interface which uses CRUD APIs to expo
- have a very simple setup

  - no apache, nginx or any external web server to configure
  - no mysql, postgresql, or any database to configure
  - no complicated configuration options, you can enjoy the default settings
  - you should be able to setup your server from scratch in a few minutes

- security

  - code is written with robustness and security in mind

Non goals
=========

- have a distributed, scalable, complex architecture

  - scissy is not designed to host thousand of repositories, every companies
    should be able to host its own scissy server, and I think that one server
    should be enough to host a company's repositories. And if not, they could
    be able to regroup projects on separated machines. Also, vertical scaling is
    an option, as scissy takes advantage of hard drive and multi-cores.

- be social like github: even if it's cool, github does it very well.

------
Design
------

Components
==========

- scissy-init, initialize a new scissy-environment
- scissy, web server, acl server (used by scissy-shell)
- scissy-shell, ensure ACL between ssh and git-shell

.. image:: design.png

File system layout
==================

- ~/db/scissy.sqlite   stores the database
- ~/.ssh/authorized_keys stores the ssh authorized_keys
- ~/repos/${repo_id}/    stores the bare git repository
- ~/conf                 stores the config files
- ~/conf/key.pem         the web server key (certtool --generate-privkey --outfile key.pem)
- ~/conf/cert.pem        the web server certificate (certtool -s --load-privkey --outfile cert.pem)

Database model
==============



--------
CRUD API
--------

Conventions
===========

XXX

User
====

Register a new user
-------------------

``POST /api/user/:user``

Update user's details
---------------------

``PUT /api/user/:user``

Delete a user
-------------

``DELETE /api/user/:user``

Group
=====

List groups
-----------

``GET /api/groups/``

Create a new group
------------------

``POST /api/group/:group``

Edit a group
------------

``PUT /api/group/:group``

Delete a group
--------------

``DELETE /api/group/:group``

List the users of a group
-------------------------

``GET /api/group/:group/users/``

Add user to a group
-------------------

``POST /api/group/:group/user/:user``

Edit user's role in a group
---------------------------

``PUT /api/group/:group/user/:user``

Remove a user from a group
--------------------------

``DELETE /api/group/:group/user/:user``

Repository
==========

List repositories
-----------------

``GET /api/repos/``

Create a repository
-------------------

``POST /api/repo/:repo``

Add user to a repository
------------------------

``POST /api/repo/:repo/users``

Update user's role
------------------

``PUT /api/repo/:repo/user/:user``

Remove a user from a repository
-------------------------------

``DELETE /api/repo/:repo/user/:user``

List commits
------------

``GET /api/repo/:repo/log/:head``

Get a commit
------------

``GET /api/repo/:repo/commit/:ref``

Browse the tree
---------------

``GET /api/repo/:repo/tree/:path``

List tags
---------

``GET /api/repo/:repo/tags``

Download
--------

``GET /api/repo/:repo/archive/:rev?format=``
