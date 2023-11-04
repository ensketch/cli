#pragma once
#include <ensketch/cli/parser_error.hpp>

namespace ensketch::cli {

///
///
template <static_zstring n, static_zstring d>
struct flag {
  static consteval auto name() { return n; }
  static consteval auto description() { return d; }

  constexpr void parse() noexcept { value = true; }

  bool value = false;
};

namespace detail {
template <typename type>
struct is_flag : std::false_type {};
template <static_zstring name, static_zstring description>
struct is_flag<flag<name, description>> : std::true_type {};
}  // namespace detail

namespace instance {

/// This concept checks whether a given type
/// is an instance of the 'flag' template.
///
template <typename type>
concept flag = detail::is_flag<type>::value;

}  // namespace instance

}  // namespace ensketch::cli
