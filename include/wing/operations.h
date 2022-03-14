#ifndef __WING_OPERATIONS_H__
#define __WING_OPERATIONS_H__

#include <string>
#include <memory>
#include <unordered_map>
#include <functional>
#include <wing/build.h>

namespace wing {
  typedef std::function<void(wing::application&)> operation_function;
  struct operation {
    bool requires_project;
    std::vector<std::string> required_tools;
    operation_function run;
  };

  std::unordered_map<std::string, operation> load_operations();
}

#endif 