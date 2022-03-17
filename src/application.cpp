#include <wing/application.h>
#include <wing/build.h>


wing::application::application(const fs::path& cwd, const std::string& cmd, const std::vector<std::string>& v) {
  cli_command = cmd;
  cli_args = v;
  working_directory = cwd;
  wd_is_project = fs::exists(cwd / "wing.toml");
  if (wd_is_project) {
    // NOTE:
    // this may be changed later, but for now it's always gonna be `/build`
    build_directory = working_directory / "build";
  }
}

void wing::application::load_project() {
  // try to load the project, but if any key directories are missing
  // then unset wd_is_project
  directory_project = wing::load_project(working_directory);
  
}

const bool wing::application::is_project() const { return wd_is_project; }
const fs::path& wing::application::get_working_dir() const { return working_directory; }
const fs::path& wing::application::get_build_dir() const { return build_directory; }
const wing::project_config& wing::application::get_config() const { return directory_project->cfg(); }
const std::vector<std::string>& wing::application::get_args() const { return cli_args; }
const wing::project& wing::application::get_project() const { return *directory_project; }
void wing::application::add_tool(const std::string& name, const fs::path& p) { loaded_tools.emplace(name, p); }
wing::tool& wing::application::get_tool(const std::string& s) { return loaded_tools.at(s); }
