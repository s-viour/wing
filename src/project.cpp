#include <wing/project.h>
#include <wing/config.h>

namespace fs = std::filesystem;
using namespace wing;

void project::add_include(const fs::path& f) { include_paths.push_back(f); }
void project::add_src(const fs::path& f) { source_files.push_back(f); }
const project_config& project::cfg() const { return project_cfg; }
const fs::path& project::dir() const { return project_dir; }
const std::vector<fs::path>& project::includes() const { return include_paths; }
const std::vector<fs::path>& project::sources() const { return source_files; }
