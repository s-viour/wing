#ifndef __WING_NINJA_H__
#define __WING_NINJA_H__


#include <filesystem>
#include <fstream>
#include <vector>
#include <optional>
namespace fs = std::filesystem;


namespace wing {
  using namespace std;

  struct ninja_variable {
    const string key;
    const string value;
    const unsigned indent = 0;
  };

  struct ninja_rule {
    const string name;
    const string command;
    const optional<string> description = {};
    const optional<string> depfile = {};
    const optional<string> deps = {};
    // some fields omitted because they are not used currently
  };

  struct ninja_build {
    const std::string command;
    const fs::path outputs;
    const vector<fs::path> inputs;
  };

  struct ninja_comment {
    const string comment;
  };
}

using namespace wing;
std::ostream& operator<<(std::ostream&, const ninja_variable&);
std::ostream& operator<<(std::ostream&, const ninja_rule&);
std::ostream& operator<<(std::ostream&, const ninja_build&);
std::ostream& operator<<(std::ostream&, const ninja_comment&);


#endif