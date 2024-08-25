#pragma once
#include <ensketch/cli/option_entry.hpp>

namespace ensketch::cli {

template <typename type>
concept generic_option_list = true;

template <generic_option_entry... options>
using option_list_base =
    named_tuple<meta::name_list<options::name()...>, std::tuple<options...>>;

template <generic_option_entry... options>
using option_list_data = named_tuple<meta::name_list<options::name()...>,
                                     std::tuple<typename options::type...>>;

template <generic_option_entry... options>
struct option_list {
  using base_type = option_list_base<options...>;
  using data_type = option_list_data<options...>;

  static consteval auto names() noexcept { return data_type::names; }

  constexpr option_list(auto&&... opts)
    requires(sizeof...(opts) == sizeof...(options))
      : option_list(meta::index_list_from_iota<sizeof...(opts)>(),
                    forward<decltype(opts)>(opts)...) {}

  template <size_t... indices>
  constexpr option_list(meta::index_list<indices...>, auto&&... opts)
      : _init{forward<decltype(opts)>(opts)...},
        _data{invoke(get<indices>(_init))...} {}

  constexpr auto data() const noexcept -> const data_type& { return _data; }
  constexpr auto data() noexcept -> data_type& { return _data; }

  constexpr auto base() const noexcept -> const base_type& { return _init; }
  constexpr auto base() noexcept -> base_type& { return _init; }

  base_type _init;
  data_type _data;
};

template <typename... options>
option_list(options&&...) -> option_list<unwrap_ref_decay_t<options>...>;

}  // namespace ensketch::cli
