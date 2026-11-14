#pragma once

#include <filesystem>

using namespace std;
namespace fs = filesystem;

/**
 * @brief Command-line options for the duplicate finder.
 */
struct cli_args_t {
  fs::path dir_path;
  bool delete_flag = false;
};

/**
 * @brief Parses command-line arguments into a cli_args_t.
 *
 * Recognized arguments are a directory path (any argument not starting with
 * '-'), -d to enable deletion, and -h or --help to print usage. If several
 * directory paths are given, the last one wins. When help is requested, the
 * usage message is printed and the process exits with status 0.
 *
 * @param[in] argc Number of entries in argv.
 * @param[in] argv Argument vector as received by main(); argv[0] is the
 *            program name.
 * @return Parsed arguments, with dir_path validated as an existing directory.
 * @throws std::invalid_argument If an unknown option is given or the
 *         directory does not exist or is not a directory.
 */
cli_args_t parse_args(int argc, char *argv[]);

/**
 * @brief Scans a directory for duplicate files and optionally deletes them.
 *
 * Prints progress to standard output: scans args.dir_path, groups files by
 * size, finds duplicates, and reports the counts. If duplicates exist and
 * args.delete_flag is set, the user is prompted to delete one file per group
 * via delete_duplicates(); otherwise nothing is deleted.
 *
 * All exceptions are caught internally and reported to standard error, so
 * this function does not throw.
 *
 * @param[in] args Parsed command-line options, as produced by parse_args().
 */
void run(const cli_args_t &args);
