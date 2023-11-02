#pragma once
#include <ensketch/cli/parser.hpp>

namespace ensketch::cli {

namespace generic {

template <typename list, typename option_list_type>
concept parser_list =
    generic::named_tuple<list> && generic::option_list<option_list_type> &&
    for_all(type_list_from<list>(),
            []<typename type> { return parser<type, option_list_type>; });

}

}  // namespace ensketch::cli
