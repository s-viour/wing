#ifndef __WING_PROJECT_H__
#define __WING_PROJECT_H__

#include <string>
#include <vector>
#include <filesystem>
#include <wing/config.h>

namespace fs = std::filesystem;

namespace wing {
  /// error thrown whenever project creation fails
  /// this can be due to the folder not being a valid project, or the project
  /// config file not being valid
  ///
  class project_error : public std::runtime_error {
  public:
    project_error(const std::string& s)
      : std::runtime_error(s)
    {}
  };

  /// class that represents the structure, state, and all files within a project
  /// fields:
  ///   * project_cfg   - project_config instance that stores the project's config
  ///   * project_dir   - directory the project resides in
  ///   * include_paths - list of paths to search for headers
  ///   * source_files  - list of source files (.cpp, .cxx, etc.) to be turned into object files
  ///   * lib_files     - list of external library files to be linked during build
  ///   * needs_install - whether or not the project is missing dependencies, and an install pass is needed
  ///
  class project {
  private:
    wing::project_config project_cfg;
    fs::path project_dir;
    std::vector<fs::path> include_paths;
    std::vector<fs::path> source_files;
    std::vector<fs::path> lib_files;
    bool needs_install = false;

  public:
    project() = default;
    project(project_config&& cfg, const fs::path& dir)
      : project_cfg(std::move(cfg))
      , project_dir(dir)
    {}

    /// add an include search path to the project
    void add_include(const fs::path&);
    /// add a source file to the project
    void add_src(const fs::path&);
    /// add a library file to the project
    void add_library(const fs::path&);
    /// set the needs_install flag to the specified value
    void set_needs_install(const bool);
    
    // getters
    const project_config& cfg() const;
    const fs::path& dir() const;
    const std::vector<fs::path>& includes() const;
    const std::vector<fs::path>& sources() const;
    const std::vector<fs::path>& libs() const;
  };
}

#endif
