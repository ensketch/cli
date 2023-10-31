#pragma once
#include <ensketch/cli/utility.hpp>

namespace ensketch::cli {

namespace generic {

/// This concepts checks whether a given type
/// fulfills the requirements of a program option.
///
template <typename type>
concept option = requires {
  { type::name() } -> convertible_to<czstring>;
  { type::description() } -> convertible_to<czstring>;
};

}  // namespace generic

}  // namespace ensketch::cli
