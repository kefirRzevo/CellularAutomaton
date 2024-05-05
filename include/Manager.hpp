#pragma once

#include <boost/program_options.hpp>
#include <filesystem>
#include <stdexcept>
#include <optional>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "View.hpp"

namespace po = boost::program_options;

namespace automaton {

class Manager final {
  std::optional<std::string> boundCond_;
  std::optional<std::string> boundCondFile_;
  std::optional<unsigned int> boundCondRandSeed_;
  std::optional<size_t> height_;
  std::optional<unsigned char> ruleVal_;
  bool help_ = false;

  po::options_description options_{"Options"};

  void initProgramOptions() {
    po::options_description generic("Generic options");
    generic.add_options()("help,h", "help message");

    po::options_description config("Configuration");
    config.add_options()
    ("bound-cond", po::value<std::string>(), "Boundary condition")
    ("bound-cond-file", po::value<std::string>(), "File with boundary condition")
    ("rand", po::value<unsigned int>()->default_value(0), "Seed for randomizing boundary condition")
    ("height", po::value<size_t>()->default_value(64), "Height of the polygon")
    ("rule", po::value<unsigned int>()->default_value(110), "Rule that will be applied");

    options_.add(generic).add(config);
  }

  void parseProgramOptions(int argc, const char *argv[]) {
    po::variables_map vm;
    try {
      auto parser = po::command_line_parser(argc, argv);
      po::store(parser.options(options_).run(), vm);
      po::notify(vm);

      if (vm.count("help")) {
        std::cout << options_ << std::endl;
        help_ = true;
        return;
      }
      std::filesystem::path fullpath;
      if (vm.count("bound-cond")) {
        auto boundCond = vm["bound-cond"].as<std::string>();
        boundCond_.emplace(boundCond);
      }
      if (vm.count("bound-cond-file")) {
        auto boundCondFile = vm["bound-cond-file"].as<std::string>();
        boundCondFile_.emplace(boundCondFile);
      }
      if (vm.count("rand")) {
        auto boundCondRandSeed = vm["rand"].as<unsigned int>();
        boundCondRandSeed_.emplace(boundCondRandSeed);
      }
      if (vm.count("height")) {
        auto height = vm["height"].as<size_t>();
        height_.emplace(height);
      }
      if (vm.count("rule")) {
        auto ruleVal = vm["rule"].as<unsigned int>();
        if (ruleVal > 255) {
          throw std::logic_error("Max rule is 2^8-1=255");
        }
        ruleVal_.emplace(ruleVal);
      }
    } catch (const po::required_option &e) {
      if (vm.count("help")) {
        std::cout << options_ << std::endl;
        help_ = true;
        return;
      } else {
        throw;
      }
    }
  }

  void process() {
    if (help_) {
      return;
    }
    size_t height;
    if (height_.has_value()) {
      height = height_.value();
    } else {
      throw std::logic_error("Height isn't set");
    }
    unsigned char ruleVal;
    if (ruleVal_.has_value()) {
      ruleVal = ruleVal_.value();
    } else {
      throw std::logic_error("Rule isn't set");
    }
    Rule rule{ruleVal};
    Row boundCond;
    if (boundCond_.has_value()) {
      boundCond = Row{boundCond_.value()};
    } else if (boundCondFile_.has_value()) {
      std::ifstream file{boundCondFile_.value()};
      std::ostringstream fileBuf;
      fileBuf << file.rdbuf();
      boundCond = Row{fileBuf.str()};
    } else if (boundCondRandSeed_.has_value()) {
      boundCond = Row{height};
      boundCond.randomize(boundCondRandSeed_.value());
    } else {
      throw std::logic_error("Boundary condition isn't set");
    }
    Polygon poly{height, boundCond};
    Model model{rule, poly};
    View view{model};
    view.run();
  }

public:
  void run(int argc, const char *argv[]) {
    initProgramOptions();
    parseProgramOptions(argc, argv);
    process();
  }
};

} // namespace paracl
