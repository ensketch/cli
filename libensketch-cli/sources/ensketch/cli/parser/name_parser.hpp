#pragma once
#include <ensketch/cli/arg_list.hpp>
#include <ensketch/cli/option_list.hpp>
//
#include <ensketch/cli/option/attachment.hpp>
#include <ensketch/cli/option/flag.hpp>
//
#include <ensketch/cli/parser_error.hpp>
#include <ensketch/xstd/static_radix_tree.hpp>

namespace ensketch::cli {

// namespace generic {

// template <typename option_type>
// concept name_parsable = requires(option_type& option,
//                                  czstring current,
//                                  arg_list& args) {
//   { option.parse(current, args) } -> convertible_to<bool>;
// };

// }  // namespace generic

template <generic::option_list option_list_type>
struct name_parser {
  static constexpr void parse(czstring current,
                              arg_list& args,
                              option_list_type& options) {
    // Generate a static radix tree of option names based on the given option list.
    //
    constexpr auto name_tree = static_radix_tree_from(options.names);

    // Try to match the names inside the radix tree as prefixes
    // and call in a successful case the respective parsing routine
    // of the specific option type.
    //
    bool parsed = false;
    const auto prefix_matched =
        traverse(name_tree, current, [&]<static_zstring prefix>(czstring tail) {
          // using option_type = decay_t<decltype(option<prefix>(options))>;
          // if constexpr (generic::name_parsable<option_type>)
          parsed = parse(value<prefix>(options), tail, args);
          // else {
          //   args.unpop_front();
          //   throw parser_error(args, string("Option '") + czstring(prefix) +
          //                                "cannot be set by its name.");
          // }
        });
    if (prefix_matched && parsed) return;

    // If no name could be matched as prefix,
    // we have failed to parse the argument.
    //
    args.unpop_front();
    throw parser_error(args, string("Unknown option '") + args.front() + "'.");
  }

  ///
  ///
  static constexpr bool parse(instance::flag auto& option,
                              czstring current,
                              arg_list& args) {
    if (*current) return false;
    return option.value = true;
  }

  ///
  ///
  static constexpr bool parse(instance::attachment auto& option,
                              czstring current,
                              arg_list& args) {
    if (*current) return false;
    if (args.empty()) {
      args.unpop_front();
      throw parser_error(
          args, string("No given value for option '") + args.front() + "'.");
    }
    // option.value = args.pop_front();
    current = args.pop_front();
    option.parse(current, args);
    return true;
  }
};

}  // namespace ensketch::cli
