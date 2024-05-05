#pragma once

#include <string>
#include <random>
#include <bitset>
#include <iostream>
#include <algorithm>

#include <boost/dynamic_bitset.hpp> 

namespace automaton {

class Rule {
  unsigned char val_;

public:
  Rule(unsigned char val) : val_(val) {}

  bool calculateValue(std::bitset<3> neighbors) const {
    return val_ & (1 << neighbors.to_ulong());
  }
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

  void randomize(unsigned int seed) {
    std::srand(seed);
    std::vector<bool> data(data_.size());
    size_t size = std::rand() % data_.size();
    std::fill(data.begin(), data.begin() + size, 1);
    std::shuffle(data.begin(), data.end(), std::mt19937{});
    for (size_t i = 0; i != data_.size(); ++i) {
      data_.set(i, data[i]);
    }
    data_.shrink_to_fit();
  }
};

class Polygon {
  size_t width_;
  size_t height_;
  size_t curRowIndx_;

  std::vector<Row> rows_;

public:
  Polygon(size_t height, const Row& boundaryCond)
  : width_(boundaryCond.size()), height_(height),
    curRowIndx_(0), rows_(height) {
      std::fill(rows_.begin(), rows_.end(), Row{width_});
      rows_[0] = boundaryCond;
    }

  size_t getWidth() const { return width_; }
  size_t getHeight() const { return height_; }

  auto& operator[](size_t indx) { return rows_[indx]; }
  const auto& operator[](size_t indx) const { return rows_[indx]; }

  auto begin() { return rows_.begin(); }
  auto begin() const { return rows_.begin(); }
  auto end() { return rows_.end(); }
  auto end() const { return rows_.end(); }

  void update(const Rule& rule) {
    auto& curRow = rows_.at(curRowIndx_);
    curRowIndx_ = (curRowIndx_ + 1) % height_;
    auto& nextRow = rows_.at(curRowIndx_);
    std::bitset<3> neighbors;
    for (auto i = 0; i != static_cast<int>(width_); ++i) {
      neighbors[0] = curRow.get((i - 1) % width_);
      neighbors[1] = curRow.get(i);
      neighbors[2] = curRow.get((i + 1) % width_);
      nextRow.set(i, rule.calculateValue(neighbors));
    }
  }
};

class Model {
  Rule rule_;
  Polygon poly_;

public:
  Model(const Rule& rule, const Polygon& poly)
  : rule_(rule), poly_(poly) {}

  void update() {
    poly_.update(rule_);
  }

  const Polygon& getPolygon() const {
    return poly_;
  }
};

} // namespace automaton
