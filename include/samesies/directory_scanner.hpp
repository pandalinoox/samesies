#pragma once

#include <cstdint>
#include <filesystem>
#include <unordered_map>
#include <vector>

using namespace std;
namespace fs = filesystem;

/**
 * @brief Maps a file size in bytes to all file paths having exactly that size.
 */
using file_size_group_t = unordered_map<uintmax_t, vector<fs::path>>;

/**
 * @brief Recursively scans a directory and groups its regular files by size.
 *
 * Grouping by size is a cheap first pass for duplicate detection: files with
 * different sizes can never be identical, so only files sharing a size need
 * further comparison.
 */
class DirectoryScanner {

private:
  /** @brief Root directory that is scanned. */
  fs::path _directory_path;

public:
  /**
   * @brief Constructs a scanner for the given directory.
   *
   * @param[in] directory_path Path to the directory to scan. Must refer to an
   *            existing directory.
   * @throws fs::filesystem_error If directory_path is not an existing
   *         directory.
   */
  explicit DirectoryScanner(const fs::path &directory_path);

  /**
   * @brief Recursively scans the directory and groups regular files by size.
   *
   * Only regular files are included; directories, sockets, and other special
   * files are skipped. Paths within each group appear in directory iteration
   * order, which is unspecified.
   *
   * @return Map from file size in bytes to the paths of all files with that
   *         size, ordered by ascending size. Empty if the directory contains
   *         no regular files.
   * @throws fs::filesystem_error If a directory cannot be traversed or a file
   *         size cannot be read.
   */
  file_size_group_t get_groups(void) const;
};
