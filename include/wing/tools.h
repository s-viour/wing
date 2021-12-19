#ifndef __WING_TOOLS_H__
#define __WING_TOOLS_H__


#include <string>
#include <vector>
#include <optional>
#include <filesystem>
namespace fs = std::filesystem;


namespace wing {
  class tool_error : public std::exception {
  private:
    std::string msg;
  public:
    tool_error(const std::error_code&);
    const char* what();
  };

  class tool {
  private:
    std::string tool_name;
    fs::path tool_path;

  public:
    tool() = default;
    tool(const std::string& name, const fs::path& path)
      : tool_name(name)
      , tool_path(path)
    {}
    
    int execute(const std::vector<std::string>&);
  };

  std::optional<fs::path> find_tool(const std::string&);
}


#endif