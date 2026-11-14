#include "samesies/directory_scanner.hpp"
#include "samesies/duplicate_finder.hpp"

#include <criterion/criterion.h>

#include <cstddef>
#include <unistd.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

#define CHUNK_SIZE 4096
#define MULTI_CHUNK_COUNT 3
#define LARGE_SIZE (CHUNK_SIZE * MULTI_CHUNK_COUNT)
#define LARGE_SIZE_PLUS_ONE (LARGE_SIZE + 1)

#define CONTENT_ALPHA "alpha content"
#define CONTENT_BETA "beta content"
#define CONTENT_GAMMA "gamma content"
#define CONTENT_DELTA "delta content"
#define DIR_PREFIX "samesies_test_"

static fs::path root_dir;

static void setup_dir(void) {
  root_dir = fs::temp_directory_path() / (DIR_PREFIX + to_string(getpid()));
  fs::remove_all(root_dir);
  fs::create_directories(root_dir);
}

static void teardown_dir(void) { fs::remove_all(root_dir); }

static fs::path write_file(const fs::path &relative, const string &content) {
  fs::path full_path = root_dir / relative;
  fs::create_directories(full_path.parent_path());
  ofstream out(full_path, ios::binary);
  out.write(content.data(), static_cast<streamsize>(content.size()));
  return full_path;
}

static list_of_duplicate_files_t normalize(list_of_duplicate_files_t groups) {
  for (auto &group : groups) {
    sort(group.begin(), group.end());
  }
  sort(groups.begin(), groups.end());
  return groups;
}

static list_of_duplicate_files_t find_duplicates(void) {
  DirectoryScanner scanner(root_dir);
  return normalize(get_duplicates(scanner.get_groups()));
}

TestSuite(duplicate_finder, .init = setup_dir, .fini = teardown_dir);

Test(duplicate_finder, empty_directory_has_no_duplicates) {
  cr_assert(find_duplicates().empty());
}

Test(duplicate_finder, single_file_has_no_duplicates) {
  write_file("only.txt", CONTENT_ALPHA);

  cr_assert(find_duplicates().empty());
}

Test(duplicate_finder, two_identical_files_are_duplicates) {
  fs::path first = write_file("first.txt", CONTENT_ALPHA);
  fs::path second = write_file("second.txt", CONTENT_ALPHA);

  list_of_duplicate_files_t expected = normalize({{first, second}});

  cr_assert(find_duplicates() == expected);
}

Test(duplicate_finder, same_size_different_content_is_not_duplicate) {
  write_file("first.txt", CONTENT_ALPHA);
  write_file("second.txt", CONTENT_BETA);

  cr_assert(find_duplicates().empty());
}

Test(duplicate_finder, different_sizes_are_not_duplicates) {
  write_file("short.txt", CONTENT_ALPHA);
  write_file("long.txt", CONTENT_ALPHA CONTENT_ALPHA);

  cr_assert(find_duplicates().empty());
}

Test(duplicate_finder, unique_file_is_excluded_from_duplicate_group) {
  fs::path first = write_file("first.txt", CONTENT_ALPHA);
  fs::path second = write_file("second.txt", CONTENT_ALPHA);
  write_file("third.txt", CONTENT_BETA);

  list_of_duplicate_files_t expected = normalize({{first, second}});

  cr_assert(find_duplicates() == expected);
}

Test(duplicate_finder, three_identical_files_form_one_group) {
  fs::path first = write_file("first.txt", CONTENT_ALPHA);
  fs::path second = write_file("second.txt", CONTENT_ALPHA);
  fs::path third = write_file("third.txt", CONTENT_ALPHA);

  list_of_duplicate_files_t expected = normalize({{first, second, third}});

  cr_assert(find_duplicates() == expected);
}

Test(duplicate_finder, multiple_distinct_duplicate_groups_are_separated) {
  fs::path alpha_one = write_file("alpha_one.txt", CONTENT_ALPHA);
  fs::path alpha_two = write_file("alpha_two.txt", CONTENT_ALPHA);
  fs::path beta_one = write_file("beta_one.txt", CONTENT_BETA);
  fs::path beta_two = write_file("beta_two.txt", CONTENT_BETA);

  list_of_duplicate_files_t expected =
      normalize({{alpha_one, alpha_two}, {beta_one, beta_two}});

  cr_assert(find_duplicates() == expected);
}

Test(duplicate_finder, same_size_groups_with_mixed_content) {
  fs::path alpha_one = write_file("alpha_one.txt", CONTENT_ALPHA);
  fs::path alpha_two = write_file("alpha_two.txt", CONTENT_ALPHA);
  write_file("beta.txt", CONTENT_BETA);
  write_file("delta.txt", CONTENT_DELTA);
  write_file("gamma.txt", CONTENT_GAMMA);

  list_of_duplicate_files_t expected = normalize({{alpha_one, alpha_two}});

  cr_assert(find_duplicates() == expected);
}

Test(duplicate_finder, duplicates_in_subdirectories_are_found) {
  fs::path top = write_file("top.txt", CONTENT_ALPHA);
  fs::path nested = write_file("sub/deep/nested.txt", CONTENT_ALPHA);

  list_of_duplicate_files_t expected = normalize({{top, nested}});

  cr_assert(find_duplicates() == expected);
}

Test(duplicate_finder, empty_files_are_duplicates_of_each_other) {
  fs::path first = write_file("first.txt", "");
  fs::path second = write_file("second.txt", "");

  list_of_duplicate_files_t expected = normalize({{first, second}});

  cr_assert(find_duplicates() == expected);
}

Test(duplicate_finder, identical_files_smaller_than_one_chunk) {
  string content(CHUNK_SIZE - 1, 'x');
  fs::path first = write_file("first.bin", content);
  fs::path second = write_file("second.bin", content);

  list_of_duplicate_files_t expected = normalize({{first, second}});

  cr_assert(find_duplicates() == expected);
}

Test(duplicate_finder, identical_files_exactly_one_chunk) {
  string content(CHUNK_SIZE, 'x');
  fs::path first = write_file("first.bin", content);
  fs::path second = write_file("second.bin", content);

  list_of_duplicate_files_t expected = normalize({{first, second}});

  cr_assert(find_duplicates() == expected);
}

Test(duplicate_finder, identical_files_spanning_multiple_chunks) {
  string content(static_cast<size_t>(LARGE_SIZE), 'x');
  fs::path first = write_file("first.bin", content);
  fs::path second = write_file("second.bin", content);

  list_of_duplicate_files_t expected = normalize({{first, second}});

  cr_assert(find_duplicates() == expected);
}

Test(duplicate_finder, identical_files_with_partial_last_chunk) {
  string content(LARGE_SIZE_PLUS_ONE, 'x');
  fs::path first = write_file("first.bin", content);
  fs::path second = write_file("second.bin", content);

  list_of_duplicate_files_t expected = normalize({{first, second}});

  cr_assert(find_duplicates() == expected);
}

Test(duplicate_finder, large_files_differing_in_first_byte) {
  string base(static_cast<size_t>(LARGE_SIZE), 'x');
  string modified = base;
  modified.front() = 'y';
  write_file("first.bin", base);
  write_file("second.bin", modified);

  cr_assert(find_duplicates().empty());
}

Test(duplicate_finder, large_files_differing_in_last_byte) {
  string base(static_cast<size_t>(LARGE_SIZE), 'x');
  string modified = base;
  modified.back() = 'y';
  write_file("first.bin", base);
  write_file("second.bin", modified);

  cr_assert(find_duplicates().empty());
}

Test(duplicate_finder, large_files_differing_in_middle_chunk) {
  string base(static_cast<size_t>(LARGE_SIZE), 'x');
  string modified = base;
  modified[CHUNK_SIZE + (CHUNK_SIZE / 2)] = 'y';
  write_file("first.bin", base);
  write_file("second.bin", modified);

  cr_assert(find_duplicates().empty());
}

Test(duplicate_finder, pair_among_differing_large_files) {
  string base(static_cast<size_t>(LARGE_SIZE), 'x');
  string modified = base;
  modified.back() = 'y';
  fs::path first = write_file("first.bin", base);
  fs::path second = write_file("second.bin", base);
  write_file("third.bin", modified);

  list_of_duplicate_files_t expected = normalize({{first, second}});

  cr_assert(find_duplicates() == expected);
}

Test(duplicate_finder, different_file_names_do_not_matter) {
  fs::path first = write_file("report.txt", CONTENT_ALPHA);
  fs::path second = write_file("image.png", CONTENT_ALPHA);

  list_of_duplicate_files_t expected = normalize({{first, second}});

  cr_assert(find_duplicates() == expected);
}

Test(duplicate_finder, get_duplicates_on_empty_groups_returns_empty) {
  file_size_group_t groups;

  cr_assert(get_duplicates(groups).empty());
}

Test(duplicate_finder, get_duplicates_skips_single_path_groups) {
  fs::path only = write_file("only.txt", CONTENT_ALPHA);
  file_size_group_t groups;
  groups[sizeof(CONTENT_ALPHA) - 1] = {only};

  cr_assert(get_duplicates(groups).empty());
}
