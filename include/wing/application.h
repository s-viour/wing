#ifndef __WING_APPLICATION_H__
#define __WING_APPLICATION_H__


#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <spdlog/spdlog.h>
#include <wing/tools.h>
#include <wing/error.h>

namespace fs = std::filesystem;


namespace wing {
  struct app_options {
    std::vector<std::string> required_tools;
    fs::path working_directory;

    app_options() = default;
    app_options& add_required_tool(const std::string&);
    app_options& set_working_directory(const fs::path&);
  };

  class application {
  private:
    std::unordered_map<std::string, wing::tool> tools;
    fs::path working_directory;
    friend wing::expected<application> create_application(const app_options&);

  public:
    application() = default;
    application(const application&) = delete;
    application(application&& a) = default;
    tool& get_tool(const std::string&);
    const fs::path& get_working_directory() const;
  };

  wing::expected<application> create_application(const app_options&);
}


#endif