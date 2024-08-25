#pragma once
#include <ensketch/cli/parser_kernel.hpp>

namespace ensketch::cli {

template <char c, meta::string str>
struct char_binding {
  static constexpr auto character = c;
  static constexpr auto name = str;
};

template <char c, meta::string name>
inline constexpr auto bind = char_binding<c, name>{};

consteval auto shortcuts(auto... bindings) noexcept {
  return meta::type_list<decltype(bindings)...>{};
}

template <typename option_list, typename binding_scheme, typename parser>
struct shortcut_parser : parser {
  using parser::operator();

  static consteval auto scheme() noexcept { return binding_scheme{}; }

  shortcut_parser(option_list&, binding_scheme, auto&& f)
      : parser{forward<decltype(f)>(f)} {}

  constexpr void operator()(czstring current,
                            arg_list& args,
                            option_list& options) {
    auto args_backup = args;
    args_backup.unpop_front();
    for (auto arg = current; *arg; ++arg) {
      const auto valid = for_each_until(scheme(), [&]<typename binding> {
        if (*arg != binding::character) return false;
        invoke(*this, args_backup.front(), arg, args, options,
               value<binding::name>(options.base()));
        return true;
      });
      if (valid) continue;
      throw parser_error(current, args_backup,
                         format("unknown short option '{}' in '{}'", *arg,
                                args_backup.front()));
    }
  }
};

template <typename option_list, typename scheme, typename parser>
shortcut_parser(option_list&, scheme, parser&&)
    -> shortcut_parser<option_list, scheme, unwrap_ref_decay_t<parser>>;

struct shortcut_option_parser {
  template <generic_option_entry option_entry>
  constexpr void operator()(czstring arg,
                            czstring current,
                            arg_list& args,
                            auto& options,
                            option_entry& entry) {
    if (!args) {
      args.unpop_front();
      throw parser_error(
          current, args,
          format("missing argument after '{}' for '{}' in shortcut '{}'",
                 args.front(), *current, arg));
    }
    current = args.pop_front();
    parse(current, args, value<option_entry::name()>(options.data()));
  }

  template <meta::string name, bool init>
  constexpr void operator()(czstring arg,
                            czstring current,
                            arg_list& args,
                            auto& options,
                            flag_entry<name, init>) {
    value<name>(options.data()) = true;
  }

  template <meta::string name, typename type, typename init>
  constexpr void operator()(czstring arg,
                            czstring current,
                            arg_list& args,
                            auto& options,
                            pos_entry<name, type, init>) {
    args.unpop_front();
    throw parser_error(
        current, args,
        format("shortcut to purely positional option '{}' in '{}'", current,
               arg));
  }
};

constexpr auto default_shortcut_parser(auto& options, auto bindings) noexcept {
  return shortcut_parser{options, bindings, shortcut_option_parser{}};
}

// template <auto binding_scheme, generic::option_list option_list_type>
// struct shortcut_parser {
//   static consteval auto bindings_lut_from() {
//     using function = void (*)(czstring, arg_list&, option_list_type&);
//     array<function, 256> bindings{};

//     for_each(binding_scheme, [&]<typename binding> {
//       bindings[binding::character] = [](czstring current, arg_list& args,
//                                         option_list_type& options) {
//         ensketch::cli::parse(value<binding::identifier>(options), current,
//                              args);
//       };
//     });

//     return bindings;
//   }

//   static constexpr void parse(czstring current,
//                               arg_list& args,
//                               option_list_type& options) {
//     constexpr auto bindings = bindings_lut_from();

//     auto args_backup = args;
//     for (auto arg = current; *arg; ++arg) {
//       const auto parse_function = bindings[*arg];
//       if (parse_function) {
//         parse_function(arg + 1, args, options);
//         continue;
//       }
//       args_backup.unpop_front();
//       throw parser_error(args_backup, string("Unknown short option '") + *arg +
//                                           "' in '" + args_backup.front() +
//                                           "'.");
//     }
//   }
// };

}  // namespace ensketch::cli
