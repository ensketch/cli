#pragma once
#include <ensketch/cli/utility.hpp>

namespace ensketch::cli {

namespace generic {

template <typename option>
concept flag_parse = requires(option& o) {
  { o.parse() } -> same_as<void>;
};

template <typename option>
concept args_parse = requires(option& o, arg_list& args) {
  { o.parse(args) } -> same_as<void>;
};

template <typename option>
concept tail_parse = requires(option& o, czstring tail) {
  { o.parse(tail) } -> same_as<void>;
};

template <typename option>
concept all_parse = requires(option& o, czstring tail, arg_list& args) {
  { o.parse(tail, args) } -> same_as<void>;
};

template <typename option>
concept unique_parse = (flag_parse<option> && !args_parse<option> &&
                        !tail_parse<option> && !all_parse<option>) ||
                       (!flag_parse<option> && args_parse<option> &&
                        !tail_parse<option> && !all_parse<option>) ||
                       (!flag_parse<option> && !args_parse<option> &&
                        tail_parse<option> && !all_parse<option>) ||
                       (!flag_parse<option> && !args_parse<option> &&
                        !tail_parse<option> && all_parse<option>);

/// This concepts checks whether a given type
/// fulfills the requirements of a program option.
///
template <typename type>
concept option = unique_parse<type> && requires {
  { type::name() } -> convertible_to<czstring>;
  { type::description() } -> convertible_to<czstring>;
};

}  // namespace generic

template <generic::option option_type>
constexpr void parse(option_type& option, czstring tail, arg_list& args) {
  if constexpr (generic::flag_parse<option_type>) {
    option.parse();
  } else if constexpr (generic::args_parse<option_type>) {
    option.parse(args);
  } else if constexpr (generic::tail_parse<option_type>) {
    option.parse(tail);
  } else if constexpr (generic::all_parse<option_type>) {
    option.parse(tail, args);
  }
}

}  // namespace ensketch::cli
