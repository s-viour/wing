#ifndef __WING_OPERATIONS_H__
#define __WING_OPERATIONS_H__

#include <string>
#include <memory>
#include <unordered_map>
#include <functional>
#include <tl/expected.hpp>
#include <wing/build.h>

namespace wing {
  typedef std::function<wing::application(wing::app_options&&, const wing::project&)>
    setup_fxn;
  typedef std::function<void(const wing::project&, wing::application&)>
    exec_fxn;

  class operation {
  public:
    virtual void run(app_options&&) = 0;
  };

  class build_operation : public operation {
    void run(app_options&&);
  };

  class clean_operation : public operation {
    void run(app_options&&);
  };

  std::unordered_map<std::string, std::unique_ptr<operation>> load_operations();
}

#endif 