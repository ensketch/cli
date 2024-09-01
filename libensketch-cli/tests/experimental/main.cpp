#include <print>
//
#include <ensketch/cli/args_view.hpp>
#include <ensketch/cli/defaults.hpp>
#include <ensketch/cli/parser/prefix_parser.hpp>

namespace ensketch::cli {

template <typename type, typename init>
concept initializer_for =
    std::invocable<type> && std::same_as<init, std::invoke_result_t<type>>;

template <typename type>
concept generic_option_entry =
    initializer_for<type, typename type::value_type> && requires {
      typename type::type;
      typename type::value_type;
      { type::name() } -> meta::string_instance;
    };

template <typename tag, meta::string str, typename T, initializer_for<T> init>
struct option_entry : init {
  using type = tag;
  using value_type = T;
  using initializer = init;
  using initializer::operator();
  static consteval auto name() noexcept { return str; }
};

template <typename type,
          meta::string name,
          typename value_type,
          initializer_for<value_type> initializer>
constexpr auto auto_option_entry(initializer&& init) {
  return option_entry<type, name, value_type,
                      std::unwrap_ref_decay_t<initializer>>{
      std::forward<initializer>(init)};
}

template <generic_option_entry entry>
consteval auto name(entry) noexcept {
  return entry::name();
}

template <generic_option_entry entry>
consteval auto value_type(entry) noexcept {
  return meta::as_value<typename entry::value_type>;
}

struct flag_option {};
template <meta::string name, bool init = false>
constexpr auto flag() {
  return auto_option_entry<flag_option, name, bool>([] { return init; });
}

struct var_option {};
template <meta::string name, typename type, initializer_for<type> initializer>
constexpr auto var(initializer&& init) {
  return option_entry<var_option, name, type,
                      std::unwrap_ref_decay_t<initializer>>{
      std::forward<initializer>(init)};
}
template <meta::string name, std::default_initializable type>
constexpr auto var() noexcept {
  return var<name, type>([] { return type{}; });
}
template <meta::string name, typename type, auto init>
constexpr auto var() noexcept {
  return var<name, type>([] -> type { return init; });
}

template <generic_option_entry... entries>
  requires(elementwise_unique(meta::value_list<entries::name()...>{}))
struct options_specification : entries... {
  template <typename base>
  constexpr auto forward_as() & noexcept -> base& {
    return *this;
  }
  template <typename base>
  constexpr auto forward_as() const& noexcept -> const base& {
    return *this;
  }
  template <typename base>
  constexpr auto forward_as() && noexcept -> base&& {
    return std::move(*this);
  }
  template <typename base>
  constexpr auto forward_as() const&& noexcept -> const base&& {
    return std::move(*this);
  }

  template <meta::string name>
  constexpr auto entry(this auto&& self) noexcept {
    constexpr auto names = meta::name_list<entries::name()...>{};
    constexpr auto types = meta::type_list<entries...>{};
    constexpr auto bases = zip(names, types);
    // static_assert(meta::map_instance<decltype(bases)>);
    constexpr auto value = at<name>(bases);
    using base = meta::as_type<value>;
    return std::forward<decltype(self)>(self).template forward_as<base>();
  }
};

template <meta::string name>
constexpr auto entry(auto&& options) noexcept {
  return std::forward<decltype(options)>(options).template entry<name>();
}

template <generic_option_entry... entries>
using options_data = named_tuple<meta::name_list<entries::name()...>,
                                 std::tuple<typename entries::value_type...>>;

template <generic_option_entry... entries>
constexpr auto options_data_from(options_specification<entries...>& options) {
  return options_data<entries...>{
      std::invoke(static_cast<entries&>(options))...};
}

template <generic_option_entry... entries>
struct option_list : options_specification<entries...>,
                     options_data<entries...> {
  using spec = options_specification<entries...>;
  using data = options_data<entries...>;

  option_list(entries... args)
      : spec{args...}, data{options_data_from(static_cast<spec&>(*this))} {}

  constexpr auto specification() & noexcept -> spec& { return *this; }
  constexpr auto specification() const& noexcept -> const spec& {
    return *this;
  }
  constexpr auto specification() && noexcept -> spec&& { return *this; }
  constexpr auto specification() const&& noexcept -> const spec&& {
    return *this;
  }
};

template <meta::name_list_instance list>
struct name_parser {
  static consteval auto names() noexcept { return list{}; }

  template <typename context>
  constexpr void operator()(this auto&& self, context& ctx) {
    bool parsed = false;
    const auto prefix_matched = traverse(
        meta::radix_tree_from(names()), ctx.current,
        [&]<meta::string name>(czstring tail) {
          ctx.current = tail;
          parsed = std::invoke(std::forward<decltype(self)>(self),
                               entry<name>(ctx.options.specification()), ctx);
        });
    if (prefix_matched && parsed) return;
    throw std::runtime_error(
        std::format("unknown option name '{}' in '{}'", ctx.current, *ctx.arg));
  }
};

struct name_option_parser {
  template <generic_option_entry option, typename context>
    requires std::same_as<typename option::type, flag_option>
  constexpr bool operator()(this auto&& self, option entry, context& ctx) {
    if (*ctx.current == '\0') {
      std::println("long option flag {}", name(entry));
      return true;
    }
    if (*ctx.current != '=') return false;
    ++ctx.current;
    const auto value =
        std::invoke(std::forward<decltype(self)>(self), value_type(entry), ctx);
    std::println("long option flag {} = {}", name(entry), value);
    // value<name(entry)>(options.data()) = true;
    return true;
  }
};

inline constexpr auto parse_bool = []<typename context>(meta::type_tag<bool>,
                                                        context& ctx) {
  string_view view{ctx.current};
  if (view == "true") return true;
  if (view == "false") return false;
  throw std::runtime_error(std::format("invalid bool value '{}'", view));
};

template <typename option_list>
struct context {
  option_list& options;
  args_view args;
  decltype(args.begin()) arg{};
  czstring current{};
};

constexpr auto default_parser() {
  return prefix_parser{
      add_prefix<"--">(match{
          name_parser<meta::name_list<"help", "verbose">>{},
          name_option_parser{},
          parse_bool,
      }),

      add_prefix<"-">([](auto& ctx) {
        std::println("short option: arg = {}, ptr = {}", *ctx.arg, ctx.current);
      }),

      add_prefix<"", ".">([](auto& ctx) {
        std::println("positional option: arg = {}, ptr = {}", *ctx.arg,
                     ctx.current);
      }),
  };
}

constexpr void parse(int argc, char** argv, auto& options, auto&& parser) {
  auto ctx =
      context{.options = options, .args{static_cast<size_t>(argc), argv}};
  for (ctx.arg = ++ctx.args.begin(); ctx.arg != ctx.args.end(); ++ctx.arg) {
    ctx.current = *ctx.arg;
    parser.parse(ctx);
  }
}

}  // namespace ensketch::cli

int main(int argc, char** argv) {
  namespace cli = ensketch::cli;

  auto options = cli::option_list{
      cli::flag<"help">(),
      cli::flag<"verbose", true>(),
      cli::var<"number", int>(),
  };

  // auto data = cli::options_data_from(options);
  // cli::meta::print_type(data);
  std::println("options size = {}", sizeof(options));
  // std::println("options data size = {}", sizeof(data));
  // std::println("{}", entry<"help">(options).name());

  cli::args_view args{static_cast<size_t>(argc), argv};
  for (size_t i = 0; auto arg : args) std::println("argv[{}] = {}", i++, arg);
  std::println();

  cli::parse(argc, argv, options, cli::default_parser());
}
