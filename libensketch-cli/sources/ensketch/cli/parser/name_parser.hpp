#pragma once
#include <ensketch/cli/parser_error.hpp>

namespace ensketch::cli {

// namespace generic {

// template <typename option_type>
// concept name_parsable = requires(option_type& option,
//                                  czstring current,
//                                  arg_list& args) {
//   { option.parse(current, args) } -> convertible_to<bool>;
// };

// }  // namespace generic

struct name_parser {
  template <instance::option_list options>
  static constexpr void parse(czstring current, arg_list& args, options& opts) {
    // Generate a static radix tree of option names based on the given option list.
    using names = name_tree<options>;
    using namespace static_radix_tree;
    // Try to match the names inside the radix tree as prefixes
    // and call in a successful case the respective parsing routine
    // of the specific option type.
    bool parsed = false;
    const auto prefix_matched =
        traverse<names>(current, [&]<static_zstring prefix>(czstring tail) {
          using option_type = decay_t<decltype(option<prefix>(opts))>;
          if constexpr (generic::name_parsable<option_type>)
            parsed = option<prefix>(opts).parse(tail, args);
          else {
            args.unpop_front();
            throw parser_error(args, string("Option '") + czstring(prefix) +
                                         "cannot be set by its name.");
          }
        });
    if (prefix_matched && parsed) return;
    // If no name could be matched as prefix,
    // we have failed to parse the argument.
    args.unpop_front();
    throw parser_error(args, string("Unknown option '") + args.front() + "'.");
  }
};

}  // namespace ensketch::cli
