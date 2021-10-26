#include <filesystem>
#include <fstream>
#include <spdlog/spdlog.h>
#include <wing/build.h>
#include <wing/ninja.h>

using namespace wing;

void wing::generate_buildfile(fs::path& dir) {
  using namespace wing;
  spdlog::debug("generating buildfile...");

  project_config cfg;
  try {
    cfg = load_config(dir / "wing.toml");
  } catch (std::runtime_error& e) {
    spdlog::error("{}", e.what());
    exit(1);
  }


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
}