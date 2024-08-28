#pragma once
#include <ensketch/cli/parser/value_parser.hpp>
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
    // (this->*state)(current, args, options);
    std::invoke(state, this, current, args, options);
  }

  template <size_t position>
  constexpr void parse(czstring current, arg_list& args, option_list& options) {
    if constexpr (position < size(scheme())) {
      constexpr auto name = element<position>(scheme());
      std::invoke(*this, current, args, options, value<name>(options.base()));
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
  constexpr void operator()(this auto&& self,
                            czstring current,
                            arg_list& args,
                            auto& options,
                            pos_option auto entry) {
    if (!std::forward<decltype(self)>(self).parse_value(
            current, value<name(entry)>(options.data()))) {
      args.unpop_front();
      throw parser_error(
          current, args,
          format("invalid {} value '{}' in positional argument for '{}'",
                 as_string<meta::as_type<type(entry)>>, current, name(entry)));
    }
  }

  constexpr void operator()(this auto&& self,
                            czstring current,
                            arg_list& args,
                            auto& options,
                            list_option auto entry) {
    auto& v = value<name(entry)>(options.data());
    v.push_back({});
    if (!std::forward<decltype(self)>(self).parse_value(current, v.back())) {
      args.unpop_front();
      throw parser_error(
          current, args,
          format("invalid {} value '{}' in positional argument for '{}'",
                 as_string<meta::as_type<type(entry)>>, current, name(entry)));
    }
  }
};

template <meta::string_list_instance position_scheme, typename option_list>
constexpr auto default_position_parser(option_list& options,
                                       position_scheme scheme) {
  return position_parser{
      options, scheme, match{default_value_parser(), position_option_parser{}}};
}

}  // namespace ensketch::cli
