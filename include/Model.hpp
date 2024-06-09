#pragma once

#include <string>
#include <random>
#include <bitset>
#include <iostream>
#include <algorithm>

#include <boost/dynamic_bitset.hpp> 

namespace automaton {

class Rule {
public:
  using value_type = unsigned char;

  Rule(value_type val) : val_(val) {}

  bool calculateValue(std::bitset<3> neighbors) const {
    return val_ & (1 << neighbors.to_ulong());
  }

private:
  value_type val_;
};

class Row {
  boost::dynamic_bitset<> data_;

public:
  Row() = default;

  Row(size_t width) {
    data_.resize(width);
    data_.shrink_to_fit();
  }

  Row(const std::string& string) : data_(string) {}

  size_t size() const { return data_.size(); }

  void set(size_t indx, bool val) { data_.set(indx, val); }
  bool get(size_t indx) const { return data_.test(indx); }

  void dump() const {
    std::string string(data_.size(), ' ');
    boost::to_string(data_, string);
    std::cout << string << std::endl;
  }

  void randomize() {
    std::srand(0);
    std::vector<bool> data(data_.size());
    size_t num_filled = std::rand() % data_.size();
    std::fill(data.begin(), data.begin() + num_filled, 1);
    std::shuffle(data.begin(), data.end(), std::mt19937{});
    for (size_t i = 0; i != data_.size(); ++i) {
      data_.set(i, data[i]);
    }
  }
};

class Polygon {
  size_t width_;
  size_t height_;

  std::vector<Row> rows_;

public:
  Polygon(size_t height, const Row& boundaryCond)
  : width_(boundaryCond.size()), height_(height),
    rows_(height) {
      std::fill(rows_.begin(), rows_.end(), Row{width_});
      rows_.front() = boundaryCond;
    }

  size_t getWidth() const { return width_; }
  size_t getHeight() const { return height_; }

  auto& operator[](size_t indx) { return rows_[indx]; }
  const auto& operator[](size_t indx) const { return rows_[indx]; }

  auto begin() { return rows_.begin(); }
  auto begin() const { return rows_.begin(); }
  auto end() { return rows_.end(); }
  auto end() const { return rows_.end(); }

  void fill(const Rule& rule) {
    auto prevRow = rows_.begin();
    auto curRow = std::next(rows_.begin());
    for (; curRow != rows_.end(); ++curRow) {
      std::bitset<3> neighbors;
      for (auto i = 0; i != static_cast<int>(width_); ++i) {
        neighbors[0] = prevRow->get((i - 1) % width_);
        neighbors[1] = prevRow->get(i);
        neighbors[2] = prevRow->get((i + 1) % width_);
        curRow->set(i, rule.calculateValue(neighbors));
      }
      prevRow = curRow;
    }
  }

  void dump() const {
    for (const auto& row : rows_) {
      row.dump();
    }
  }
};

class Model {
  Rule rule_;
  Polygon poly_;

public:
  Model(const Rule& rule, const Polygon& poly)
  : rule_(rule), poly_(poly) {}

  void fill() {
    poly_.fill(rule_);
  }

  const Polygon& getPolygon() const {
    return poly_;
  }
};

} // namespace automaton
