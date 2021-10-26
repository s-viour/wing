#include <filesystem>
#include <vector>
#include <wing/application.h>
#include <wing/config.h>
namespace fs = std::filesystem;

namespace wing {
  class project {
  private:
    wing::project_config cfg;
    fs::path build_dir;
    std::vector<fs::path> include_paths;
    std::vector<fs::path> source_files;

  public:
    project() = default;

    void add_include(const fs::path&);
    void add_src(const fs::path&);
    const std::vector<fs::path>& includes();
    const std::vector<fs::path>& sources();
  };

  project create_project(fs::path&);
  void generate_buildfile(fs::path&);
  int build_dir(wing::application&, fs::path&);
}