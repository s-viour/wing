#include <wing/application.h>
#include <wing/error.h>
#include <spdlog/spdlog.h>

using namespace wing;


app_options& app_options::add_required_tool(const std::string& name) {
  required_tools.push_back(name);
  return *this;
}

app_options& app_options::set_working_directory(const fs::path& p) {
  working_directory = p;
  return *this;
}

tool& application::get_tool(const std::string& name) {
  return tools.at(name);
}

const fs::path& application::get_working_directory() const {
  return working_directory;
}

wing::expected<application> wing::create_application(const app_options& opts) {
  application app;
  app.working_directory = opts.working_directory;

  for (const auto& t : opts.required_tools) {
    auto path = find_tool(t);
    if (!path) {
      auto s = fmt::format("failed to initialize required tool [{}]", t);
      spdlog::error(s);
      return wing::unexpected(s);
    }
    app.tools.insert({t, tool(t, path.value())});
  }

  return app;
}