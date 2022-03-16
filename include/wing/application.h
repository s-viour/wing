#ifndef __WING_APPLICATION_H__
#define __WING_APPLICATION_H__


#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <cxxopts.hpp>
#include <wing/tools.h>
#include <wing/config.h>
#include <wing/project.h>

namespace fs = std::filesystem;

namespace wing {
  /// class that represents the application state within a particular directory
  /// the application state consists of:
  ///   * cli_command       - command to be executed by the application
  ///   * cli_args          - arguments to the application
  ///   * wd_is_project     - whether or not the application has been init'd inside a project
  ///   * working_directory - the working directory of the application
  ///   * build_directory   - build directory of the project (may be unset)
  ///   * directory_project - optional value storing the project structure (project.h) if wd_is_project is true
  ///   * loaded_tools      - mapping of tools usable by operations
  ///
  class application {
  private:
    std::string cli_command;
    std::vector<std::string> cli_args;

    bool wd_is_project;
    fs::path working_directory;

    fs::path build_directory;
    std::optional<wing::project> directory_project;
    std::unordered_map<std::string, wing::tool> loaded_tools;

  public:
    application() : application(fs::current_path(), {}) {}
    application(const std::string& cmd, const std::vector<std::string>& args) : application(fs::current_path(), cmd, args) {};
    application(const fs::path&, const std::string&, const std::vector<std::string>&);
    
    const bool is_project() const;
    const fs::path& get_working_dir() const;
    const fs::path& get_build_dir() const;
    const std::vector<std::string>& get_args() const;
    const project_config& get_config() const;
    const wing::project& get_project() const;

    void add_tool(const std::string&, const fs::path&);
    tool& get_tool(const std::string&);
  };
}


#endif