#ifndef __WING_APPLICATION_H__
#define __WING_APPLICATION_H__


#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <cxxopts.hpp>
#include <wing/tools.h>
#include <wing/error.h>

namespace fs = std::filesystem;

namespace wing {
  struct app_options {
    const cxxopts::Options& cli_opts;
    const cxxopts::ParseResult& cli_result;
    std::vector<std::string> required_tools;
    fs::path working_directory;

    explicit app_options(const cxxopts::Options& c, const cxxopts::ParseResult& r)
      : cli_opts(c), cli_result(r) {}
    app_options& add_required_tool(const std::string&);
    app_options& set_working_directory(const fs::path&);
    app_options& set_cli_opts(const cxxopts::Options&);
  };

  class application {
  private:
    const cxxopts::Options& cli_opts;
    const cxxopts::ParseResult& cli_result;
    std::unordered_map<std::string, wing::tool> tools;
    fs::path working_directory;
    friend wing::expected<application> create_application(const app_options&);

  public:
    application(const app_options& o)
      : cli_opts(o.cli_opts), cli_result(o.cli_result) {}
    //application(const application&) = delete;
    application(application&&) = default;
    
    tool& get_tool(const std::string&);
    const fs::path& get_working_directory() const;
    const cxxopts::Options& get_cli_options() const;
    const cxxopts::ParseResult& get_cli_results() const;
  };

  wing::expected<application> create_application(const app_options&);
}


#endif