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

  // add source and header files
  // use recursive_directory_iterator to make sure we find
  // every single source file
  fs::recursive_directory_iterator src(dir / "src");
  for (auto& file : src) {
    prj.add_src(file);
  }

  if (fs::exists(dir / "include")) {
    // non-recursive directory traversal for the include folder
    // we only want to go one-level deep here since the organization
    // in the include folder actually matters
    fs::directory_iterator include(dir / "include");
    for (auto& file : include) {
      prj.add_include(file);
    }
  }

  return prj;
}


void wing::generate_buildfile(const project& prj) {
  spdlog::debug("generating buildfile...");

  auto& cfg = prj.cfg();
  auto& dir = prj.dir();
  auto excfg = load_config(dir / "wing.toml");


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
  for (auto& p : fs::recursive_directory_iterator("src")) {
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
  spdlog::debug("successfully generated buildfile!");
}

int wing::build_dir(application& app) {
  auto ninja = app.get_tool("ninja");
  return ninja.execute({"-C", "build"});
}
