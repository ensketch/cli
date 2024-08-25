#pragma once
#include <ensketch/cli/option_list.hpp>

namespace ensketch::cli {

template <typename type, typename option_list>
concept parser_entry_for =
    invocable<type, czstring, arg_list&, option_list&> && requires {
      { type::prefix() } -> meta::string_instance;
    };

template <meta::string str, typename parser>
struct parser_entry : parser {
  using parser::operator();
  static constexpr auto prefix() noexcept { return str; }
};

template <meta::string prefix>
constexpr auto parser(auto&& f) {
  return parser_entry<prefix, unwrap_ref_decay_t<decltype(f)>>{
      forward<decltype(f)>(f)};
}

}  // namespace ensketch::cli
