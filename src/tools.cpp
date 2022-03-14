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

using namespace wing;

tool_error::tool_error(const std::error_code& ec) {
  msg = fmt::format("error code: {}", ec.value());
}

const char* tool_error::what() { return msg.c_str(); }

int tool::execute(const std::vector<std::string>& args) {
  reproc::options opts;
  opts.redirect.parent = true;
  std::vector<std::string> fullargs;
  fullargs.push_back(tool_path);
  fullargs.insert(fullargs.end(), args.begin(), args.end());
  
  int status = -1;
  std::error_code ec;
  spdlog::debug("executing tool [{}]", tool_path.string());
  std::tie(status, ec) = reproc::run(fullargs, opts);
  if (ec) {
    throw tool_error(ec);
  }
  return status;
}

std::optional<fs::path> wing::find_tool(const std::string& name) {
  spdlog::debug("attempting to find tool [{}]", name);

  // check the relative (and technically absolute) paths first
  // if we found it there, return it immediately
  auto relative = fs::current_path() / fs::path(name);
  if (fs::exists(relative)) {
    spdlog::debug("found [{}] at relative path", name);
    return relative;
  }

  // otherwise, check the PATH
  auto envpath = std::getenv("PATH");
  if (envpath == NULL) {
    // if PATH is undefined, we can't find anything
    // so return nothing
    spdlog::warn("PATH variable is empty, wing cannot find tools via PATH!");
    return {};
  }

  // extract every path entry for searching
  std::vector<fs::path> paths;
  std::istringstream iss(envpath);
  std::string line;
  while (std::getline(iss, line, ':')) {
    paths.push_back(line);
  }

  // iterate every path entry looking for a file called `name`
  for (auto& path : paths) {
    try {
      fs::directory_iterator it(path);
      for (auto& entry : it) {
        if (entry.path().filename() == name && entry.exists()) {
          spdlog::debug("found tool [{}]", name);
          return entry;
        }
      }
    } catch (fs::filesystem_error& e) {
      spdlog::debug("skipping nonexistent directory [{}]", path.string());
      continue;
    }
  }

  spdlog::warn("failed to find tool [{}]", name);
  return {};
}