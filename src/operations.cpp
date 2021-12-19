#include <wing/application.h>
#include <wing/operations.h>
#include <wing/build.h>

void wing::build_operation::run(app_options&& _opts) {
  auto prj = wing::load_project(fs::current_path());

  auto opts = std::move(_opts);
  opts
    .add_required_tool("ninja")
    .add_required_tool("c++");

  auto app = wing::create_application(opts);
  wing::generate_buildfile(prj);
  wing::build_dir(app);
}

void wing::clean_operation::run(app_options&& _opts) {
  auto opts = std::move(_opts);
  opts
    .add_required_tool("ninja");
  auto app = wing::create_application(opts);
  app.get_tool("ninja").execute({"-C", "build", "-t", "clean"});
}

std::unordered_map<std::string, std::unique_ptr<wing::operation>> wing::load_operations() {
  std::unordered_map<std::string, std::unique_ptr<wing::operation>> m;
  m.insert({"build", std::make_unique<build_operation>()});
  m.insert({"clean", std::make_unique<clean_operation>()});

  return m;
}