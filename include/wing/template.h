#ifndef __WING_TEMPLATE_H__
#define __WING_TEMPLATE_H__


#include <string>
#include <filesystem>
namespace fs = std::filesystem;

namespace wing {
  void project_from_template(const std::string&, const fs::path&);
}


#endif