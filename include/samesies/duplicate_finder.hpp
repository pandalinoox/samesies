#pragma once

#include "samesies/directory_scanner.hpp"

#include <filesystem>
#include <vector>

using namespace std;
namespace fs = filesystem;

/**
 * @brief List of duplicate groups, where each group holds the paths of files
 *        with identical content.
 */
using list_of_duplicate_files_t = vector<vector<fs::path>>;

/**
 * @brief Finds files with identical content within each size group.
 *
 * Only files that share a size can be duplicates, so each group from
 * @p groups is processed independently, and groups with fewer than two files
 * are skipped. Within a group, files are read in 4 KiB chunks and hashed
 * incrementally with FNV-1a. After every chunk, files whose running hash is
 * unique are dropped, so differing files stop being read early. Files that
 * survive to the end of the data are reported as duplicates.
 *
 * Equality is decided by a 64-bit non-cryptographic hash, so a hash
 * collision could in rare cases report two different files as duplicates.
 * Empty files with the same size (zero) are all reported as duplicates of
 * each other.
 *
 * @param[in] groups Files grouped by size, as produced by
 *            DirectoryScanner::get_groups().
 * @return List of duplicate groups, each containing at least two paths. The
 *         order of groups and of paths within a group is unspecified. Empty
 *         if no duplicates exist.
 */
list_of_duplicate_files_t get_duplicates(const file_size_group_t &groups);
