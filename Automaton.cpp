#include "include/Manager.hpp"

int main(int argc, const char* argv[]) {
  try {
    automaton::Manager manager;
    manager.run(argc, argv);
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
