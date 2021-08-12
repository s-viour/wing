#ifndef __WING_TEMPLATE_H__
#define __WING_TEMPLATE_H__


#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <filesystem>
namespace fs = std::filesystem;


namespace wing {
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


#endif