#pragma once
#include <ensketch/cli/parser_error.hpp>

namespace ensketch::cli {

// namespace generic {

// template <typename option_type>
// concept shortcut_parsable = requires(option_type& option, arg_list& args) {
//   { option_type::short_name() } -> same_as<char>;
//   option.parse(args);
// };

// }  // namespace generic

template <char c, static_zstring name>
struct char_binding {
  static constexpr auto character = c;
  static constexpr auto identifier = name;
};

// template <instance::char_binding... bindings>
// using char_binding_list = type_list<bindings...>;

///
///
template <auto binding_scheme, generic::option_list option_list_type>
struct shortcut_parser {
  static consteval auto bindings_lut_from() {
    using function = void (*)(czstring, arg_list&, option_list_type&);
    array<function, 256> bindings{};

    for_each(binding_scheme, [&]<typename binding> {
      bindings[binding::character] = [](czstring current, arg_list& args,
                                        option_list_type& options) {
        parse(value<binding::identifier>(options), current, args);
      };
    });

    return bindings;
  }

  ///
  ///
  static constexpr void parse(czstring current,
                              arg_list& args,
                              option_list_type& options) {
    constexpr auto bindings = bindings_lut_from();

    auto args_backup = args;
    for (auto arg = current; *arg; ++arg) {
      const auto parse_function = bindings[*arg];
      if (parse_function) {
        parse_function(arg + 1, args, options);
        continue;
      }
      args_backup.unpop_front();
      throw parser_error(args_backup, string("Unknown short option '") + *arg +
                                          "' in '" + args_backup.front() +
                                          "'.");
    }
  }

  ///
  ///
  static constexpr void parse(instance::flag auto& option,
                              czstring current,
                              arg_list& args) {
    option.value = true;
  }

  ///
  ///
  static constexpr void parse(instance::attachment auto& option,
                              czstring current,
                              arg_list& args) {
    if (*current)
      throw parser_error(args,
                         "Attachments in shortcuts must be placed at the end.");

    if (args.empty()) {
      args.unpop_front();
      throw parser_error(
          args, string("No given value for option '") + args.front() + "'.");
    }

    current = args.pop_front();
    option.parse(current, args);
  }

  // array<size_t, 256> bindings{};
};

}  // namespace ensketch::cli
