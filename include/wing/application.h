#ifndef __WING_APPLICATION_H__
#define __WING_APPLICATION_H__


#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <cxxopts.hpp>
#include <wing/tools.h>

namespace fs = std::filesystem;

namespace wing {
  struct app_options {
    std::vector<std::string> required_tools;

    app_options() = default;
    app_options& add_required_tool(const std::string&);
  };

  class application_error : public std::runtime_error {
  public:
    application_error(const std::string& s)
      : std::runtime_error(s)
    {}
  };

  class application {
  private:
    std::unordered_map<std::string, wing::tool> tools;

  public:
    application() = default;
    application(const app_options&);
    
    tool& get_tool(const std::string&);
  };

  application create_application(const app_options&);
}


#endif