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

application::application(const app_options& opts) {
  for (const auto& t : opts.get_required()) {
    auto path = find_tool(t);
    if (!path) {
      spdlog::error("failed to initialize required tool: {}", t);
      throw std::runtime_error("failed to initialize required tool!");
    }
    auto pair = std::make_pair(t, tool(t, path.value()));
    tools.insert(pair);
  }
}

tool& application::get_tool(const std::string& name) {
  return tools.at(name);
}