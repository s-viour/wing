#ifndef __WING_CONFIG_H__
#define __WING_CONFIG_H__


#include <string>
#include <vector>
#include <optional>
#include <filesystem>

namespace fs = std::filesystem;


namespace wing {
  enum project_type {
    EXECUTABLE
  };

  enum dependency_type {
    VCPKG
  };

  struct dependency {
    const std::string name;
    //const dependency_type type = dependency_type::VCPKG;

    dependency() = default;
  };

  class config_error : public std::runtime_error {
  public:
    config_error(const std::string& s)
      : std::runtime_error(s) {} 
  };

  struct project_config {
    std::string name;
    //const project_type type;
    //fs::path vcpkg_dir;
    std::vector<dependency> dependencies;
  };

  wing::project_config load_config(const fs::path&);
}


#endif