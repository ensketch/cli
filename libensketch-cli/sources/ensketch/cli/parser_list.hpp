#pragma once

namespace ensketch::cli {

namespace generic {

template <typename list>
concept parser_list =
    generic::named_tuple<list> &&
    for_all(type_list_from<list>(), []<typename type> { return parser<type>; });

}

}  // namespace ensketch::cli
