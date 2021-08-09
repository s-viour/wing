#ifndef __WING_TOOLS_H__
#define __WING_TOOLS_H__


#include <string>
#include <vector>
#include <optional>
#include <filesystem>
namespace fs = std::filesystem;


namespace wing {
  struct tool {
  private:
    std::string tool_name;
    fs::path tool_path;

  public:
    tool(const std::string& name, const fs::path& path) : tool_name(name), tool_path(path) {}
    int execute(const std::vector<std::string>&);
  };

  std::optional<tool> init_tool(const std::string&);
}


#endif