#include <spdlog/spdlog.h>
#include <fmt/core.h>
#include <wing/application.h>
#include <wing/build.h>
#include <wing/template.h>
#include <wing/operations.h>
#include <wing/application.h>
#include <wing/project.h>



int run_build(wing::application& app) {
  wing::generate_buildfile(app.get_project());
  return wing::build_dir(app);
}

int run_clean(wing::application& app) {
  return app.get_tool("ninja").execute({"-C", "build", "-t", "clean"});
}

int run_new(wing::application& app) {
  auto& args = app.get_args();
  if (args.empty()) {
    fmt::print(stderr, "missing arguments to command!\n");
    return 1;
  }
  auto tmplt = wing::project_template::default_project();
  tmplt.create(app.get_working_dir() / args[0]);
  return 0;
}

/*
 * not sure if this command needs to come back
class init_vcpkg_operation : public wing::operation {
public:
  void run(wing::app_options&& _opts, const std::vector<std::string>&) {
    auto opts = std::move(_opts);
    opts
      .add_required_tool("git");
    auto app = wing::create_application(opts);
    auto git = app.get_tool("git");
    auto path = fs::current_path() / "vcpkg";
    git.execute({"clone", "https://github.com/microsoft/vcpkg.git", path.string()});
    wing::tool bootstrap("bootstrap", fs::path("./vcpkg/bootstrap-vcpkg.sh"));
    if (bootstrap.execute({}) != 0) {
      fmt::print(stderr, "bootstrap script failed to execute!\n");
    }
  }
};
*/


int run_install(wing::application& app) {
  spdlog::debug("running install");
  auto cfg = app.get_config();
  spdlog::debug("loaded config");
  auto vcpkg = app.get_tool("vcpkg/vcpkg");
  for (auto& dep : cfg.dependencies) {
    spdlog::debug("installing {}", dep.name);
    auto status = vcpkg.execute({"install", dep.name});
    if (status != 0) {
      fmt::print(stderr, "vcpkg failed!\n");
      return status;
    }
  }
  return 0;
}


int run_run(wing::application& app) {
  const auto& cfg = app.get_config();
  auto exename = fmt::format("build/{}", cfg.name);
  if (!fs::exists(exename)) {
    fmt::print("building executable before running");
    run_build(app);
  }
  wing::tool executable(exename);
  return executable.execute(app.get_args());
}

std::unordered_map<std::string, wing::operation> wing::load_operations() {
  std::unordered_map<std::string, operation> m;
  auto build_operation = wing::operation {
    .requires_project = true,
    .required_tools = {"ninja", "c++"},
    .run = run_build
  };
  auto clean_operation = wing::operation {
    .requires_project = true,
    .required_tools = {"ninja"},
    .run = run_clean
  };
  auto new_operation = wing::operation {
    .requires_project = false,
    .required_tools = {},
    .run = run_new
  };
  auto run_operation = wing::operation {
    .requires_project = false,
    .required_tools = {},
    .run = run_run
  };
  auto install_operation = wing::operation {
    .requires_project = true,
    .required_tools = {"vcpkg/vcpkg"},
    .run = run_install
  };

  // we may be able to optimize this by moving it into the insert
  // not sure though
  // TODO: investigate this
  m.insert({"build", build_operation});
  m.insert({"clean", clean_operation});
  m.insert({"new", new_operation});
  m.insert({"run", run_operation});
  m.insert({"install", install_operation});


  return m;
}
