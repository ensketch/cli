#pragma once
#include <ensketch/cli/defaults.hpp>

namespace ensketch::cli {

template <typename type, typename init>
concept initializer_for =
    invocable<type> && same_as<init, invoke_result_t<type>>;

template <typename type>
concept generic_option_entry =
    initializer_for<type, typename type::type> && requires {
      { type::name() } -> meta::string_instance;
    };

template <meta::string str, bool init = false>
struct flag_entry {
  using type = bool;
  static constexpr auto name() noexcept { return str; }
  static constexpr auto operator()() noexcept { return init; }
};

template <meta::string name, bool initial = false>
constexpr auto flag() noexcept {
  return flag_entry<name, initial>{};
}

template <meta::string str, typename t, initializer_for<t> initializer>
struct var_entry : initializer {
  using type = t;
  using initializer::operator();
  static constexpr auto name() noexcept { return str; }
};

template <meta::string name, typename type, initializer_for<type> initializer>
constexpr auto var(initializer&& init) noexcept {
  return var_entry<name, type, unwrap_ref_decay_t<initializer>>{
      forward<initializer>(init)};
}

template <meta::string name, invocable initializer>
constexpr auto var(initializer&& init) noexcept {
  return var<name, invoke_result_t<initializer>>(forward<initializer>(init));
}

template <meta::string name, default_initializable type>
constexpr auto var() noexcept {
  return var<name, type>([] { return type{}; });
}

template <meta::string name, typename type, auto init>
constexpr auto var() noexcept {
  return var<name, type>([] -> type { return init; });
}

template <meta::string str, typename t, initializer_for<t> initializer>
struct pos_entry : initializer {
  using type = t;
  using initializer::operator();
  static constexpr auto name() noexcept { return str; }
};

template <meta::string name, typename type, initializer_for<type> initializer>
constexpr auto pos(initializer&& init) noexcept {
  return pos_entry<name, type, unwrap_ref_decay_t<initializer>>{
      forward<initializer>(init)};
}

template <meta::string name, invocable initializer>
constexpr auto pos(initializer&& init) noexcept {
  return pos<name, invoke_result_t<initializer>>(forward<initializer>(init));
}

template <meta::string name, default_initializable type>
constexpr auto pos() noexcept {
  return pos<name, type>([] { return type{}; });
}

template <meta::string name, typename type, auto init>
constexpr auto pos() noexcept {
  return pos<name, type>([] -> type { return init; });
}

template <meta::string str, typename t, initializer_for<vector<t>> initializer>
struct list_entry : initializer {
  using type = vector<t>;
  using initializer::operator();
  static constexpr auto name() noexcept { return str; }
};

template <meta::string name,
          typename type,
          initializer_for<vector<type>> initializer>
constexpr auto list(initializer&& init) noexcept {
  return list_entry<name, type, unwrap_ref_decay_t<initializer>>{
      forward<initializer>(init)};
}

template <meta::string name, invocable initializer>
constexpr auto list(initializer&& init) noexcept {
  return list<name, typename invoke_result_t<initializer>::value_type>(
      forward<initializer>(init));
}

template <meta::string name, default_initializable type>
constexpr auto list() noexcept {
  return list<name, type>([] { return vector<type>{}; });
}

}  // namespace ensketch::cli
