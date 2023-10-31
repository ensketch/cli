#pragma once

namespace ensketch::cli {

constexpr void parse(arg_list args, option_list& opts, parser_list parsers) {
  // Generate a static radix tree of prefixes based on the given prefix parsers.
  using prefixes = static_name_tree<parser_list>;
  using namespace static_radix_tree;
  // Assume that the first argument is the name of the program.
  args.pop_front();
  // Process all arguments in the list one after another.
  while (!args.empty()) {
    const auto current = args.pop_front();
    // Traverse the static prefix tree with the current argument
    // and call the specific parsing routine by template parameters
    // when a prefix could be matched.
    const auto prefix_matched =
        traverse<prefixes>(current, [&]<static_zstring prefix>(czstring tail) {
          get_by_name<prefix>(parsers).parse(tail, args, opts);
        });
    if (prefix_matched) continue;
    // If no prefix could be matched then we failed to parse the given argument.
    args.unpop_front();
    throw parser_error(args, string("Failed to parse '") + args.front() + "'.");
  }
}

}  // namespace ensketch::cli
