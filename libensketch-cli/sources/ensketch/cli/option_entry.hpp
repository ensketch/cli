#pragma once
#include <ensketch/cli/defaults.hpp>

namespace ensketch::cli {

template <typename type, typename init>
concept initializer_for =
    std::invocable<type> && std::same_as<init, std::invoke_result_t<type>>;

template <typename type>
concept generic_option_entry =
    initializer_for<type, typename type::type> && requires {
      { type::name() } -> meta::string_instance;
    };

template <generic_option_entry option_entry>
consteval auto name(option_entry) noexcept {
  return option_entry::name();
}

template <generic_option_entry option_entry>
consteval auto type(option_entry) noexcept {
  return meta::as_value<typename option_entry::type>;
}

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

template <typename type>
concept flag_option =
    matches<type, []<meta::string name, bool init>(flag_entry<name, init>) {
      return meta::as_signature<true>;
    }>;

template <meta::string str, typename t, initializer_for<t> initializer>
struct var_entry : initializer {
  using type = t;
  using initializer::operator();
  static constexpr auto name() noexcept { return str; }
};

template <meta::string name, typename type, initializer_for<type> initializer>
constexpr auto var(initializer&& init) noexcept {
  return var_entry<name, type, std::unwrap_ref_decay_t<initializer>>{
      std::forward<initializer>(init)};
}

template <meta::string name, std::invocable initializer>
constexpr auto var(initializer&& init) noexcept {
  return var<name, std::invoke_result_t<initializer>>(
      forward<initializer>(init));
}

template <meta::string name, std::default_initializable type>
constexpr auto var() noexcept {
  return var<name, type>([] { return type{}; });
}

template <meta::string name, typename type, auto init>
constexpr auto var() noexcept {
  return var<name, type>([] -> type { return init; });
}

template <typename type>
concept var_option =
    matches<type,
            []<meta::string name, typename t, typename init>(
                var_entry<name, t, init>) { return meta::as_signature<true>; }>;

template <meta::string str, typename t, initializer_for<t> initializer>
struct pos_entry : initializer {
  using type = t;
  using initializer::operator();
  static constexpr auto name() noexcept { return str; }
};

template <meta::string name, typename type, initializer_for<type> initializer>
constexpr auto pos(initializer&& init) noexcept {
  return pos_entry<name, type, std::unwrap_ref_decay_t<initializer>>{
      std::forward<initializer>(init)};
}

template <meta::string name, std::invocable initializer>
constexpr auto pos(initializer&& init) noexcept {
  return pos<name, std::invoke_result_t<initializer>>(
      std::forward<initializer>(init));
}

template <meta::string name, std::default_initializable type>
constexpr auto pos() noexcept {
  return pos<name, type>([] { return type{}; });
}

template <meta::string name, typename type, auto init>
constexpr auto pos() noexcept {
  return pos<name, type>([] -> type { return init; });
}

template <typename type>
concept pos_option =
    matches<type,
            []<meta::string name, typename t, typename init>(
                pos_entry<name, t, init>) { return meta::as_signature<true>; }>;

template <meta::string str,
          typename t,
          initializer_for<std::vector<t>> initializer>
struct list_entry : initializer {
  using type = std::vector<t>;
  using initializer::operator();
  static constexpr auto name() noexcept { return str; }
};

template <meta::string name,
          typename type,
          initializer_for<std::vector<type>> initializer>
constexpr auto list(initializer&& init) noexcept {
  return list_entry<name, type, std::unwrap_ref_decay_t<initializer>>{
      std::forward<initializer>(init)};
}

template <meta::string name, std::invocable initializer>
constexpr auto list(initializer&& init) noexcept {
  return list<name, typename std::invoke_result_t<initializer>::value_type>(
      forward<initializer>(init));
}

template <meta::string name, std::default_initializable type>
constexpr auto list() noexcept {
  return list<name, type>([] { return std::vector<type>{}; });
}

template <typename type>
concept list_option = matches<type,
                              []<meta::string name, typename t, typename init>(
                                  list_entry<name, t, init>) {
                                return meta::as_signature<true>;
                              }>;

}  // namespace ensketch::cli
