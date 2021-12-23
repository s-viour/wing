#include <fmt/core.h>
#include <wing/application.h>
#include <wing/build.h>
#include <wing/template.h>
#include <wing/operations.h>


class build_operation : public wing::operation {
public:
  void run(wing::app_options&& _opts, const std::vector<std::string>&) {
    auto prj = wing::load_project(fs::current_path());

    auto opts = std::move(_opts);
    opts
      .add_required_tool("ninja")
      .add_required_tool("c++");

    auto app = wing::create_application(opts);
    wing::generate_buildfile(prj);
    wing::build_dir(app);
  }
};

class clean_operation : public wing::operation {
public:
  void run(wing::app_options&& _opts, const std::vector<std::string>&) {
    auto opts = std::move(_opts);
    opts
      .add_required_tool("ninja");
    auto app = wing::create_application(opts);
    app.get_tool("ninja").execute({"-C", "build", "-t", "clean"});
  }
};

class new_operation : public wing::operation {
public:
  void run(wing::app_options&& _opts, const std::vector<std::string>& args) {
    if (args.empty()) {
      fmt::print(stderr, "missing arguments to command!\n");
      return;
    }
    auto opts = std::move(_opts);
    auto app = wing::create_application(opts);
    auto tmplt = wing::project_template::default_project();
    tmplt.create(fs::current_path() / args[0]);
  }
};

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

class install_operation : public wing::operation {
public:
  void run(wing::app_options&& _opts, const std::vector<std::string>&) {
    auto opts = std::move(_opts);
    opts
      .add_required_tool("vcpkg/vcpkg");
    auto app = wing::create_application(opts);
    auto cfg = wing::load_config(fs::current_path() / "wing.toml");
    auto vcpkg = app.get_tool("vcpkg/vcpkg");
    for (auto& dep : cfg.dependencies) {
      vcpkg.execute({"install", dep.name});
    }
  }
};

class run_operation : public wing::operation {
public:
  void run(wing::app_options&& _opts, const std::vector<std::string>&) {
    auto cfg = wing::load_config(fs::current_path() / "wing.toml");
    auto opts = std::move(_opts);
    auto cloned_opts = opts;
    auto exename = fmt::format("build/{}", cfg.name);
    opts
      .add_required_tool(exename);
    wing::application app;
    try {
      app = wing::create_application(opts);
    } catch (const wing::application_error& e) {
      fmt::print("building executable before running\n");
      build_operation().run(std::move(cloned_opts), {});
    }
    app = wing::create_application(opts);
    app.get_tool(exename).execute({});
  }
};

std::unordered_map<std::string, std::unique_ptr<wing::operation>> wing::load_operations() {
  std::unordered_map<std::string, std::unique_ptr<wing::operation>> m;
  m.insert({"build", std::make_unique<build_operation>()});
  m.insert({"clean", std::make_unique<clean_operation>()});
  m.insert({"new", std::make_unique<new_operation>()});
  m.insert({"init-vcpkg", std::make_unique<init_vcpkg_operation>()});
  m.insert({"install", std::make_unique<install_operation>()});
  m.insert({"run", std::make_unique<run_operation>()});

  return m;
}
