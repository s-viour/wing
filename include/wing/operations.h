#ifndef __WING_OPERATIONS_H__
#define __WING_OPERATIONS_H__

#include <string>
#include <memory>
#include <unordered_map>
#include <functional>
#include <wing/build.h>

namespace wing {
  typedef std::function<int(wing::application&)> operation_function;

  /// represents an operation (essentially, just a command) and its set of requirements to run
  /// this structure may be updated as necessary
  /// fields:
  ///   * requires_project - whether or not this operation needs to be executed within a project directory
  ///   * requires_loaded_project - whether or not this operation needs a fully-loaded project structure
  ///   * required_tools - list of external tools this operation needs
  ///   * run - pointer to the function that runs this operation
  struct operation {
    bool requires_project;
    bool requires_loaded_project;
    std::vector<std::string> required_tools;
    operation_function run;
  };

  /// returns all operation structures in an unordered map
  /// i think this is the best representation of the operations, since they're statically 
  /// defined as functions in `operations.cpp`. all this function does is setup their requirements
  /// and map them to their names
  std::unordered_map<std::string, operation> load_operations();
}

#endif 