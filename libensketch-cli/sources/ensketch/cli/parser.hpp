#pragma once
#include <ensketch/cli/arg_list.hpp>
#include <ensketch/cli/option_list.hpp>

namespace ensketch::cli {

namespace generic {

///
///
template <typename type, typename option_list_type>
concept parser = generic::option_list<option_list_type> &&
                 requires(type value,
                          czstring current,
                          arg_list& args,
                          option_list_type& options) {
                   { value.parse(current, args, options) } -> same_as<void>;
                 };

}  // namespace generic

}  // namespace ensketch::cli
