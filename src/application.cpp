#include <wing/application.h>
#include <spdlog/spdlog.h>

using namespace wing;


app_options& app_options::add_required_tool(const std::string& name) {
  required_tools.push_back(name);
  return *this;
}

const std::vector<std::string>& app_options::get_required() const {
  return required_tools;
}

tool& application::get_tool(const std::string& name) {
  return tools.at(name);
}

void application::add_tool(const std::string& name, const tool& t) {
  tools.insert({name, t});
}

application wing::init_application(const app_options& opts) {
  application app;

  for (const auto& t : opts.get_required()) {
    auto path = find_tool(t);
    if (!path) {
      spdlog::error("failed to initialize required tool: {}", t);
      throw tool_not_found_error();
    }
    app.add_tool(t, tool(t, path.value()));
  }

  return app;
}