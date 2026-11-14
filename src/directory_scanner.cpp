#include "samesies/directory_scanner.hpp"

#include <system_error>

DirectoryScanner::DirectoryScanner(const fs::path &directory_path)
    : _directory_path(directory_path) {
  if (!fs::is_directory(directory_path)) {
    throw fs::filesystem_error(
        "Such directory doesn't exist dumbo!", directory_path,
        make_error_code(errc::no_such_file_or_directory));
  }
}

file_size_group_t DirectoryScanner::get_groups(void) const {
  file_size_group_t res;

  for (const fs::directory_entry &entry :
       fs::recursive_directory_iterator(this->_directory_path)) {

    if (entry.is_regular_file()) {
      const fs::path &file_path = entry.path();
      res[fs::file_size(file_path)].push_back(file_path);
    }
  }

  return res;
}
