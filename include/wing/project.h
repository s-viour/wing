#ifndef __WING_PROJECT_H__
#define __WING_PROJECT_H__

#include <string>
#include <vector>
#include <filesystem>
#include <wing/config.h>

namespace fs = std::filesystem;

namespace wing {
  // error thrown whenever project creation fails
  // due to the folder not being a valid project, or the project
  // config file not being valid
  class project_error : public std::runtime_error {
  public:
    project_error(const std::string& s)
      : std::runtime_error(s)
    {}
  };

  class project {
  private:
    wing::project_config project_cfg;
    fs::path project_dir;
    std::vector<fs::path> include_paths;
    std::vector<fs::path> source_files;

  public:
    // constructor that takes ownership of a config
    project() = default;
    project(project_config&& cfg, const fs::path& dir)
      : project_cfg(std::move(cfg))
      , project_dir(dir)
    {}

    void add_include(const fs::path&);
    void add_src(const fs::path&);
    const project_config& cfg() const;
    const fs::path& dir() const;
    const std::vector<fs::path>& includes() const;
    const std::vector<fs::path>& sources() const;
  };
}

#endif
