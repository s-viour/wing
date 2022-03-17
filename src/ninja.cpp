#include <string>
#include <wing/ninja.h>


std::ostream& operator<<(std::ostream& o, const wing::ninja_variable& v) {
  if (v.value.empty()) {
    return o;
  }

  for (unsigned i = 0; i < v.indent; ++i) {
    o << "  ";
  }

  o << v.key << " = " << v.value << '\n';
  return o;
}

std::ostream& operator<<(std::ostream& o, const wing::ninja_rule& r) {
  o << "rule " << r.name << '\n'
    << ninja_variable{.key="command", .value=r.command, .indent = 1};
  if (r.depfile)
    o << ninja_variable{.key="depfile", .value=r.depfile.value(), .indent = 1};
  return o;
}
std::ostream& operator<<(std::ostream& o, const wing::ninja_build& b) {
  o << "build ";
  o << b.outputs.string() << ": ";
  o << b.command << ' ';
  for (auto& e : b.inputs) {
    o << e.string() << ' ';
  }
  o << '\n';
  return o;
}