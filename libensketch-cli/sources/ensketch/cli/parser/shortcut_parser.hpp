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
      : parser{std::forward<decltype(f)>(f)} {}

  constexpr void operator()(czstring current,
                            arg_list& args,
                            option_list& options) {
    auto args_backup = args;
    args_backup.unpop_front();
    for (auto arg = current; *arg; ++arg) {
      const auto valid = for_each_until(scheme(), [&]<typename binding> {
        if (*arg != binding::character) return false;
        std::invoke(*this, args_backup.front(), arg, args, options,
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
    -> shortcut_parser<option_list, scheme, std::unwrap_ref_decay_t<parser>>;

struct shortcut_option_parser {
  constexpr void operator()(this auto&& self,
                            czstring arg,
                            czstring current,
                            arg_list& args,
                            auto& options,
                            var_option auto entry) {
    if (!args) {
      args.unpop_front();
      throw parser_error(
          current, args,
          format("missing argument after '{}' for '{}' in shortcut '{}'",
                 args.front(), *current, arg));
    }
    auto it = args.pop_front();
    if (!std::forward<decltype(self)>(self).parse_value(
            it, value<name(entry)>(options.data()))) {
      args.unpop_front();
      throw parser_error(
          current, args,
          std::format(
              "invalid {} value '{}' for shortcut '{}' of option '{}' in '{}'",
              as_string<meta::as_type<type(entry)>>, it, *current, name(entry),
              arg));
    }
  }

  constexpr void operator()(this auto&& self,
                            czstring arg,
                            czstring current,
                            arg_list& args,
                            auto& options,
                            list_option auto entry) {
    if (!args) {
      args.unpop_front();
      throw parser_error(
          current, args,
          format("missing argument after '{}' for '{}' in shortcut '{}'",
                 args.front(), *current, arg));
    }
    auto it = args.pop_front();
    auto& v = value<name(entry)>(options.data());
    v.push_back({});
    if (!std::forward<decltype(self)>(self).parse_value(it, v.back())) {
      args.unpop_front();
      throw parser_error(
          current, args,
          std::format(
              "invalid {} value '{}' for shortcut '{}' of option '{}' in '{}'",
              as_string<meta::as_type<type(entry)>>, it, *current, name(entry),
              arg));
    }
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
  return shortcut_parser{
      options, bindings,
      match{default_value_parser(), shortcut_option_parser{}}};
}

}  // namespace ensketch::cli
