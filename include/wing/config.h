#ifndef __WING_CONFIG_H__
#define __WING_CONFIG_H__


#include <string>
#include <vector>
#include <optional>
#include <filesystem>
#include <wing/error.h>

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

  struct project_config {
    std::string name;
    //const project_type type;
    //fs::path vcpkg_dir;
    std::vector<dependency> dependencies;

    project_config() = default;
  };

  wing::expected<project_config> load_config(const fs::path&);
}


#endif