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
  class application {
  private:
    std::vector<std::string> cli_args;

    bool wd_is_project;
    fs::path working_directory;

    fs::path build_directory;
    std::optional<wing::project> directory_project;
    std::unordered_map<std::string, wing::tool> loaded_tools;

  public:
    application() : application(fs::current_path(), {}) {}
    application(const fs::path& p) : application(p, {}) {};
    application(const std::vector<std::string>& args) : application(fs::current_path(), args) {};
    application(const fs::path&, const std::vector<std::string>&);
    
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