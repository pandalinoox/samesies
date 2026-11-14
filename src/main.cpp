#include "samesies/cli.hpp"

#include <exception>
#include <iostream>

int main(int argc, char *argv[]) {
  try {
    cli_args_t args = parse_args(argc, argv);
    run(args);
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
  }

  return 0;
}
