#pragma once
#include <ensketch/cli/default_parser_primitives.hpp>
#include <ensketch/cli/parser/value_parser.hpp>
#include <ensketch/cli/parser_kernel.hpp>

namespace ensketch::cli {

template <typename option_parser>
struct name_parser : option_parser {
  using option_parser::operator();

  template <typename option_list>
  constexpr void operator()(this auto&& self,
                            czstring current,
                            arg_list& args,
                            option_list& options) {
    bool parsed = false;
    const auto prefix_matched =
        traverse(meta::radix_tree_from(option_list::names()),  //
                 current, [&]<meta::string prefix>(czstring tail) {
                   parsed = invoke(forward<decltype(self)>(self), tail, args,
                                   options, value<prefix>(options.base()));
                 });
    if (prefix_matched && parsed) return;

    args.unpop_front();
    throw parser_error(
        current, args,
        format("unknown option name '{}' in '{}'", current, args.front()));
  }
};

inline constexpr auto name_parse_flag = [](this auto&& self,
                                           czstring current,
                                           arg_list& args,
                                           auto& options,
                                           flag_option auto entry) {
  if (*current == '\0') {
    value<name(entry)>(options.data()) = true;
    return true;
  }
  if (*current != '=') return false;
  ++current;

  if (!forward<decltype(self)>(self).parse_value(
          current, value<name(entry)>(options.data()))) {
    args.unpop_front();
    throw parser_error(
        current, args,
        format("invalid {} value '{}' in '{}'",
               as_string<meta::as_type<type(entry)>>, current, args.front()));
  }

  // auto result = invoke(forward<decltype(self)>(self), type(entry), current);
  // if (!result) {
  //   args.unpop_front();
  //   throw parser_error(
  //       current, args,
  //       format("invalid boolean value '{}' in '{}'", current, args.front()));
  // }
  // value<name(entry)>(options.data()) = result.value();

  return true;
};

inline constexpr auto name_parse_var = [](this auto&& self,
                                          czstring current,
                                          arg_list& args,
                                          auto& options,
                                          var_option auto entry) {
  if (*current == '\0') {
    if (args) {
      current = args.pop_front();
      if (!forward<decltype(self)>(self).parse_value(
              current, value<name(entry)>(options.data()))) {
        args.unpop_front();
        throw parser_error(current, args,
                           format("invalid {} value '{}' for '{}'",
                                  as_string<meta::as_type<type(entry)>>,
                                  current, name(entry)));
      }
      // auto result = invoke(forward<decltype(self)>(self), type(entry), current);
      // if (!result) {
      //   args.unpop_front();
      //   throw parser_error(current, args,
      //                      format("invalid {} value '{}' for '{}' in '{}'",
      //                             as_string<meta::as_type<type(entry)>>,
      //                             current, name(entry), args.front()));
      // }
      // value<name(entry)>(options.data()) = move(result.value());
      return true;
    }
    args.unpop_front();
    throw parser_error(current, args,
                       format("missing argument for '{}' after '{}'",
                              name(entry), args.front()));
  }
  if (*current != '=') return false;
  ++current;
  if (!forward<decltype(self)>(self).parse_value(
          current, value<name(entry)>(options.data()))) {
    args.unpop_front();
    throw parser_error(current, args,
                       format("invalid '{}' value '{}' for '{}' in '{}'",
                              as_string<meta::as_type<type(entry)>>, current,
                              name(entry), args.front()));
  }
  // auto result = invoke(forward<decltype(self)>(self), type(entry), current);
  // if (!result) {
  //   args.unpop_front();
  //   throw parser_error(current, args,
  //                      format("invalid {} value '{}' for '{}' in '{}'",
  //                             as_string<meta::as_type<type(entry)>>, current,
  //                             name(entry), args.front()));
  // }
  // value<name(entry)>(options.data()) = move(result.value());
  return true;
};

inline constexpr auto name_parse_pos = [](this auto&& self,
                                          czstring current,
                                          arg_list& args,
                                          auto& options,
                                          pos_option auto entry) {
  if (*current != '\0')
    if (*current != '=') return false;
  args.unpop_front();
  throw parser_error(
      current, args,
      format("forbidden access to purely positional option '{}' in '{}'",
             name(entry), args.front()));
};

inline constexpr auto name_parse_list = [](this auto&& self,
                                           czstring current,
                                           arg_list& args,
                                           auto& options,
                                           list_option auto entry) {
  if (*current != '\0') return false;
  if (!args) {
    args.unpop_front();
    throw parser_error(current, args,
                       format("missing argument for '{}' after '{}'",
                              name(entry), args.front()));
  }
  current = args.pop_front();
  value<name(entry)>(options.data()).push_back({});
  if (!forward<decltype(self)>(self).parse_value(
          current, value<name(entry)>(options.data()).back())) {
    args.unpop_front();
    throw parser_error(current, args,
                       format("invalid '{}' value '{}' for '{}' in '{}'",
                              as_string<meta::as_type<type(entry)>>, current,
                              name(entry), args.front()));
  }
  // auto result = invoke(
  //     forward<decltype(self)>(self),
  //     meta::as_value<typename meta::as_type<type(entry)>::value_type>, current);
  // if (!result)
  //   throw parser_error(current, args,
  //                      format("invalid argument for '{}'", current));
  // value<name(entry)>(options.data()).push_back(move(result.value()));
  return true;
};

template <typename option_list>
constexpr auto default_name_parser(option_list&) noexcept {
  return name_parser{match{default_value_parser(), parse_type, name_parse_flag,
                           name_parse_var, name_parse_pos, name_parse_list}};
}

}  // namespace ensketch::cli
