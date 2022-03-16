#include <filesystem>
#include <fstream>
#include <spdlog/spdlog.h>
#include <wing/build.h>
#include <wing/ninja.h>
#include <wing/application.h>
#include <wing/project.h>

using namespace wing;

wing::project wing::load_project(const fs::path& dir) {
  // first, check and ensure the required files are there
  if (!fs::exists(dir / "src")) {
    throw project_error("missing src directory");
  } else if (!fs::exists(dir / "wing.toml")) {
    throw project_error("missing wing.toml file");
  } else if (fs::is_empty(dir / "src")) {
    throw project_error("src directory is empty");
  }

  // this may throw, the caller should attempt to catch
  // BOTH project_error and config_error
  auto cfg = load_config(dir / "wing.toml");
  project prj(std::move(cfg), dir);

  std::vector<fs::path> valid_exts{".cpp", ".cxx", ".cc"};

  // add source and header files
  // use recursive_directory_iterator to make sure we find
  // every single source file
  fs::recursive_directory_iterator src(dir / "src");
  for (auto& p : src) {
    auto pred = [p](auto val) { return val == p.path().extension(); };
    if (std::none_of(valid_exts.begin(), valid_exts.end(), pred)) {
      continue;
    }
    prj.add_src(p);
  }

  // for now, add both include/ and src/ as include directories
  if (fs::exists(dir / "include")) {
    prj.add_include(dir / "include");
  }
  prj.add_include(dir / "src");

  // if vcpkg directory is present, attempt to find its include too
  if (fs::exists(dir / "vcpkg")) {
    prj.add_include(dir / "vcpkg/installed/x64-linux/include");
    fs::directory_iterator vcpkg_libs(dir / "vcpkg/installed/x64-linux/lib");
    for (auto& l : vcpkg_libs) {
      if (!l.is_regular_file()) continue;
      prj.add_library(l);
    }
  }

  return prj;
}


void wing::generate_buildfile(const project& prj) {
  spdlog::debug("generating buildfile for project {}", prj.dir().string());

  auto& cfg = prj.cfg();
  auto& dir = prj.dir();
  auto build_dir = dir / "build";
  fs::create_directory(build_dir);

  // generate variables
  // this includes generating the $cflags and $libs variables
  // which will include parsing include files and lib files
  std::ofstream buildfile((build_dir / "build.ninja").string());
  buildfile 
    << ninja_variable{"cflags", "-Wall"}
    << ninja_variable{"cxx", "c++"};

  for (auto& inc : prj.includes()) {
    buildfile << ninja_variable{"cflags", fmt::format("$cflags -I{}", inc.string())};
  }
  for (auto& lib : prj.libs()) {
    buildfile << ninja_variable{"libs", fmt::format("$libs {}", lib.string())};
  }
  
  // generate the necessary rules for building
  buildfile
    << ninja_rule{
      .name="cc",
      .command="$cxx -std=c++17 -MMD -MT $out -MF $out.d $cflags -c $in -o $out",
      .description="CC $out",
      .depfile="$out.d",
      .deps="gcc"
    }
    << ninja_rule{"link", "$cxx -o $out $in $ldflags -Wl,--start-group $libs -Wl,--end-group -lpthread -lrt"};


  // generate build commands for src files
  std::vector<fs::path> objects;
  for (auto& s : prj.sources()) {
    auto output = s.filename();
    output.replace_extension(".o");
    objects.push_back(output);
    buildfile << ninja_build{.command="cc", .outputs={output}, .inputs={s}};
  }

  // generate link command and default
  buildfile << ninja_build{.command="link", .outputs={cfg.name}, .inputs=objects};
  buildfile << "default " << cfg.name << '\n';
  spdlog::debug("successfully generated buildfile!");
}

int wing::build_dir(application& app) {
  auto ninja = app.get_tool("ninja");
  return ninja.execute({"-C", "build"});
}
