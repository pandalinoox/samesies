#include "samesies/delete_handler.hpp"

#include <criterion/criterion.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;
namespace fs = filesystem;

Test(delete_handler, deletes_selected_duplicate) {
  fs::path temp_dir = fs::temp_directory_path() / "samesies_test";
  fs::create_directories(temp_dir);

  fs::path file1 = temp_dir / "file1.txt";
  fs::path file2 = temp_dir / "file2.txt";
  fs::path file3 = temp_dir / "file3.txt";

  ofstream(file1) << "duplicate";
  ofstream(file2) << "duplicate";
  ofstream(file3) << "duplicate";

  list_of_duplicate_files_t dup_files = {{file1, file2, file3}};

  istringstream input("1\n");
  ostringstream output;

  auto *old_cin = cin.rdbuf(input.rdbuf());
  auto *old_cout = cout.rdbuf(output.rdbuf());

  delete_duplicates(dup_files);

  cin.rdbuf(old_cin);
  cout.rdbuf(old_cout);

  cr_assert(fs::exists(file1));
  cr_assert(!fs::exists(file2));
  cr_assert(fs::exists(file3));

  fs::remove_all(temp_dir);
}

Test(delete_handler, deletes_file_at_index_zero) {
  fs::path temp_dir = fs::temp_directory_path() / "samesies_test_zero";
  fs::create_directories(temp_dir);

  fs::path file1 = temp_dir / "file1.txt";
  fs::path file2 = temp_dir / "file2.txt";

  ofstream(file1) << "duplicate";
  ofstream(file2) << "duplicate";

  list_of_duplicate_files_t dup_files = {{file1, file2}};

  istringstream input("0\n");
  ostringstream output;

  auto *old_cin = cin.rdbuf(input.rdbuf());
  auto *old_cout = cout.rdbuf(output.rdbuf());

  delete_duplicates(dup_files);

  cin.rdbuf(old_cin);
  cout.rdbuf(old_cout);

  cr_assert(!fs::exists(file1));
  cr_assert(fs::exists(file2));

  fs::remove_all(temp_dir);
}

Test(delete_handler, handles_invalid_input) {
  fs::path temp_dir = fs::temp_directory_path() / "samesies_test_invalid";
  fs::create_directories(temp_dir);

  fs::path file1 = temp_dir / "file1.txt";
  fs::path file2 = temp_dir / "file2.txt";

  ofstream(file1) << "duplicate";
  ofstream(file2) << "duplicate";

  list_of_duplicate_files_t dup_files = {{file1, file2}};

  istringstream input("5\nabc\n1\n");
  ostringstream output;

  auto *old_cin = cin.rdbuf(input.rdbuf());
  auto *old_cout = cout.rdbuf(output.rdbuf());

  delete_duplicates(dup_files);

  cin.rdbuf(old_cin);
  cout.rdbuf(old_cout);

  cr_assert(fs::exists(file1));
  cr_assert(!fs::exists(file2));

  cr_assert(strstr(output.str().c_str(), "Invalid input") != nullptr);

  fs::remove_all(temp_dir);
}
