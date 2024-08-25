#pragma once
// #include <ensketch/cli/arg_list.hpp>
// #include <ensketch/cli/option_list.hpp>
// //
// #include <ensketch/cli/option/attachment.hpp>
// #include <ensketch/cli/option/flag.hpp>
// //
#include <ensketch/cli/parser_kernel.hpp>
// //
// #include <ensketch/xstd/meta/radix_tree.hpp>

namespace ensketch::cli {

struct name_option_parser {
  template <generic_option_entry option_entry>
  constexpr void operator()(czstring current,
                            arg_list& args,
                            auto& options,
                            option_entry& entry) {
    if (*current == '\0') {
      if (args) {
        current = args.pop_front();
        parse(current, args, value<option_entry::name()>(options.data()));
        return;
      }
      args.unpop_front();
      throw parser_error(current, args,
                         format("missing argument for '{}' after '{}'",
                                option_entry::name(), args.front()));
    }

    if (*current == '=') {
      parse(current + 1, args, value<option_entry::name()>(options.data()));
      return;
    }

    args.unpop_front();
    throw std::runtime_error(std::format("unknown option name '{}{}' in '{}'",
                                         option_entry::name(), current,
                                         args.front()));
  }

  template <meta::string name, bool init>
  constexpr void operator()(czstring current,
                            arg_list& args,
                            auto& options,
                            flag_entry<name, init>) {
    if (*current == '\0') {
      value<name>(options.data()) = true;
      return;
    }

    if (*current == '=') {
      parse(current + 1, args, value<name>(options.data()));
      return;
    }

    args.unpop_front();
    throw parser_error(current, args,
                       format("unknown option name '{}{}' in '{}'", name,
                              current, args.front()));
  }

  template <meta::string name, typename type, typename init>
  constexpr void operator()(czstring current,
                            arg_list& args,
                            auto& options,
                            list_entry<name, type, init>) {
    if (*current == '\0') {
      if (args) {
        current = args.pop_front();
        auto& v = value<name>(options.data());
        v.push_back({});
        parse(current, args, v.back());
        return;
      }
      args.unpop_front();
      throw parser_error(
          current, args,
          format("missing argument for '{}' after '{}'", name, args.front()));
    }

    // if (*current == '=') {
    //   auto& v = value<name>(options.data());
    //   v.push_back({});
    //   parse(current + 1, args, v.back());
    //   return;
    // }

    args.unpop_front();
    throw parser_error(current, args,
                       format("unknown option name '{}{}' in '{}'", name,
                              current, args.front()));
  }

  template <meta::string name, typename type, typename init>
  constexpr void operator()(czstring current,
                            arg_list& args,
                            auto& options,
                            pos_entry<name, type, init>) {
    args.unpop_front();
    throw parser_error(
        current, args,
        format("name access to purely positional option '{}' in '{}'", name,
               args.front()));
  }
};

template <typename option_parser>
struct name_parser : option_parser {
  using option_parser::operator();

  template <typename option_list>
  constexpr void operator()(czstring current,
                            arg_list& args,
                            option_list& options) {
    const auto prefix_matched = traverse(
        meta::radix_tree_from(option_list::names()),  //
        current, [&, this]<meta::string prefix>(czstring tail) {
          invoke(*this, tail, args, options, value<prefix>(options.base()));
        });
    if (prefix_matched) return;

    args.unpop_front();
    throw parser_error(
        current, args,
        format("unknown option name '{}' in '{}'", current, args.front()));
  }
};

constexpr auto default_name_parser() noexcept {
  return name_parser{name_option_parser{}};
}

}  // namespace ensketch::cli
