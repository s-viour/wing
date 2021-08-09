#include <filesystem>
#include <fstream>
#include <iostream>
#include <fmt/core.h>
#include <wing/ninja.h>
#include <wing/template.h>
#include <wing/tools.h>
#include <spdlog/spdlog.h>
namespace fs = std::filesystem;


bool directory_is_project(const fs::path&);
void generate_buildfile(fs::path&);
int build_dir(fs::path&);

int main(int argc, char* argv[]) {
  spdlog::set_level(spdlog::level::debug);
  using namespace wing;
  std::string progname(argv[0]);

  if (argc < 2) {
    fmt::print("not enough arguments!\n");
    fmt::print("usage: wing [new|build]\n");
    return 0;
  }

  std::string cmd(argv[1]);
  if (cmd == "new") {
    if (argc < 3) {
      fmt::print("not enough arguments to 'new' command!\n");
      fmt::print("usage: wing new [projectname]\n");
      return 0;
    }

    std::string projname(argv[2]);
    auto projdir = fs::current_path() / projname;
    project_from_template("wingdefault", projdir);

    fmt::print("generated project {} in directory {}\n", projname, projdir.string());
  } else if (cmd == "build") {
    auto pwd = fs::current_path();
    if (!directory_is_project(pwd)) {
      fmt::print(stderr, "error: current directory is not a project folder!\n");
      return 1;
    }

    fmt::print("generating buildfile...\n");
    generate_buildfile(pwd);
    fmt::print("generated buildfile successfully! building...\n");
    int status = -1;
    try {
      status = build_dir(pwd);
    } catch (std::runtime_error& e) {
      fmt::print(stderr, "ninja failed to execute properly!");
      return 1;
    }
    if (status != 0) {
      fmt::print("build failed!\n");
    } else {
      fmt::print("built executable target in {}/build\n", pwd.string());
    }
  }
  else {
    fmt::print("unknown subcommand: {}\n", cmd);
  }
  return 0;
}

bool directory_is_project(const fs::path& dir) {
  fs::directory_entry srcdir(dir / "src");
  if (!srcdir.exists()) { return false; }

  return true;
}

void generate_buildfile(fs::path& dir) {
  using namespace wing;

  auto build_dir = dir / "build";
  fs::create_directory(build_dir);

  std::ofstream buildfile((build_dir / "build.ninja").string());
  buildfile 
    << ninja_variable{"cflags", "-Wall -I../include"}
    << ninja_variable{"cxx", "g++"}
    << ninja_rule{
      .name="cc",
      .command="$cxx -MMD -MT $out -MF $out.d $cflags -c $in -o $out",
      .description="CC $out",
      .depfile="$out.d",
      .deps="gcc"
    }
    << ninja_rule{"link", "$cxx $ldflags -o $out $in $libs"};

  std::vector<fs::path> outputs;
  for (auto& p : fs::directory_iterator("src")) {
    if (p.path().extension() != ".cpp") {
      continue;
    }
    auto path = ".." / p.path();
    auto input = std::vector<fs::path>{path};
    auto output = path.filename().concat(".o");
    buildfile << ninja_build{.command="cc", .outputs = output, .inputs = input};
    outputs.push_back(output);
  }
  buildfile << ninja_build{.command="link", .outputs = "project", .inputs=outputs};
  buildfile << "default project\n";
}

int build_dir(fs::path& dir) {
  auto maybe_ninja = init_tool("ninja");
  spdlog::debug("tool [ninja] init: {}\n", maybe_ninja ? "good" : "failed");
  if (!maybe_ninja) {
    fmt::print(stderr, "ninja failed to initialize! is it in your PATH?");
    throw std::runtime_error("required tool failed to initialize");
  }
  auto ninja = maybe_ninja.value();

  return ninja.execute({"-C", (dir / "build").string()});
}