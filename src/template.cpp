#include <string>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <fmt/core.h>
#include <spdlog/spdlog.h>
#include <wing/template.h>
namespace fs = std::filesystem;
using namespace wing;


void project_template::add_file(const project_file& file) {
  children.push_back(file);
}

void project_template::create(const fs::path& root) {
  fs::create_directory(root);
  for (auto& file : children) {
    if (file.file_type == project_file::DIRECTORY) {
      fs::create_directory(root / file.location);
    } else {
      std::ofstream created((root / file.location).string());
      if (file.contents == "") {
        spdlog::warn("empty file {} created. this may be an error!", file.location.string());
      }
      created << file.contents;
    }
  }
}

project_template project_template::default_project() {
  static constexpr auto README = R"(
# project\n
your project description here!
)";

  static constexpr auto MAIN_CPP = R"(
#include <project/sayhello.h>

int main() {
  sayhello("world");
};
)";

  constexpr static auto SAYHELLO_HDR = R"(
#include <string>


void sayhello(const std::string&);
)";
  
  constexpr static auto SAYHELLO_CPP = R"(
#include <iostream>
#include <project/sayhello.h>

void sayhello(const std::string& name) {
  std::cout << "hello, " << name << "!\n";
}
)";

  constexpr static auto MANIFEST = R"(
name = "project"
dependencies = []
)";

  project_template t("wing-default");
  t.add_file(project_file{
    .file_type = project_file::DIRECTORY,
    .location = fs::path("src")
  });
  t.add_file(project_file{
    .file_type = project_file::DIRECTORY,
    .location = fs::path("include")
  });
  t.add_file(project_file{
    .file_type = project_file::DIRECTORY,
    .location = fs::path("include/project")
  });
  t.add_file(project_file{
    .file_type = project_file::DIRECTORY,
    .location = fs::path("src")
  });
  t.add_file(project_file{
    .file_type = project_file::FILE,
    .location = fs::path("README.md"),
    .contents = README
  });
  t.add_file(project_file{
    .file_type = project_file::FILE,
    .location = fs::path("include/project/sayhello.h"),
    .contents = SAYHELLO_HDR
  });
  t.add_file(project_file{
    .file_type = project_file::FILE,
    .location = fs::path("src/sayhello.cpp"),
    .contents = SAYHELLO_CPP
  });
  t.add_file(project_file{
    .file_type = project_file::FILE,
    .location = fs::path("src/main.cpp"),
    .contents = MAIN_CPP
  });
  t.add_file(project_file{
    .file_type = project_file::FILE,
    .location = fs::path("wing.toml"),
    .contents = MANIFEST
  });

  return t;
}

void wing::project_from_template(const std::string& name, const fs::path& dir) {
  auto homedir = std::getenv("HOME");
  auto tmpltdir = fs::path(homedir) / ".wing/templates" / name;
  auto it = fs::directory_iterator(tmpltdir);
  fs::copy(tmpltdir, dir, fs::copy_options::recursive);
}
