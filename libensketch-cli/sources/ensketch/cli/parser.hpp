#pragma once

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
                   { value.parse(current, arg, options) } -> same_as<void>;
                 };

}  // namespace generic

}  // namespace ensketch::cli
