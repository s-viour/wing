#pragma once


#include <filesystem>
#include <fstream>
#include <vector>
#include <optional>
namespace fs = std::filesystem;


namespace wing {
  using namespace std;

  /// represents a variable in a build.ninja file
  /// this structure should be initialized via designated initializers and then written to an ofstream
  struct ninja_variable {
    const string key;
    const string value;
    const unsigned indent = 0;
  };

  /// represents a rule in a build.ninja file
  ///
  struct ninja_rule {
    const string name;
    const string command;
    const optional<string> description = {};
    const optional<string> depfile = {};
    const optional<string> deps = {};
    // some fields omitted because they are not used currently
  };

  /// represents a build directive in a build.ninja file
  ///
  struct ninja_build {
    const std::string command;
    const fs::path outputs;
    const vector<fs::path> inputs;
  };

  /// represents a comment in a build.ninja file
  ///
  struct ninja_comment {
    const string comment;
  };
}

using namespace wing;
std::ostream& operator<<(std::ostream&, const ninja_variable&);
std::ostream& operator<<(std::ostream&, const ninja_rule&);
std::ostream& operator<<(std::ostream&, const ninja_build&);
std::ostream& operator<<(std::ostream&, const ninja_comment&);
