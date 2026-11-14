#pragma once

#include "samesies/duplicate_finder.hpp"

/**
 * @brief Interactively prompts the user to delete one file from each
 *        duplicate group.
 *
 * For every non-empty group in @p dup_files, the paths are printed with
 * numeric indices and the user is asked to choose one to delete. Invalid
 * input (non-numeric or out of range) re-prompts until a valid index is
 * entered. Exactly one file is removed per group, so the remaining files in
 * the group are kept. Empty groups are skipped.
 *
 * Reads from standard input and writes to standard output. If standard input
 * reaches end-of-file, the prompt loop will not terminate.
 *
 * @param[in] dup_files Groups of duplicate files, as produced by
 *            get_duplicates().
 * @throws fs::filesystem_error If a chosen file cannot be removed.
 */
void delete_duplicates(const list_of_duplicate_files_t &dup_files);
