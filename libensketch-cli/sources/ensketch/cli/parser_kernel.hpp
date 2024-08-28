#pragma once
#include <ensketch/cli/parser_error.hpp>
#include <ensketch/cli/parser_list.hpp>

namespace ensketch::cli {

template <typename option_list, typename parser_list>
constexpr void parse(arg_list args,
                     option_list& options,
                     parser_list&& parsers) {
  // Assume that the first argument is the name of the program.
  args.pop_front();

  // Process all arguments in the list one after another.
  while (args) {
    const auto current = args.pop_front();

    // Traverse the static prefix tree with the current argument
    // and call the specific parsing routine when a prefix could be matched.
    const auto prefix_matched =
        traverse(meta::radix_tree_from(parser_list::names),  //
                 current, [&]<meta::string prefix>(czstring tail) {
                   invoke(value<prefix>(parsers.data()), tail, args, options);
                 });
    if (prefix_matched) continue;

    // If no prefix could be matched then we failed to parse the given argument.
    args.unpop_front();
    throw parser_error(
        current, args,
        std::format("no matching parser rule for '{}'", args.front()));
  }
}

}  // namespace ensketch::cli
