#pragma once
#include <ensketch/cli/arg_list.hpp>
#include <ensketch/cli/option_list.hpp>
#include <ensketch/cli/parser_error.hpp>
#include <ensketch/cli/parser_list.hpp>
//
#include <ensketch/xstd/static_radix_tree.hpp>

namespace ensketch::cli {

constexpr void parse(
    arg_list args,
    generic::option_list auto& options,
    generic::parser_list<meta::reduction<decltype(options)>> auto& parsers) {
  // Generate a static radix tree of prefixes based on the given prefix parsers.
  //
  constexpr auto prefix_tree = static_radix_tree_from(parsers.names);

  // Assume that the first argument is the name of the program.
  //
  args.pop_front();

  // Process all arguments in the list one after another.
  //
  while (args) {
    const auto current = args.pop_front();

    // Traverse the static prefix tree with the current argument
    // and call the specific parsing routine when a prefix could be matched.
    //
    const auto prefix_matched = traverse(
        prefix_tree, current, [&]<static_zstring prefix>(czstring tail) {
          value<prefix>(parsers).parse(tail, args, options);
        });
    if (prefix_matched) continue;

    // If no prefix could be matched then we failed to parse the given argument.
    //
    args.unpop_front();
    throw parser_error(args, string("Failed to parse '") + args.front() + "'.");
  }
}

}  // namespace ensketch::cli
