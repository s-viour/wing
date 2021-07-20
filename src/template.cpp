#include <string>
#include <filesystem>
#include <cstdlib>
#include <fmt/core.h>
#include <wing/template.h>
namespace fs = std::filesystem;


void wing::project_from_template(const std::string& name, const fs::path& dir) {
  auto homedir = std::getenv("HOME");
  auto tmpltdir = fs::path(homedir) / ".wing/templates" / name;
  auto it = fs::directory_iterator(tmpltdir);
  fs::copy(tmpltdir, dir, fs::copy_options::recursive);
}