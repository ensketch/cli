#pragma once
#include <ensketch/cli/option.hpp>

namespace ensketch::cli {

namespace generic {

template <typename list>
concept option_list =
    generic::named_tuple<list> &&
    for_all(type_list_from<list>(), []<typename type> { return option<type>; });

}  // namespace generic

///
///
template <generic::option... options>
using option_list = named_tuple<static_identifier_list<options::name()...>,
                                std::tuple<options...>>;

}  // namespace ensketch::cli
