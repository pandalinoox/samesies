#include "samesies/duplicate_finder.hpp"
#include "samesies/fnv1a_hash.hpp"

#include <fstream>
#include <unordered_map>

#define BUFFER_SIZE 4096

struct file_info_t {
  uint64_t _hash;
  ifstream _stream;
};

using path_hash_stream_map_t = unordered_map<fs::path, file_info_t>;
using duplicates_map_t = unordered_map<uint64_t, vector<fs::path>>;

static void initialize_phsm(path_hash_stream_map_t &phsm,
                            const vector<fs::path> &file_paths) {
  for (const fs::path &_path : file_paths) {
    phsm[_path] = {FNV_OFFSET_BASIS_64, _path};
  }
}

static void read_next_4kb(ifstream &file, vector<char> &buffer) {
  file.read(buffer.data(), static_cast<streamsize>(buffer.size()));
  buffer.resize(static_cast<size_t>(file.gcount()));
}

static bool update_phsm_dm(duplicates_map_t &dup_map, const fs::path &_path,
                           file_info_t &file_info) {
  vector<char> data(BUFFER_SIZE);

  read_next_4kb(file_info._stream, data);

  if (data.empty()) {
    return false;
  }

  uint64_t new_hash = fnv1a_hash(file_info._hash, data);

  file_info._hash = new_hash;
  dup_map[new_hash].push_back(_path);

  return true;
}

static void prune_paths(path_hash_stream_map_t &phsm,
                        duplicates_map_t &dup_map) {

  for (auto &[_hash, _paths] : dup_map) {
    if (_paths.size() < 2) {
      phsm.erase(_paths[0]);
    }
  }
}

static void process_phsm(path_hash_stream_map_t &phsm) {
  while (!phsm.empty()) {
    bool any_data_read = true;
    duplicates_map_t dup_map;

    for (auto &[_path, file_info] : phsm) {
      any_data_read = update_phsm_dm(dup_map, _path, file_info);

      if (!any_data_read) {
        break;
      }
    }

    prune_paths(phsm, dup_map);

    if (!any_data_read) {
      break;
    }
  }
}

static void copy_duplicate_files(const path_hash_stream_map_t &phsm,
                                 list_of_duplicate_files_t &res) {

  duplicates_map_t final_dup_map;
  for (const auto &[_path, file_info] : phsm) {
    final_dup_map[file_info._hash].push_back(_path);
  }

  for (auto &[_hash, _paths] : final_dup_map) {
    res.push_back(_paths);
  }
}

list_of_duplicate_files_t get_duplicates(const file_size_group_t &groups) {
  list_of_duplicate_files_t res;

  for (const auto &[file_size, file_paths] : groups) {
    if (file_paths.size() < 2) {
      continue;
    }

    path_hash_stream_map_t phsm;

    initialize_phsm(phsm, file_paths);

    process_phsm(phsm);

    copy_duplicate_files(phsm, res);
  }

  return res;
}
