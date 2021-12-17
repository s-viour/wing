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
#include <wing/error.h>
#include <wing/build.h>
namespace fs = std::filesystem;


bool directory_is_project(const fs::path&);

wing::expected<void> run(const cxxopts::Options& opts, const cxxopts::ParseResult& result);
wing::expected<void> help(wing::application&&);
wing::expected<void> build(wing::application&&);
wing::expected<void> clean(wing::application&&);
wing::expected<void> init_vcpkg(wing::application&&);
wing::expected<void> vcpkg_install(wing::application&&);
wing::expected<void> new_project(wing::application&&);

int main(int argc, char* argv[]) {
  cxxopts::Options options("wing", "experimental c++ build system");
  options.add_options()
    ("h,help", "show help", cxxopts::value<std::string>())
    ("d,debug", "enable debug logging")
    ("command", "command to run", cxxopts::value<std::string>()->default_value("help"))
    ("arguments", "arguments to the command", cxxopts::value<std::vector<std::string>>())
    ;
  
  try {
    options.parse_positional({"command", "arguments"});
    auto result = options.parse(argc, argv);
    // setup spdlog properly based on the -d or --debug switch
    spdlog::set_level(spdlog::level::err);
    if (result["debug"].count()) {
      spdlog::set_level(spdlog::level::debug);
    }
    
    run(options, result)
      .map_error([](auto e) {
        fmt::print(stderr, "error executing command: {}\n", e);
      });
  } catch (cxxopts::OptionException&) {
    // if an exception was thrown during parse
    // print help and exit
    fmt::print("{}", options.help({"", "help", "debug"}));
    return 0;
  }
}

wing::expected<void> run(const cxxopts::Options& copts, const cxxopts::ParseResult& result) {
  auto cmd = result["command"].as<std::string>();
  auto args = result["command"].as<std::string>();
  auto opts = app_options(copts, result);
  opts.set_working_directory(fs::current_path());
  if (cmd == "help") {
    return create_application(opts)
      .and_then(help);
  } else if (cmd == "build") {
    opts
      .add_required_tool("ninja")
      .add_required_tool("c++");
    return create_application(opts)
      .and_then(build);
  } else if (cmd == "clean") {
    opts
      .add_required_tool("ninja");
    return create_application(opts)
      .and_then(clean);
  } else if (cmd == "install") {
    opts
      .add_required_tool("vcpkg/vcpkg");
    return create_application(opts)
      .and_then(vcpkg_install);
  } else if (cmd == "init-vcpkg") {
    opts
      .add_required_tool("git");
    return create_application(opts)
      .and_then(init_vcpkg);
  } else if (cmd == "new") {
    return create_application(opts)
      .and_then(new_project);
  }

  return wing::unexpected({"invalid command!"});
}

wing::expected<void> help(wing::application&& app) {
  fmt::print("{}", app.get_cli_options().help({"", "help", "debug"}));
  return {};
}

wing::expected<void> build(wing::application&& _app) {
  auto app = std::move(_app);
  if (!directory_is_project(app.get_working_directory())) {
    return wing::unexpected({"directory is not a project!"});
  }
  auto res1 = wing::generate_buildfile(app);
  if (!res1) return res1;
  auto result = wing::build_dir(app);
  fmt::print("finished build!\n");
  return result;
}

wing::expected<void> clean(wing::application&& _app) {
  auto app = std::move(_app);
  if (!directory_is_project(app.get_working_directory())) {
    return wing::unexpected({"directory is not a project!"});
  }
  auto status = app.get_tool("ninja").execute({"-C", app.get_working_directory() / "build", "-t", "clean"});
  if (status != 0) {
    return wing::unexpected({"error cleaning!"});
  }
  return {};
}

wing::expected<void> init_vcpkg(application&& _app) {
  auto app = std::move(_app);
  if (!directory_is_project(app.get_working_directory())) {
    return wing::unexpected({"directory is not a project!"});
  }
  auto& dir = app.get_working_directory();
  auto git = app.get_tool("git");
  auto path = (dir / "vcpkg");
  git.execute({"clone", "https://github.com/microsoft/vcpkg.git", path.string()});

  tool bootstrap("bootstrap", fs::path("./vcpkg/bootstrap-vcpkg.sh"));
  // TODO: check return value of execute
  bootstrap.execute({});
  return {};
}

wing::expected<void> vcpkg_install(application&& _app) {
  auto app = std::move(_app);
  if (!directory_is_project(app.get_working_directory())) {
    return wing::unexpected({"directory is not a project!"});
  }
  auto& dir = app.get_working_directory();
  tool vcpkg("vcpkg", (dir / "vcpkg/vcpkg"));
  auto excfg = load_config(dir / "wing.toml");
  if (!excfg) {
    return wing::unexpected(excfg.error());
  }
  auto cfg = excfg.value();

  for (auto& dep : cfg.dependencies) {
    vcpkg.execute({"install", dep.name});
  }
  return {};
}

wing::expected<void> new_project(application&& _app) {
  auto app = std::move(_app);
  auto args = app.get_cli_results()["arguments"].as<std::vector<std::string>>();
  if (args.empty()) {
    return wing::unexpected({"not enough arguments!"});
  }
  fs::path dir = args.at(0);
  project_template::default_project().create(dir);
  return {};
}

bool directory_is_project(const fs::path& dir) {
  fs::directory_entry srcdir(dir / "src");
  if (!srcdir.exists()) { return false; }

  fs::directory_entry manifest(dir / "wing.toml");
  if (!manifest.exists()) { return false; }

  return true;
}
