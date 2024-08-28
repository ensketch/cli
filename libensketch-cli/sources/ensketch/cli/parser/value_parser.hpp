#pragma once
#include <ensketch/cli/defaults.hpp>

namespace ensketch::cli {

template <typename... rules>
struct value_parser : rules... {
  using rules::operator()...;
  template <typename type>
  constexpr bool parse_value(czstring& it, type& value) {
    return std::invoke(*this, it, value);
  }
};

inline constexpr auto parse_assignable =
    [](czstring& it, std::assignable_from<string_view> auto&& value) {
      string_view view{it};
      value = it;
      it = end(view);
      return true;
    };

inline constexpr auto parse_bool = [](czstring& it, bool& value) {
  string_view view{it};
  if (view == "true") {
    value = true;
    it = end(view);
    return true;
  }
  if (view == "false") {
    value = false;
    it = end(view);
    return true;
  }
  return false;
};

inline constexpr auto parse_number =
    []<typename type>(czstring& it, type& value)
  requires std::is_arithmetic_v<type> &&
           (!std::same_as<bool, std::decay_t<decltype(value)>>)
{
  string_view view{it};
  auto [ptr, ec] = std::from_chars(begin(view), end(view), value);
  if ((ec == std::errc()) && (ptr == end(view))) {
    it = end(view);
    return true;
  }
  return false;
};

// inline constexpr auto parse_float = [](czstring& it,
//                                        floating_point auto& value) {
//   string_view view{it};
//   auto [ptr, ec] = from_chars(begin(view), end(view), value);
//   if ((ec == std::errc()) && (ptr == end(view))) {
//     it = end(view);
//     return true;
//   }
//   return false;
// };

inline constexpr auto parse_optional =
    []<typename type>(this auto&& self, czstring& it, optional<type>& value) {
      value = std::make_optional<type>();
      return std::forward<decltype(self)>(self).parse_value(it, value.value());
    };

constexpr auto default_value_parser() {
  return value_parser{
      parse_assignable,
      parse_bool,
      parse_number,
      parse_optional,
  };
}

}  // namespace ensketch::cli
