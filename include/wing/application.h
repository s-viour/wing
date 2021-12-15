#ifndef __WING_APPLICATION_H__
#define __WING_APPLICATION_H__


#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <wing/tools.h>
#include <wing/error.h>

namespace fs = std::filesystem;


namespace wing {
  class app_options {
  private:
    std::vector<std::string> required_tools;

  public:
    app_options() = default;

    app_options& add_required_tool(const std::string&);
    const std::vector<std::string>& get_required() const;
  };

  class tool_not_found_error : public std::runtime_error {
  public:
    tool_not_found_error()
      : std::runtime_error("failed to initialize required tool!") {}
  };

  class application {
  private:
    std::unordered_map<std::string, wing::tool> tools;

  public:
    application() = default;
    tool& get_tool(const std::string&);
    void add_tool(const std::string&, const wing::tool&);
  };

  wing::expected<application> create_application(const app_options&);
}


#endif