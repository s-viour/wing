#include <fstream>
#include <toml++/toml.h>
#include <wing/config.h>

using namespace wing;


wing::project_config wing::load_config(const fs::path& path) {
  auto table = toml::parse_file(path.string());
  auto name = table["name"].value<std::string>();
  //auto project_type = table["type"].value<std::string>();
  //auto vcpkg_dir = table["vcpkg_dir"].value<std::string>();
  auto table_dependencies = table["dependencies"].as_array();

  if (!name || !table_dependencies) {
    throw wing::config_error("missing required values or has malformed values");
  }

  std::vector<dependency> dependencies;
  for (auto& table_dep : *table_dependencies) {
    auto dep = dependency {
      .name = table_dep.value<std::string>().value(),
    };
    dependencies.push_back(dep);
  }

  return project_config {
    .name = name.value(),
    .dependencies = dependencies
  };
}