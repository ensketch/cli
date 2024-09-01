#pragma once
#include <ensketch/cli/defaults.hpp>

namespace ensketch::cli {

using args_view_base = std::span<const czstring>;

struct args_view : args_view_base {
  using base = args_view_base;
  using size_type = typename args_view_base::size_type;

  // struct iterator {
  //   typename base::iterator arg;
  //   czstring ptr;
  // };

  constexpr args_view() noexcept = default;

  args_view(size_type argc, const czstring* argv) : base{argv, argc} {}
};

}  // namespace ensketch::cli
