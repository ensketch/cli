#pragma once
#include <ensketch/cli/parser_kernel.hpp>

namespace ensketch::cli {

template <typename option_list,
          meta::string_list_instance position_scheme,
          typename parser>
struct position_parser : parser {
  using parser::operator();
  using state_type = void (position_parser::*)(czstring,
                                               arg_list&,
                                               option_list&);
  state_type state = &position_parser::parse<0>;

  static consteval auto scheme() noexcept { return position_scheme{}; }

  position_parser(option_list&, position_scheme, auto&& f)
      : parser{std::forward<decltype(f)>(f)} {}

  constexpr void operator()(czstring current,
                            arg_list& args,
                            option_list& options) {
    (this->*state)(current, args, options);
  }

  template <size_t position>
  constexpr void parse(czstring current, arg_list& args, option_list& options) {
    if constexpr (position < size(scheme())) {
      constexpr auto name = element<position>(scheme());
      invoke(*this, current, args, options, value<name>(options.base()));
      state = &position_parser::parse<position + 1>;
    } else {
      args.unpop_front();
      throw parser_error(
          current, args,
          format("unexpected positional argument '{}'", args.front()));
    }
  }
};

template <typename option_list, typename scheme, typename parser>
position_parser(option_list&, scheme, parser&&)
    -> position_parser<option_list, scheme, std::unwrap_ref_decay_t<parser>>;

struct position_option_parser {
  template <meta::string name, typename type, typename init>
  constexpr void operator()(czstring current,
                            arg_list& args,
                            auto& options,
                            pos_entry<name, type, init>) {
    parse(current, args, value<name>(options.data()));
  }

  template <meta::string name, typename type, typename init>
  constexpr void operator()(czstring current,
                            arg_list& args,
                            auto& options,
                            list_entry<name, type, init>) {
    auto& v = value<name>(options.data());
    v.push_back({});
    parse(current, args, v.back());
  }
};

template <meta::string_list_instance position_scheme, typename option_list>
constexpr auto default_position_parser(option_list& options,
                                       position_scheme scheme) {
  return position_parser{options, scheme, position_option_parser{}};
}

}  // namespace ensketch::cli
