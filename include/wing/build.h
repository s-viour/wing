#ifndef __WING_BUILD_H__
#define __WING_BUILD_H__

#include <filesystem>
#include <vector>
#include <wing/application.h>
#include <wing/config.h>
#include <wing/project.h>
namespace fs = std::filesystem;

namespace wing {
  wing::project load_project(const fs::path&);
  void generate_buildfile(const project&);
  int build_dir(wing::application&);
}

#endif