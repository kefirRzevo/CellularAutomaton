#pragma once

#include <boost/program_options.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>

#include "View.hpp"

namespace po = boost::program_options;

namespace automaton {

class Manager final {
  std::optional<std::string> boundCondStr_;
  std::optional<std::string> boundCondFile_;
  std::optional<std::string> buildFile_;
  std::optional<size_t> width_;
  size_t height_;
  unsigned char ruleVal_;
  bool help_ = false;

  po::options_description options_{"Options"};

  void initProgramOptions() {
    po::options_description generic("Generic options");
    generic.add_options()("help,h", "help message");

    po::options_description config("Configuration");
    config.add_options()("bound-cond-str", po::value<std::string>(),
                         "Boundary condition string")(
        "bound-cond-file", po::value<std::string>(),
        "File with boundary condition")(
        "build-file", po::value<std::string>(),
        "File to build boundary condition")(
        "rand", po::value<size_t>()->default_value(4000),
        "Width for randomizing boundary condition")(
        "height", po::value<size_t>()->default_value(3000)->required(),
        "Height of the polygon")(
        "rule", po::value<unsigned int>()->default_value(110)->required(),
        "Rule that will be applied");

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
      if (vm.count("bound-cond-str"))
        boundCondStr_ = vm["bound-cond-str"].as<std::string>();
      if (vm.count("bound-cond-file"))
        boundCondFile_ = vm["bound-cond-file"].as<std::string>();
      if (vm.count("build-file"))
        buildFile_ = vm["build-file"].as<std::string>();
      if (vm.count("rand"))
        width_ = vm["rand"].as<size_t>();
      if (vm.count("height"))
        height_ = vm["height"].as<size_t>();
      if (vm.count("rule")) {
        auto ruleVal = vm["rule"].as<unsigned int>();
        if (ruleVal > 255)
          throw std::logic_error("Max rule is 2^8-1=255");
        ruleVal_ = ruleVal;
      }
    } catch (const po::required_option &e) {
      if (vm.count("help")) {
        std::cout << options_ << std::endl;
        help_ = true;
        return;
      }
      throw;
    }
  }

  void process() {
    if (help_)
      return;
    BoundCond boundCond;
    if (boundCondStr_.has_value()) {
      boundCond = BoundCond::createFromString(boundCondStr_.value());
    } else if (boundCondFile_.has_value()) {
      auto string = automaton::readAndJoin(boundCondFile_.value());
      boundCond = BoundCond::createFromString(string);
    } else if (buildFile_.has_value()) {
      BoundCondBuilder builder;
      boundCond = builder.build(buildFile_.value());
    } else if (width_.has_value()) {
      boundCond = BoundCond::createRandom(width_.value());
    } else {
      auto msg = "Boundary condition is not set";
      throw std::runtime_error(msg);
    }
    Rule rule{ruleVal_};
    Polygon poly{height_, boundCond.begin(), boundCond.end()};
    Model model{std::move(rule), std::move(poly)};
    View view{std::move(model)};
    view.run();
  }

public:
  void run(int argc, const char *argv[]) {
    initProgramOptions();
    parseProgramOptions(argc, argv);
    process();
  }
};

} // namespace automaton
