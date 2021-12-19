#include <spdlog/spdlog.h>
#include <wing/application.h>
#include <wing/tools.h>


wing::app_options& wing::app_options::add_required_tool(const std::string& name) {
  required_tools.push_back(name);
  return *this;
}

wing::application::application(const app_options& opts) {
  for (const auto& t : opts.required_tools) {
    auto path = wing::find_tool(t);
    if (!path) {
      auto s = fmt::format("failed to initialize required tool [{}]", t);
      spdlog::error(s);
      throw application_error(s);
    }
    tools.insert({t, tool(t, path.value())});
  }
}

wing::tool& wing::application::get_tool(const std::string& name) {
  return tools.at(name);
}

wing::application wing::create_application(const app_options& opts) {
  return wing::application(opts);
}
