#pragma once


#include <string>
#include <vector>
#include <optional>
#include <filesystem>
#include <fmt/core.h>
namespace fs = std::filesystem;


namespace wing {
  /// error thrown by tools when they fail to execute or perform their job correctly
  ///
  class tool_error : public std::runtime_error {
  private:
    std::error_code ec;
  public:
    tool_error() : std::runtime_error("tool error") {}
    tool_error(const std::error_code& ec)
      : std::runtime_error(fmt::format("error code: {}", ec.value())) {}
  };

  /// represents any external executable tool invoked by wing
  /// this is basically just a lightweight wrapper over-top of reproc++
  ///
  class tool {
  private:
    fs::path tool_path;

  public:
    tool() = default;
    tool(const fs::path& path) : tool_path(path) {}
    
    int execute(const std::vector<std::string>&);
  };

  /// attempts to find an executable file by the given name
  /// currently, this function searches the current directory and the PATH
  /// returns an empty optional if the tool was not found
  ///
  std::optional<fs::path> find_tool(const std::string&);
}
