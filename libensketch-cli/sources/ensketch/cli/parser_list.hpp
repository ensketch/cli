#pragma once
#include <ensketch/cli/parser_entry.hpp>

namespace ensketch::cli {

template <typename type, typename option_list>
concept parser_list_for = true;

template <typename option_list, parser_entry_for<option_list>... parsers>
struct parser_list : named_tuple<meta::name_list<parsers::prefix()...>,
                                 std::tuple<parsers...>> {
  using base = named_tuple<meta::name_list<parsers::prefix()...>,
                           std::tuple<parsers...>>;
  using base::base;

  constexpr auto data() const noexcept -> const base& { return *this; }
  constexpr auto data() noexcept -> base& { return *this; }
};

template <typename option_list>
constexpr auto parser_list_from(
    parser_entry_for<option_list> auto&&... parsers) {
  return parser_list<option_list,
                     std::unwrap_ref_decay_t<decltype(parsers)>...>{
      std::forward<decltype(parsers)>(parsers)...};
}

}  // namespace ensketch::cli
