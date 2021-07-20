#ifndef __WING_TOOLS_H__
#define __WING_TOOLS_H__


#include <string>
#include <vector>
#include <filesystem>
namespace fs = std::filesystem;


namespace wing {
  class tool {
  private:
    std::string tool_name;
    fs::path tool_path;
    bool tool_found = false;

  public:
    tool(const std::string&);
    bool good() { return tool_found; }

    int execute(const std::vector<std::string>&);
  };
}


#endif