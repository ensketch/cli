#pragma once
#include <ensketch/cli/parser/name_parser.hpp>
#include <ensketch/cli/parser/position_parser.hpp>
#include <ensketch/cli/parser/shortcut_parser.hpp>

namespace ensketch::cli {

template <typename option_list>
constexpr void parse(arg_list args,
                     option_list& options,
                     auto bindings,
                     auto position_scheme) {
  parse(args, options,
        parser_list_from<option_list>(
            parser<"--">(default_name_parser()),
            parser<"-">(default_shortcut_parser(options, bindings)),
            parser<"">(default_position_parser(options, position_scheme))));
}

}  // namespace ensketch::cli
