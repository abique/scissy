#ifndef SCISSY_SERVICE_HH
# define SCISSY_SERVICE_HH

# include "service.pb.h"

namespace scissy
{
  class Service : public pb::Service::Server
  {
  public:

    /////////////
    // Service //
    /////////////

    virtual bool serviceInfo(pb::Void & request,
                             pb::ServiceInfo & response) override;

    ////////////////
    // User stuff //
    ////////////////

    virtual bool userCreate(pb::UserCreate & request,
                            pb::StatusMsg &  response) override;

    virtual bool userAuth(pb::UserAuth & request,
                          pb::Session & response) override;

    virtual bool userCheckAuthToken(pb::UserAuthToken & request,
                                    pb::Session & response) override;

    virtual bool userRevokeAuthToken(pb::UserAuthToken & request,
                                     pb::StatusMsg & response) override;

    virtual bool userAddSshKey(pb::UserSshKey & request,
                               pb::StatusMsg & response) override;

    virtual bool userRemoveSshKey(pb::UserSshKey & request,
                                  pb::StatusMsg & response) override;

    virtual bool userGetSshKeys(pb::UserSshKeySelector & request,
                                pb::UserSshKeys & response) override;

    ////////////////////////////
    // Group management stuff //
    ////////////////////////////

    virtual bool groupCreate(pb::GroupCreate & request,
                             pb::GroupInfo & response) override;

    virtual bool groupDelete(pb::GroupDelete & request,
                             pb::StatusMsg & response) override;

    virtual bool groupAddUser(pb::GroupAddUser & request,
                              pb::StatusMsg & response) override;

    virtual bool groupRemoveUser(pb::GroupRemoveUser & request,
                                 pb::StatusMsg & response) override;

    ////////////////
    // Group view //
    ////////////////

    virtual bool groupsList(pb::GroupSelector & request,
                            pb::GroupList & response) override;

    virtual bool groupGetInfo(pb::GroupSelector & request,
                              pb::GroupInfo & response) override;

    virtual bool groupListMembers(pb::GroupMemberSelector & request,
                                  pb::MemberList & response) override;

    ///////////////////////////
    // Repository management //
    ///////////////////////////

    virtual bool repoCreate(pb::RepoCreate & request,
                            pb::RepoInfo & response) override;

    virtual bool repoUpdate(pb::RepoUpdate & request,
                            pb::StatusMsg & response) override;

    virtual bool repoDelete(pb::RepoDelete & request,
                            pb::StatusMsg & response) override;

    virtual bool repoAddUser(pb::RepoAddUser & request,
                             pb::StatusMsg & response) override;

    virtual bool repoRemoveUser(pb::RepoRemoveUser & request,
                                pb::StatusMsg & response) override;

    virtual bool repoAddGroup(pb::RepoAddGroup & request,
                              pb::StatusMsg & response) override;

    virtual bool repoRemoveGroup(pb::RepoRemoveGroup & request,
                                 pb::StatusMsg & response) override;

    /////////////////////
    // Repository view //
    /////////////////////

    virtual bool reposList(pb::RepoSelector & request,
                           pb::RepoList & response) override;

    virtual bool repoGetInfo(pb::RepoSelector & request,
                             pb::RepoInfo & response) override;

    virtual bool repoListMembers(pb::RepoSelector & request,
                                 pb::MemberList & response) override;

    virtual bool repoListBranches(pb::RepoSelector & request,
                                  pb::GitBranches & response) override;

    virtual bool repoGetCommit(pb::CommitSelector & request,
                               pb::GitCommit & response) override;

    virtual bool repoGetLog(pb::LogSelector & request,
                            pb::GitLog & response) override;

    virtual bool repoGetTree(pb::TreeSelector & request,
                             pb::GitTree & response) override;

    virtual bool repoGetBlob(pb::BlobSelector & request,
                             pb::GitBlob & response) override;

    ///////////////
    // Ssh shell //
    ///////////////

    virtual bool shellControl(pb::ShellControl & request,
                              pb::ShellStatus & response) override;
  };
}

#endif /* !SCISSY_SERVICE_HH */
