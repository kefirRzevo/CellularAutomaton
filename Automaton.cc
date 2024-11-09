#include "include/Manager.hpp"

auto main(int argc, const char* argv[]) -> int {
  try {
    automaton::Manager manager;
    manager.run(argc, argv);
  } catch (const std::exception& e) {
    std::cerr << "Caught exception: " << e.what() << std::endl;
  }
  return 0;
}
