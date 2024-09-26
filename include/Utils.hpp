#include <algorithm>
#include <deque>
#include <fstream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace automaton {

using BoolStorage = std::deque<bool>;

inline std::string readAndJoin(std::string_view path) {
  std::ifstream file(path.data());
  if (!file) {
    auto msg = "Failed to open file: " + std::string(path);
    throw std::runtime_error(msg);
  }
  std::string result;
  std::string line;
  while (std::getline(file, line)) {
    result += line;
  }
  return result;
}

inline std::string boolStorageToString(const BoolStorage &data) {
  std::string result(data.size(), '0');
  std::transform(data.begin(), data.end(), std::back_inserter(result),
                 [](auto &&it) { return it ? '1' : '0'; });
  return result;
}

} // namespace automaton
