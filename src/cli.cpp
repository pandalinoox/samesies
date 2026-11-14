#include "samesies/cli.hpp"
#include "samesies/delete_handler.hpp"
#include "samesies/directory_scanner.hpp"
#include "samesies/duplicate_finder.hpp"

#include <exception>
#include <iostream>
#include <string>

static void print_help(const char *program_name) {
  std::cout << "Usage: " << program_name << "[DIRECTORY] [OPTIONS]\n\n"
            << "Find duplicate files in a directory.\n\n"
            << "Options:\n"
            << "  -d              Delete duplicate files\n"
            << "  -h, --help      Display this help message\n\n"
            << "Examples:\n"
            << "  " << program_name << "\n"
            << "  " << program_name << " ./files\n"
            << "  " << program_name << " ./files -d\n"
            << "  " << program_name << " -d\n";
}

cli_args_t parse_args(int argc, char *argv[]) {
  cli_args_t args;

  args.dir_path = fs::current_path();

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];

    if (arg == "-h" || arg == "--help") {
      print_help(argv[0]);
      std::exit(0);
    }
    if (arg == "-d") {
      args.delete_flag = true;
    } else if (!arg.empty() && arg[0] != '-') {
      args.dir_path = arg;
    } else {
      throw std::invalid_argument("Unknown argument: " + arg);
    }
  }

  if (!fs::exists(args.dir_path) || !fs::is_directory(args.dir_path)) {
    throw std::invalid_argument("Invalid directory: " + args.dir_path.string());
  }

  return args;
}

void run(const cli_args_t &args) {
  try {
    std::cout << "Scanning directory: " << args.dir_path << '\n';

    DirectoryScanner dir_scanner(args.dir_path);
    file_size_group_t groups = dir_scanner.get_groups();

    std::cout << "Found " << groups.size() << " file size group(s).\n";
    std::cout << "Finding duplicate files...\n";

    list_of_duplicate_files_t dup_files = get_duplicates(groups);

    std::cout << "Found " << dup_files.size() << " duplicate group(s).\n";

    if (dup_files.empty()) {
      std::cout << "No duplicate files found.\n";
      return;
    }

    if (args.delete_flag) {
      std::cout << "Deleting duplicates...\n";
      delete_duplicates(dup_files);
      std::cout << "Duplicates deleted successfully.\n";
    } else {
      std::cout << "Deletion skipped. No files were deleted.\n";
    }

  } catch (const fs::filesystem_error &e) {
    std::cerr << "Filesystem error: " << e.what() << '\n';
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << '\n';
  } catch (...) {
    std::cerr << "An unexpected error occurred.\n";
  }
}
