#pragma once

namespace ensketch::cli {

template <meta::name_list_instance list, typename parser>
struct prefix_entry : parser {
  using parser::operator();

  static consteval auto prefixes() noexcept { return list{}; }

  template <meta::string prefix>
    requires(contained<prefix>(prefixes()))
  constexpr void parse(meta::value_tag<prefix>, auto& ctx) {
    std::invoke(*this, ctx);
  }
};

template <meta::string prefix, meta::string... prefixes>
constexpr auto add_prefix(auto&& f) {
  return prefix_entry<meta::name_list<prefix, prefixes...>,
                      std::unwrap_ref_decay_t<decltype(f)>>{
      std::forward<decltype(f)>(f)};
}

template <typename... parsers>
struct prefix_parser : parsers... {
  using parsers::parse...;
  static consteval auto prefixes() noexcept -> meta::name_list_instance auto {
    return (parsers::prefixes() + ...);
  }

  template <typename context>
  constexpr void parse(this auto&& self, context& ctx) {
    for (auto it = ctx.arg; it != ctx.args.end(); ++it) {
      const auto prefix_matched =
          traverse(meta::radix_tree_from(prefixes()), *it,
                   [&]<meta::string prefix>(czstring tail) {
                     ctx.arg = it;
                     ctx.current = tail;
                     std::forward<decltype(self)>(self).parse(
                         meta::as_signature<prefix>, ctx);
                   });
      if (prefix_matched) continue;
      throw std::runtime_error(
          std::format("no matching parser rule for '{}'", *it));
    }
  }
};

}  // namespace ensketch::cli
