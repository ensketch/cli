#pragma once
#include <ensketch/cli/parser_error.hpp>

namespace ensketch::cli {

template <size_t N>
struct position {
  constexpr position(const char (&n)[N], size_t c = 1) : name{n}, count{c} {}
  constexpr position(static_zstring<N> n, size_t c = 1) : name{n}, count{c} {}

  static_zstring<N> name{};
  size_t count{};
};

constexpr size_t undefined = -1;

template <position... p>
struct position_list {};

namespace detail {

template <position p, position... q>
constexpr bool visit(size_t x, auto&& f, size_t last = 0) {
  if constexpr (p.count == undefined) {
    forward<decltype(f)>(f).template operator()<p.name>();
    return true;
  } else {
    last += p.count;
    if (x <= last) {
      forward<decltype(f)>(f).template operator()<p.name>();
      return true;
    }
    if constexpr (sizeof...(q))
      return visit<q...>(x, forward<decltype(f)>(f), last);
    else
      return false;
  }
}

template <typename T>
struct visit_helper;

template <position... p>
struct visit_helper<position_list<p...>> {
  constexpr bool operator()(size_t x, auto&& f) {
    return visit<p...>(x, forward<decltype(f)>(f));
  }
};

}  // namespace detail

template <typename T>
constexpr bool visit(size_t x, auto&& f) {
  return detail::visit_helper<T>{}(x, forward<decltype(f)>(f));
}

namespace generic {

template <typename option_type>
concept positional_parsable =
    requires(option_type& option,
             czstring current,
             arg_list& args,
             size_t position) { option.parse(current, args, position); };

}  // namespace generic

template <typename position_schedule, instance::option_list options>
struct is_valid_positioning : false_type {};
template <instance::option_list options, position... p>
  requires(generic::positional_parsable<
               decay_t<decltype(option<p.name>(options{}))>> &&
           ...)  //
struct is_valid_positioning<position_list<p...>, options> : true_type {};

template <typename position_schedule>
struct positional_parser {
  template <instance::option_list options>
  constexpr void parse(czstring current, arg_list& args, options& opts) {
    ++position;
    const auto visited =
        visit<position_schedule>(position, [&]<static_zstring name> {
          option<name>(opts).parse(current, args, position);
        });
    if (visited) return;
    args.unpop_front();
    throw parser_error(args, string("Failed to parse positional argument '") +
                                 args.front() + "' due to wrong positioning.");
  }

  size_t position = 0;
};

}  // namespace ensketch::cli
