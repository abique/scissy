#ifndef SCISSY_GIT_ARCHIVE_HH
# define SCISSY_GIT_ARCHIVE_HH

# include <mimosa/archive/writer.hh>

# include "git-tree.hh"

namespace scissy
{
  bool gitArchive(GitTree &                 tree,
                  mimosa::archive::Writer & archive,
                  const std::string &       prefix = "");
}

#endif /* !SCISSY_GIT_ARCHIVE_HH */
