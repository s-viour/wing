#pragma once


#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <filesystem>
namespace fs = std::filesystem;


namespace wing {
  /// represents a single file within a project template
  /// this is consumed by the project_template::create to know how to create
  /// the project files
  ///
  struct project_file {
    typedef enum {DIRECTORY, FILE} project_file_type;

    const project_file_type file_type;
    const fs::path location;
    const std::string contents;
  };

  class project_template {
  private:
    const std::string template_name;
    std::vector<project_file> children;

  public:
    explicit project_template(const std::string& n) : template_name(n) {}
    
    void add_file(const project_file&);
    void create(const fs::path&);

    static project_template default_project();
  };

  void project_from_template(const std::string&, const fs::path&);
}
