#include <fmt/core.h>
#include <wing/help.h>

const std::string wing::desc_str() {
  return "experimental build system";
}

const std::string wing::usage_str() {
  return "USAGE:\n"
    "  wing [OPTIONS] [COMMAND]\n";
}

const std::string wing::help_str() {
  return fmt::format(
    // base help string
    "{}\n"
    "{}"
    "OPTIONS:\n"
    "  {:<30}{}\n"
    "  {:<30}{}\n"
    "COMMANDS:\n"
    "  {:<30}{}\n"
    "  {:<30}{}\n"
    "  {:<30}{}\n"
    "  {:<30}{}\n"
    "  {:<30}{}\n",

    desc_str(),
    usage_str(),
    
    // option descriptions
    "-h, --help", "show help and exit",     // -h --help
    "-d, --debug", "turn on debug logging", // -d --debug
    
    // command descriptions
    "new", "generate a new project", // new
    "build", "build the project in the current directory", // build
    "clean", "clean build files", // clean
    "run", "build, then run the executable", // run
    "install", "install all dependencies" // install
  );
}
