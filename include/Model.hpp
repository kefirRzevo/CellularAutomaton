#pragma once

#include <algorithm>
#include <bitset>
#include <deque>
#include <iostream>
#include <random>
#include <string>

namespace automaton {

inline void dump(const std::deque<bool> &data, std::ostream &os) {
  std::string out(data.size(), '0');
  std::transform(data.begin(), data.end(), std::back_inserter(out),
                 [](auto &&it) { return it ? '1' : '0'; });
  os << out << std::endl;
}

class Rule {
public:
  using value_type = unsigned char;

  Rule(value_type val) : val_(val) {}

  bool value(std::bitset<3> neighbors) const {
    return val_ & (1 << neighbors.to_ulong());
  }

private:
  value_type val_;
};

class BoundCond {
  std::deque<bool> data_;

public:
  BoundCond() = default;

  BoundCond(std::string_view string) {
    std::transform(string.begin(), string.end(), std::back_inserter(data_),
                   [](auto &&it) {
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
  }

  void randomize(size_t size) {
    std::srand(0);
    data_.clear();
    data_.insert(data_.begin(), std::rand() % size, true);
    data_.resize(size);
    std::shuffle(data_.begin(), data_.end(), std::mt19937{});
  }

  void dump(std::ostream &os) const { automaton::dump(data_, os); }

  auto size() const { return data_.size(); }
  auto begin() const { return data_.begin(); }
  auto end() const { return data_.end(); }
};

class RowBuilder final {
  std::string data_;

public:
  RowBuilder &addEther() {
    data_.append("11111000100110");
    return *this;
  }

  RowBuilder &addA() {
    data_.append("100110");
    return *this;
  }

  RowBuilder &addA2() {
    data_.append("111000100110");
    return *this;
  }

  BoundCond create() {
    BoundCond result{data_};
    data_.clear();
    return result;
  }
};

class Polygon final {
public:
  using size_type = size_t;
  using Row = std::deque<bool>;
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
    for (; curRow != rows_.end(); ++curRow) {
      curRow->resize(width());
      std::bitset<3> neighbors;
      for (size_type i = 0; i != width(); ++i) {
        neighbors[0] = prevRow->at((i + 1) % width());
        neighbors[1] = prevRow->at(i);
        neighbors[2] = prevRow->at((i - 1 + width()) % width());
        curRow->at(i) = rule.value(neighbors);
      }
      prevRow = curRow;
    }
  }

  void dump(std::ostream &os) const {
    for (const auto &row : rows_)
      automaton::dump(row, os);
  }
};

class Model {
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
