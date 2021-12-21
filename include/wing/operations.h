#ifndef __WING_OPERATIONS_H__
#define __WING_OPERATIONS_H__

#include <string>
#include <memory>
#include <unordered_map>
#include <functional>
#include <wing/build.h>

namespace wing {
  class operation {
  public:
    virtual void run(app_options&&, const std::vector<std::string>& args) = 0;
  };

  std::unordered_map<std::string, std::unique_ptr<operation>> load_operations();
}

#endif 