#ifndef SCISSY_GIT_BLOB_HH
# define SCISSY_GIT_BLOB_HH

namespace scissy
{
  class GitBlob
  {
  public:
    inline GitBlob(git_blob * blob = nullptr) : blob_(blob) {}
    inline ~GitBlob() { git_blob_free(blob_); }

    inline operator git_blob *() { return blob_; }
    inline git_blob ** ref() { return &blob_; }

  private:
    git_blob * blob_;
  };
}

#endif /* !SCISSY_GIT_BLOB_HH */
