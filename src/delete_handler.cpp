#include "samesies/delete_handler.hpp"

#include <cstddef>
#include <filesystem>
#include <iostream>
#include <limits>
#include <vector>

using namespace std;
namespace fs = filesystem;

void delete_duplicates(const list_of_duplicate_files_t &dup_files) {
  size_t number_of_dupes = dup_files.size();
  size_t curr_idx = 0;

  for (const vector<fs::path> &_paths : dup_files) {
    if (_paths.empty()) {
      continue;
    }

    cout << "\nDuplicate group [" << curr_idx + 1 << "/" << number_of_dupes
         << "]:\n";

    size_t number_of_paths = _paths.size();

    for (size_t i = 0; i < number_of_paths; ++i) {
      cout << i << ". " << _paths[i] << '\n';
    }

    size_t idx;

    while (true) {
      cout << "Enter index of file to delete (0-" << number_of_paths - 1
           << "): ";

      if (cin >> idx && idx < number_of_paths) {
        break;
      }

      cout << "Invalid input. Try again.\n";

      cin.clear();
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    fs::remove(_paths[idx]);

    curr_idx++;
  }
}
