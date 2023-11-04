#pragma once
#include <sstream>
#include <string>
//
#include <ensketch/cli/parser_error.hpp>

namespace ensketch::cli {

///
///
template <static_zstring n,
          static_zstring d,
          typename type = czstring,
          type default_value = type{}>
struct attachment {
  static consteval auto name() { return n; }
  static consteval auto description() { return d; }

  constexpr void parse(arg_list& args) {
    if (args.empty()) {
      args.unpop_front();
      throw parser_error(
          args, string("No given value for option '") + args.front() + "'.");
    }
    if constexpr (meta::equal<type, czstring>) {
      value = args.pop_front();
    } else {
      stringstream input{args.pop_front()};
      input >> value;
      if (!input) throw parser_error(args, "Failed to parse argument to type.");
    }
  }

  type value = default_value;
};

namespace detail {
template <typename type>
struct is_attachment : std::false_type {};
template <static_zstring name,
          static_zstring description,
          typename type,
          type default_value>
struct is_attachment<attachment<name, description, type, default_value>>
    : std::true_type {};
}  // namespace detail

namespace instance {

/// This concept checks whether a given type
/// is an instance of the 'attachment' template.
///
template <typename type>
concept attachment = detail::is_attachment<type>::value;

}  // namespace instance

}  // namespace ensketch::cli
