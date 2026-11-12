#include "samesies/directory_scanner.hpp"

#include <chrono>
#include <criterion/criterion.h>
#include <cstddef>
#include <filesystem>
#include <fstream>

using namespace std;
namespace fs = filesystem;

class TestDirectory {
public:
  fs::path path;

  TestDirectory() {
    path = fs::temp_directory_path() /
           ("samesies_test_" +
            to_string(chrono::steady_clock::now().time_since_epoch().count()));
    fs::create_directories(path);
  }

  ~TestDirectory() { fs::remove_all(path); }

  size_t create_file(const fs::path &relative_path,
                     const string &content) const {
    fs::create_directories((path / relative_path).parent_path());
    ofstream file(path / relative_path);
    file << content;
    file.close();
    return content.size();
  }
};

Test(directory_scanner, accepts_valid_directory) {
  TestDirectory temp;

  DirectoryScanner scanner(temp.path);

  cr_assert(true);
}

Test(directory_scanner, rejects_non_existent_directory) {
  fs::path non_existent_path = fs::temp_directory_path() / "non_existent_path";

  cr_assert_throw(DirectoryScanner scanner(non_existent_path),
                  fs::filesystem_error);
}

Test(directory_scanner, rejects_regular_file) {
  TestDirectory temp;

  temp.create_file("file.txt", "hello");

  cr_assert_throw(DirectoryScanner scanner(temp.path / "file.txt"),
                  fs::filesystem_error);
}

Test(directory_scanner, groups_file_by_size) {
  TestDirectory temp;

  size_t file_1_size = temp.create_file("file1.txt", "hello");
  temp.create_file("file2.txt", "hello"); // same content as file_1
  size_t file_3_size = temp.create_file("file3.txt", "helloworld");

  DirectoryScanner scanner(temp.path);
  file_size_group_t groups = scanner.get_groups();

  cr_assert_eq(groups.size(), 2);

  cr_assert_eq(groups[file_1_size].size(), 2);
  cr_assert_eq(groups[file_3_size].size(), 1);
}

Test(directory_scanner, scans_nested_directories) {
  TestDirectory temp;

  size_t content_size = temp.create_file("root.txt", "hello");
  temp.create_file("nested/child.txt", "hello");
  temp.create_file("nested/deep/grandchild.txt", "hello");

  DirectoryScanner scanner(temp.path);
  file_size_group_t groups = scanner.get_groups();

  cr_assert_eq(groups.size(), 1);

  cr_assert_eq(groups[content_size].size(), 3);
}

Test(directory_scanner, handles_empty_directory) {
  TestDirectory temp;

  DirectoryScanner scanner(temp.path);
  file_size_group_t groups = scanner.get_groups();

  cr_assert(groups.empty());
}
