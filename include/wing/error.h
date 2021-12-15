#ifndef __WING_ERROR_H__
#define __WING_ERROR_H__

#include <string>
#include <vector>
#include <fmt/format.h>
#include <tl/expected.hpp>


namespace wing {
  class error {
  private:
    std::string error_message;

  public:
    error() = default;
    error(const std::string& s) : error_message(s) {}
    
    const std::string& msg() const { return this->error_message; }
  };

  template<typename T>
  using expected = tl::expected<T, error>;
  using unexpected = tl::unexpected<error>;
}

template<>
struct fmt::formatter<wing::error> : formatter<string_view> {
  template<typename FormatContext>
  auto format(const wing::error& e, FormatContext& ctx) {
    return formatter<string_view>::format(e.msg(), ctx);
  }
};

#endif