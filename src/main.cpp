#include <filesystem>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <fmt/core.h>
#include <wing/ninja.h>
#include <wing/template.h>
#include <wing/tools.h>
#include <wing/application.h>
#include <spdlog/spdlog.h>
#include <wing/config.h>
#include <wing/error.h>
namespace fs = std::filesystem;


bool directory_is_project(const fs::path&);
wing::expected<application> generate_buildfile(application&&);
wing::expected<application> build_dir(application&&);
wing::expected<application> vcpkg_install_deps(application&&);
wing::expected<application> init_vcpkg(application&&);

int main(int argc, char* argv[]) {
  spdlog::set_level(spdlog::level::debug);
  using namespace wing;
  std::string progname(argv[0]);

  if (argc < 2) {
    fmt::print(stderr, "not enough arguments!\n");
    fmt::print(stderr, "usage: wing [new|build|install|vcpkg-init|clean]\n");
    return 0;
  }

  std::string cmd(argv[1]);
  if (cmd == "new") {
    if (argc < 3) {
      fmt::print(stderr, "not enough arguments to 'new' command!\n");
      fmt::print(stderr, "usage: wing new [projectname]\n");
      return 0;
    }

    std::string projname(argv[2]);
    auto projdir = fs::current_path() / projname;
    project_template::default_project().create(projdir);

    fmt::print("generated project {} in directory {}\n", projname, projdir.string());
  } else if (cmd == "build") {
    auto pwd = fs::current_path();
    if (!directory_is_project(pwd)) {
      fmt::print(stderr, "error: current directory is not a project folder!\n");
      return 1;
    }

    auto opts = app_options()
      .add_required_tool("c++")
      .add_required_tool("ninja")
      .set_working_directory(pwd);
    
    auto result = create_application(opts)
      .and_then(generate_buildfile)
      .and_then(build_dir);
    if (!result) {
      spdlog::error("{}", result.error());
      return 1;
    }
  } else if (cmd == "init-vcpkg") {
    auto pwd = fs::current_path();
    if (!directory_is_project(pwd)) {
      fmt::print(stderr, "error: current directory is not a project folder!\n");
      return 1;
    }

    auto opts = app_options()
      .add_required_tool("git");
    
    auto result = create_application(opts)
      .and_then(init_vcpkg);
    if (!result) {
      spdlog::error("{}", result.error());
      return 1;
    }
  } else if (cmd == "install") {
    auto pwd = fs::current_path();
    if (!directory_is_project(pwd)) {
      fmt::print(stderr, "error: current directory is not a project folder!\n");
      return 1;
    }
    
    // safe to get value, this will succeed
    auto opts = app_options()
      .set_working_directory(pwd);
    auto result = create_application(opts)
      .and_then(vcpkg_install_deps);
    if (!result) {
      spdlog::error("{}", result.error());
      return 1;
    }
  } else if (cmd == "clean") {
    auto opts = app_options()
      .add_required_tool("ninja");
    auto app = create_application(opts);
    if (app) {
      app.value().get_tool("ninja").execute({"-C", "build", "-t", "clean"});
    } else {
      spdlog::error("{}", app.error());
      return 1;
    }
  } else {
    fmt::print("unknown subcommand: {}\n", cmd);
  }
  return 0;
}

bool directory_is_project(const fs::path& dir) {
  fs::directory_entry srcdir(dir / "src");
  if (!srcdir.exists()) { return false; }

  fs::directory_entry manifest(dir / "wing.toml");
  if (!manifest.exists()) { return false; }

  return true;
}

wing::expected<application> generate_buildfile(application&& mapp) {
  auto app = std::move(mapp);
  using namespace wing;
  spdlog::debug("generating buildfile...");

  auto& dir = app.get_working_directory();
  auto excfg = load_config(dir / "wing.toml");
  if (!excfg) {
    auto s = fmt::format("{}", excfg.error());
    spdlog::error(s);
    return wing::unexpected(s);
  }
  auto cfg = excfg.value();


  auto build_dir = dir / "build";
  fs::create_directory(build_dir);

  std::ofstream buildfile((build_dir / "build.ninja").string());
  buildfile 
    << ninja_variable{"cflags", "-Wall -I../include"}
    << ninja_variable{"cxx", "c++"};

  fs::directory_entry vcpkg(dir / "vcpkg");
  if (vcpkg.exists()) {
    buildfile
      << ninja_variable{"cflags", "$cflags -I../vcpkg/installed/x64-linux/include"};
      //<< ninja_variable{"ldflags", "-L../vcpkg/installed/x64-linux/lib"};
    
    {
      fs::directory_iterator libs(dir / "vcpkg/installed/x64-linux/debug/lib");
      for (auto& entry : libs) {
        if (!entry.is_regular_file()) {
          continue;
        }
        buildfile << ninja_variable{"libs", "$libs " + entry.path().string()};
      }
    }
  }
  
  buildfile
    << ninja_rule{
      .name="cc",
      .command="$cxx -std=c++17 -MMD -MT $out -MF $out.d $cflags -c $in -o $out",
      .description="CC $out",
      .depfile="$out.d",
      .deps="gcc"
    }
    << ninja_rule{"link", "$cxx -o $out $in $ldflags -Wl,--start-group $libs -Wl,--end-group -lpthread -lrt"};

  


  std::vector<fs::path> outputs;
  auto valid_exts = std::vector{".cpp", ".cxx", ".cc"};
  for (auto& p : fs::directory_iterator("src")) {
    auto pred = [p](auto val) { return val == p.path().extension(); };
    if (std::none_of(valid_exts.begin(), valid_exts.end(), pred)) {
      continue;
    }
    auto path = ".." / p.path();
    auto input = std::vector<fs::path>{path};
    auto output = path.filename().concat(".o");
    buildfile << ninja_build{.command="cc", .outputs = output, .inputs = input};
    outputs.push_back(output);
  }
  buildfile << ninja_build{.command="link", .outputs = cfg.name, .inputs=outputs};
  buildfile << "default " << cfg.name << '\n';
  return std::move(app);
}

wing::expected<application> build_dir(application&& mapp) {
  auto app = std::move(mapp);
  auto& dir = app.get_working_directory();
  auto ninja = app.get_tool("ninja");
  auto status = ninja.execute({"-C", (dir / "build").string()});
  if (status != 0) {
    return wing::unexpected(fmt::format("build failure! code: {}", status));
  }
  return std::move(app);
}

wing::expected<application> init_vcpkg(application&& mapp) {
  auto app = std::move(mapp);
  auto& dir = app.get_working_directory();
  auto git = app.get_tool("git");
  auto path = (dir / "vcpkg");
  git.execute({"clone", "https://github.com/microsoft/vcpkg.git", path.string()});

  tool bootstrap("bootstrap", fs::path("./vcpkg/bootstrap-vcpkg.sh"));
  bootstrap.execute({});
  return app;
}

wing::expected<application> vcpkg_install_deps(application&& mapp) {
  auto app = std::move(mapp);
  auto& dir = app.get_working_directory();
  tool vcpkg("vcpkg", (dir / "vcpkg/vcpkg"));
  auto excfg = load_config(dir / "wing.toml");
  if (!excfg) {
    auto s = fmt::format("{}", excfg.error());
    spdlog::error(s);
    // we should try and clean this up
    // ideally, all these effecting functions will return an expected/unexpected
    return wing::unexpected(s);
  }
  auto cfg = excfg.value();

  for (auto& dep : cfg.dependencies) {
    vcpkg.execute({"install", dep.name});
  }
  return app;
}
