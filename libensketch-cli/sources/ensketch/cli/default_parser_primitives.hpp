#pragma once
#include <ensketch/cli/defaults.hpp>

namespace ensketch::cli {

struct error_base {
  czstring ptr{};
};
struct invalid_argument : error_base {};
struct out_of_range : error_base {};
struct error_type : variant<invalid_argument, out_of_range> {};
template <typename type>
using result = expected<type, error_type>;
constexpr auto error(auto&& err) noexcept {
  return unexpected(error_type{forward<decltype(err)>(err)});
}

inline constexpr auto convertible_parse =
    []<typename type>(meta::type_tag<type>, czstring str) -> result<type>
  requires convertible_to<czstring, type>
{ return static_cast<type>(str); };

inline constexpr auto bool_parse = [](meta::type_tag<bool>,
                                      czstring str) -> result<bool> {
  string_view view{str};
  if (view == "true") return true;
  if (view == "false") return false;
  return error(invalid_argument{str});
};

inline constexpr auto integral_parse =
    []<integral type>(meta::type_tag<type>, czstring str) -> result<type> {
  type result;
  string_view view{str};
  auto [ptr, ec] = from_chars(begin(view), end(view), result);
  if ((ec == std::errc()) && (ptr == end(view))) return result;
  if (ec == errc::result_out_of_range) return error(out_of_range{ptr});
  return error(invalid_argument{ptr});
};

inline constexpr auto float_parse = []<floating_point type>(
                                        meta::type_tag<type>,
                                        czstring str) -> result<type> {
  type result;
  string_view view{str};
  auto [ptr, ec] = from_chars(begin(view), end(view), result);
  if ((ec == std::errc()) && (ptr == end(view))) return result;
  if (ec == errc::result_out_of_range) return error(out_of_range{ptr});
  return error(invalid_argument{ptr});
};

// template <typename... parsers>
// struct optional_parse : parsers... {
//   using parsers::operator()...;
//   template <typename type>
//   constexpr auto operator()(meta::type_tag<optional<type>>,
//                             czstring str) const noexcept -> result<type> {
//     return (*this)(meta::as_value<type>, str);
//   }
// };

inline constexpr auto optional_parse = []<typename type>(
                                           this auto&& self,
                                           meta::type_tag<optional<type>>,
                                           czstring str) -> result<type> {
  return invoke(forward<decltype(self)>(self), meta::as_value<type>, str);
};

// inline constexpr auto parse_type = cli::optional_parse{
//     cli::convertible_parse,
//     cli::bool_parse,
//     cli::integral_parse,
//     cli::float_parse,
// };

inline constexpr auto parse_type = match{
    convertible_parse, bool_parse, integral_parse, float_parse, optional_parse};

namespace detail {
template <typename type>
struct as_string {
  static constexpr meta::string value = "";
};
template <typename type>
struct as_string<meta::type_tag<type>> {
  static constexpr meta::string value = as_string<type>::value;
};
template <typename type>
struct as_string<std::optional<type>> {
  static constexpr meta::string value = as_string<type>::value;
};
template <>
struct as_string<bool> {
  static constexpr meta::string value = "boolean";
};
template <>
struct as_string<int> {
  static constexpr meta::string value = "integer";
};
template <>
struct as_string<unsigned int> {
  static constexpr meta::string value = "unsigned integer";
};
template <>
struct as_string<float32> {
  static constexpr meta::string value = "floating-point";
};
template <>
struct as_string<float64> {
  static constexpr meta::string value = "floating-point";
};
template <>
struct as_string<std::string_view> {
  static constexpr meta::string value = "string";
};
template <>
struct as_string<std::filesystem::path> {
  static constexpr meta::string value = "path";
};
}  // namespace detail

template <typename type>
inline constexpr auto as_string = detail::as_string<type>::value;

}  // namespace ensketch::cli
