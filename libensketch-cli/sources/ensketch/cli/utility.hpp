#pragma once
#include <ensketch/xstd/named_tuple.hpp>

namespace ensketch::cli {

using namespace std;
// using namespace xstd;

using xstd::czstring;
using xstd::static_zstring;
using xstd::static_zstring_list;
using xstd::type_list;
using xstd::type_list_from;

using xstd::named_tuple;
using xstd::static_identifier_list;

namespace generic {
using namespace xstd::generic;
}  // namespace generic

namespace meta = xstd::meta;

}  // namespace ensketch::cli
