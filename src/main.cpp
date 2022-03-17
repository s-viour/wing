#include <filesystem>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <exception>
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


int run(const cxxopts::Options&, const cxxopts::ParseResult&);

int main(int argc, char* argv[]) {
  // setup CLI options 
  cxxopts::Options options("wing", "experimental c++ build system");
  options.add_options()
    ("h,help", "show help", cxxopts::value<std::string>())
    ("d,debug", "enable debug logging")
    ("command", "command to run", cxxopts::value<std::string>()->default_value("help"))
    ("arguments", "arguments to the command", cxxopts::value<std::vector<std::string>>()->default_value({}));
  ;
  
  try {
    options.parse_positional({"command", "arguments"});
    auto result = options.parse(argc, argv);
    // setup spdlog properly based on the -d or --debug switch
    spdlog::set_level(spdlog::level::err);
    if (result["debug"].count()) {
      spdlog::set_level(spdlog::level::debug);
    }
    
    // delegate parsed-argument handling to run function
    // it'll return success or failure based on how that goes
    return run(options, result);
  } catch (cxxopts::OptionException&) {
    // if an exception was thrown during parse
    // print help and exit
    fmt::print(stderr, "invalid arguments!\n");
    fmt::print(stderr, "{}", options.help({"", "help", "debug"}));
    return 1;
  }
}

int run(const cxxopts::Options& cli, const cxxopts::ParseResult& res) {
  // get arguments into correct types for handling
  auto cmd = res["command"].as<std::string>();
  auto args = res["arguments"].as<std::vector<std::string>>();
  if (std::all_of(args.begin(), args.end(), [](const std::string& s) { return s.empty(); })) {\
    args.clear();
  }
  // special case, handle `help` command explicitly
  if (cmd == "help") {
    fmt::print("{}", cli.help());
    return 0;
  }

  // load operations, attempt to find the matching one, and exit with error if not found
  spdlog::debug("attempting to run operation {}", cmd);
  auto ops = wing::load_operations();
  auto found = ops.find(cmd);
  if (found == ops.end()) {
    fmt::print(stderr, "invalid command {}!\n", cmd);
    return 1;
  }

  // initialize application in current directory
  application app(cmd, args);

  // load all required tools
  // if any of them fail to be found, exit with error
  auto& op = found->second;
  for (auto& rqt : op.required_tools) {
    auto tool = wing::find_tool(rqt);
    if (!tool) {
      fmt::print(stderr, "could not find tool {}!\n", rqt);
      return 1;
    }
    app.add_tool(rqt, *tool);
  }

  // check directory requirements
  //   * operations that require a project-directory may not be used OUTSIDE one
  //   * operations that don't require a project-directory may be used inside one, though
  if (op.requires_project && !app.is_project()) {
    fmt::print(stderr, "current directory is not valid project!\n");
    return 1;
  }
  if (op.requires_loaded_project) {
    try {
      app.load_project();
    } catch (const wing::project_error& e) {
      fmt::print(stderr, "error in project: {}", e.what());
    } catch (const wing::config_error& e) {
      fmt::print(stderr, "error in config: {}", e.what());
    }
  }
  op.run(app);
  return 0;
}
