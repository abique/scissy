#include <re2/re2.h>

#include <mimosa/archive/writer.hh>
#include <mimosa/http/error-handler.hh>
#include <mimosa/git/commit.hh>
#include <mimosa/git/tree.hh>
#include <mimosa/git/archive.hh>
#include <mimosa/git/repository.hh>

#include "archive-handler.hh"
#include "repositories.hh"
#include "session.hh"

namespace scissy
{
  bool
  ArchiveHandler::handle(mimosa::http::RequestReader &  request,
                         mimosa::http::ResponseWriter & response) const
  {
    static const RE2 match(
      "^/repo/archive/(\\d+)/([^/]+)/"
      "([^/]+)\\.(tar\\.gz|tar\\.bz2|tar\\.xz|tar\\.lzma|"
      "cpio\\.gz|cpio\\.bz2|cpio\\.xz|cpio\\.lzma|"
      "zip|iso)");

    int64_t     repo_id;
    std::string revision;
    std::string dir;
    std::string format;

    // Parse location
    if (!RE2::FullMatch(request.location(), match, &repo_id, &revision, &dir, &format))
      return mimosa::http::ErrorHandler::basicResponse(
        request, response, mimosa::http::kStatusBadRequest);

    bool is_public;
    if (!Repositories::instance().isPublic(repo_id, &is_public))
      return mimosa::http::ErrorHandler::basicResponse(
        request, response, mimosa::http::kStatusNotFound);

    // private repository
    if (!is_public) {
      Session session(request, response);

      if (session.userId() < 1)
        return mimosa::http::ErrorHandler::basicResponse(
          request, response, mimosa::http::kStatusForbidden);

      pb::Role role;
      if (!Repositories::instance().getUserRole(repo_id, session.userId(), &role))
        return mimosa::http::ErrorHandler::basicResponse(
          request, response, mimosa::http::kStatusForbidden);

      if (role < pb::kReader)
        return mimosa::http::ErrorHandler::basicResponse(
          request, response, mimosa::http::kStatusForbidden);
    }

    mimosa::git::Repository repo(Repositories::instance().getRepoPath(repo_id));
    if (!repo)
      return mimosa::http::ErrorHandler::basicResponse(
        request, response, mimosa::http::kStatusNotFound);

    git_oid oid;
    if (git_reference_name_to_id(&oid, repo, revision.c_str()) &&
        git_oid_fromstrp(&oid, revision.c_str()))
      return mimosa::http::ErrorHandler::basicResponse(
        request, response, mimosa::http::kStatusNotFound);

    mimosa::git::Commit commit(repo, &oid);
    if (!commit)
      return mimosa::http::ErrorHandler::basicResponse(
        request, response, mimosa::http::kStatusNotFound);

    mimosa::git::Tree tree(repo, git_commit_tree_id(commit));
    if (!tree)
      return mimosa::http::ErrorHandler::basicResponse(
        request, response, mimosa::http::kStatusNotFound);

    mimosa::archive::Writer archive;
    if (format == "tar.xz") {
      response.setContentType("application/x-gtar");
      archive.filterXz();
      archive.formatGnutar();
    } else if (format == "tar.gz") {
      response.setContentType("application/x-gtar");
      archive.filterGzip();
      archive.formatGnutar();
    } else if (format == "tar.bz2") {
      response.setContentType("application/x-gtar");
      archive.filterBzip2();
      archive.formatGnutar();
    } else if (format == "tar.lzma") {
      response.setContentType("application/x-gtar");
      archive.filterLzma();
      archive.formatGnutar();
    } else if (format == "cpio.xz") {
      response.setContentType("application/x-cpio");
      archive.filterXz();
      archive.formatCpio();
    } else if (format == "cpio.gz") {
      response.setContentType("application/x-cpio");
      archive.filterGzip();
      archive.formatCpio();
    } else if (format == "cpio.bz2") {
      response.setContentType("application/x-cpio");
      archive.filterBzip2();
      archive.formatCpio();
    } else if (format == "cpio.lzma") {
      response.setContentType("application/x-cpio");
      archive.filterLzma();
      archive.formatCpio();
    } else if (format == "zip") {
      response.setContentType("application/zip");
      archive.formatZip();
      archive_write_zip_set_compression_deflate(archive);
    } else if (format == "iso") {
      response.setContentType("application/x-iso9660-image");
      archive.formatIso9660();
    }

    archive.open(&response);

    response.sendHeader();

    gitArchive(tree, archive, dir + "/");
    archive.close();

    return true;
  }
}
