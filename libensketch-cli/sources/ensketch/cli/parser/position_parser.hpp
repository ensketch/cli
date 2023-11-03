#pragma once
#include <ensketch/cli/parser_error.hpp>

namespace ensketch::cli {

// template <size_t N>
// struct position {
//   constexpr position(const char (&n)[N], size_t c = 1) : name{n}, count{c} {}
//   constexpr position(static_zstring<N> n, size_t c = 1) : name{n}, count{c} {}

//   static_zstring<N> name{};
//   size_t count{};
// };

// constexpr size_t undefined = -1;

namespace generic {

template <typename type>
concept position_scheme = true;
// requires(const type& scheme, size_t position) {
//   { scheme(position) } -> convertible_to<czstring>;
//   { scheme.size() } -> convertible_to<size_t>;
// };

}  // namespace generic

// template <static_zstring... str>
// struct basic_position_scheme : static_zstring_list<str...> {
//   using base_type = static_zstring_list<str...>;

//   constexpr basic_position_scheme() = default;

//   static consteval auto base() { return base_type{}; }
//   static consteval auto name(size_t position) {
//     return element<position>(base());
//   }
// };

template <generic::position_scheme auto position_scheme,
          generic::option_list option_list_type>
struct position_parser {
  using state_type = void (position_parser::*)(czstring,
                                               arg_list&,
                                               option_list_type&);

  constexpr void parse(czstring current,
                       arg_list& args,
                       option_list_type& options) {
    (this->*state)(current, args, options);
  }

  template <size_t position>
  constexpr void parse(czstring current,
                       arg_list& args,
                       option_list_type& options) {
    if constexpr (position < size(position_scheme)) {
      constexpr auto name = element<position>(position_scheme);
      parse(value<name>(options), current, args);
      state = &position_parser::parse<position + 1>;
    } else {
      // args.unpop_front();
      throw parser_error(args, string("position"));
    }
  }

  ///
  ///
  static constexpr void parse(instance::attachment auto& option,
                              czstring current,
                              arg_list& args) {
    option.value = current;
  }

  state_type state = &position_parser::parse<0>;
};

}  // namespace ensketch::cli
