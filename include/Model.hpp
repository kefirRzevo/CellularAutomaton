#pragma once

#include <algorithm>
#include <bitset>
#include <deque>
#include <filesystem>
#include <iostream>
#include <random>
#include <string>
#include <unordered_map>

#include "Utils.hpp"

namespace fs = std::filesystem;

fs::path repoPath = fs::path(__FILE__).parent_path().parent_path();

namespace automaton {

class Rule final {
public:
  using value_type = unsigned char;

  Rule(value_type val) : val_(val) {}

  value_type getValue() const noexcept { return val_; }

  bool apply(std::bitset<3> neighbors) const {
    return val_ & (1 << neighbors.to_ulong());
  }

private:
  value_type val_;
};

class BoundCond final {
  BoolStorage data_;

public:
  using DataIter = BoolStorage::const_iterator;

  BoundCond() = default;

  static BoundCond createFromString(std::string_view string) {
    BoundCond boundCond;
    std::transform(string.begin(), string.end(),
                   std::back_inserter(boundCond.data_), [](auto &&it) {
                     switch (it) {
                     case '0':
                       return false;
                     case '1':
                       return true;
                     default:
                       auto msg = "Unknown symbol '" + std::string(it, 1) + "'";
                       throw std::runtime_error(msg);
                     }
                   });
    return boundCond;
  }

  static BoundCond createRandom(size_t width) {
    BoundCond boundCond;
    std::srand(0);
    auto &data = boundCond.data_;
    data.clear();
    data.insert(data.begin(), std::rand() % width, true);
    data.resize(width);
    data.shrink_to_fit();
    std::shuffle(data.begin(), data.end(), std::mt19937{});
    return boundCond;
  }

  void dump(std::ostream &os) const {
    os << automaton::boolStorageToString(data_) << std::endl;
  }

  size_t size() const { return data_.size(); }
  DataIter begin() const { return data_.begin(); }
  DataIter end() const { return data_.end(); }
};

class BoundCondBuilder final {
  std::string data_;

  using GlidersCollection = std::unordered_map<std::string, std::string>;

  GlidersCollection gliders_;

  void initialize() {
    for (auto &&gliderIter : fs::directory_iterator{repoPath / "resources"}) {
      auto gliderPath = gliderIter.path();
      auto gliderName = gliderPath.filename().generic_u8string();
      auto gliderString = automaton::readAndJoin(gliderPath.generic_u8string());
      gliders_.emplace(std::move(gliderName), std::move(gliderString));
    }
  }

  void dumpGlidersCollection(std::ostream &os) const {
    os << gliders_.size() << " gliders" << std::endl;
    for (auto &&[name, value] : gliders_) {
      os << name << std::endl;
      os << value << std::endl;
    }
  }

  struct ParsedGlider {
    size_t repeatsCount_;
    std::string name_;

    ParsedGlider(size_t repeatCount, std::string &&name)
        : repeatsCount_(repeatCount), name_(std::move(name)) {}
  };

  using ParsedGliders = std::vector<ParsedGlider>;

  ParsedGliders parseGliders(std::string_view buildGlidersPath) {
    auto string = automaton::readAndJoin(buildGlidersPath);
    std::stringstream ss{string};
    std::string gliderName;
    auto isPosNumber = [](std::string_view string) -> bool {
      return std::all_of(string.begin(), string.end(),
                         [](auto &&it) { return std::isdigit(it); });
    };

    ParsedGliders parsedGliders;
    std::string word;
    while (ss >> word) {
      int repeatsCount = 1;
      if (isPosNumber(word)) {
        repeatsCount = std::atoi(word.c_str());
        ss >> word;
      }
      parsedGliders.emplace_back(repeatsCount, std::move(word));
    }
    return parsedGliders;
  }

public:
  BoundCondBuilder() { initialize(); }

  BoundCond build(std::string_view buildGlidersPath) {
    std::string resString;
    auto parsedGliders = parseGliders(buildGlidersPath);
    if (parsedGliders.empty()) {
      auto msg = "No gliders in '" + std::string(buildGlidersPath) + "'";
      throw std::runtime_error(msg);
    }
    for (auto &&glider : parsedGliders) {
      auto found = gliders_.find(std::string(glider.name_));
      if (found == gliders_.end()) {
        auto msg = "Glider '" + std::string(glider.name_) + "' not found";
        throw std::runtime_error(msg);
      }
      for (size_t i = 0; i != glider.repeatsCount_; ++i)
        resString.append(found->second.c_str());
    }
    return BoundCond::createFromString(resString);
  }
};

class Polygon final {
public:
  using size_type = size_t;
  using Row = BoolStorage;
  using RowStorage = std::vector<Row>;
  using RowIter = RowStorage::const_iterator;

private:
  RowStorage rows_;

public:
  template <class InputIt>
  Polygon(size_type height, InputIt first, InputIt last) : rows_(height) {
    rows_.front() = Row{first, last};
  }

  size_type width() const { return rows_.front().size(); }
  size_type height() const { return rows_.size(); }

  const Row &operator[](size_type indx) const { return rows_[indx]; }

  RowIter begin() const { return rows_.begin(); }
  RowIter end() const { return rows_.end(); }

  void fill(const Rule &rule) {
    auto prevRow = rows_.begin();
    auto curRow = std::next(rows_.begin());
    std::bitset<3> neighbors;
    for (; curRow != rows_.end(); ++curRow) {
      curRow->resize(width());
      for (size_type i = 0; i != width(); ++i) {
        neighbors[0] = prevRow->at((i + 1) % width());
        neighbors[1] = prevRow->at(i);
        neighbors[2] = prevRow->at((i - 1 + width()) % width());
        curRow->at(i) = rule.apply(neighbors);
      }
      prevRow = curRow;
    }
  }

  void dump(std::ostream &os) const {
    for (const auto &row : rows_)
      os << automaton::boolStorageToString(row) << std::endl;
  }
};

class Model final {
  Rule rule_;
  Polygon poly_;

public:
  Model(Rule &&rule, Polygon &&poly)
      : rule_(std::move(rule)), poly_(std::move(poly)) {
    poly_.fill(rule_);
  }

  const Polygon &getPolygon() const { return poly_; }
};

} // namespace automaton
