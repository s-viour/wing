#include <filesystem>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <fmt/core.h>
#include <cxxopts.hpp>
#include <spdlog/spdlog.h>
#include <wing/ninja.h>
#include <wing/template.h>
#include <wing/tools.h>
#include <wing/application.h>
#include <wing/config.h>
#include <wing/operations.h>
namespace fs = std::filesystem;

void run(const cxxopts::Options&, const cxxopts::ParseResult&);

int main(int argc, char* argv[]) {
  cxxopts::Options options("wing", "experimental c++ build system");
  options.add_options()
    ("h,help", "show help", cxxopts::value<std::string>())
    ("d,debug", "enable debug logging")
    ("command", "command to run", cxxopts::value<std::string>()->default_value("help"))
    ("arguments", "arguments to the command", cxxopts::value<std::vector<std::string>>()->implicit_value({}))
  ;
  
  try {
    options.parse_positional({"command", "arguments"});
    auto result = options.parse(argc, argv);
    // setup spdlog properly based on the -d or --debug switch
    spdlog::set_level(spdlog::level::err);
    if (result["debug"].count()) {
      spdlog::set_level(spdlog::level::debug);
    }
    
    run(options, result);
  } catch (cxxopts::OptionException&) {
    // if an exception was thrown during parse
    // print help and exit
    fmt::print("error encountered while parsing\n");
    fmt::print("{}", options.help({"", "help", "debug"}));
    return 0;
  }
}

void run(const cxxopts::Options& cli, const cxxopts::ParseResult& res) {
  app_options opts;
  auto ops = wing::load_operations();
  fmt::print("loaded operations\n");

  auto cmd = res["command"].as<std::string>();
  //auto args = res["arguments"].as<std::vector<std::string>>();
  try {
    fmt::print("trying to run\n");
    ops.at(cmd)->run(std::move(opts));
  } catch (const std::out_of_range& e) {
    fmt::print(stderr, "invalid command [{}]!", cmd);
  }
}
