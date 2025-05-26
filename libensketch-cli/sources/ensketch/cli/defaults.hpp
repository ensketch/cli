#pragma once
#include <charconv>
#include <expected>
#include <filesystem>
//
#include <ensketch/xstd/meta/map.hpp>
#include <ensketch/xstd/meta/radix_tree.hpp>
#include <ensketch/xstd/meta/type_list.hpp>
#include <ensketch/xstd/meta/value_list.hpp>
//
#include <ensketch/xstd/functional.hpp>

namespace ensketch::cli {

// using namespace std;

using std::format;
using std::optional;
using std::string_view;

using namespace xstd;

}  // namespace ensketch::cli
