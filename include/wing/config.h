#ifndef __WING_CONFIG_H__
#define __WING_CONFIG_H__


#include <string>
#include <vector>
#include <optional>
#include <filesystem>

namespace fs = std::filesystem;


namespace wing {
  /// represents a dependency of any type
  /// presently, this structure is incredible barebones because the only dependency we support is vcpkg
  ///
  struct dependency {
    const std::string name;

    dependency() = default;
  };

  /// error thrown when config files are invalid or incorrectly-defined in some way
  ///
  class config_error : public std::runtime_error {
  public:
    config_error(const std::string& s)
      : std::runtime_error(s) {} 
  };

  /// completely represents all possible keys in a wing.toml file
  /// 
  struct project_config {
    std::string name;
    //const project_type type;
    //fs::path vcpkg_dir;
    std::vector<dependency> dependencies;
  };

  /// attempts to load a config file given a directory path
  /// throws: config_error
  ///
  wing::project_config load_config(const fs::path&);
}


#endif