#include <wing/tools.h>
#include <cstdlib>
#include <filesystem>
#include <optional>
#include <sstream>
#include <vector>
#include <fmt/core.h>
#include <spdlog/spdlog.h>
#include <reproc++/run.hpp>
namespace fs = std::filesystem;


int wing::tool::execute(const std::vector<std::string>& args) {
  reproc::options opts;
  opts.redirect.parent = true;
  std::vector<std::string> fullargs;
  fullargs.push_back(tool_path);
  fullargs.insert(fullargs.end(), args.begin(), args.end());
  
  int status = -1;
  std::error_code ec;
  std::tie(status, ec) = reproc::run(fullargs, opts);

  return ec ? ec.value() : status;
}

std::optional<fs::path> wing::find_tool(const std::string& name) {
  spdlog::debug("attempting to find tool {}", name);
  auto envpath = std::getenv("PATH");
  if (envpath == NULL) {
    spdlog::warn("PATH variable is empty! wing cannot find external tools");
    return {};
  }

  std::vector<fs::path> paths;
  std::istringstream iss(envpath);
  std::string line;
  while (std::getline(iss, line, ':')) {
    paths.push_back(line);
  }

  for (auto& path : paths) {
    fs::directory_iterator it(path);
    for (auto& entry : it) {
      if (entry.path().filename() == name && entry.exists()) {
        spdlog::debug("found tool [{}]", name);
        return entry;
      }
    }
  }

  spdlog::debug("failed to find tool [{}]", name);
  return {};
}