#include <list>
#include <print>
#include <thread>
//
#include <ensketch/cli/arg_list.hpp>
#include <ensketch/cli/default_parser.hpp>
#include <ensketch/cli/option_entry.hpp>
#include <ensketch/cli/option_list.hpp>
#include <ensketch/cli/parser/name_parser.hpp>
#include <ensketch/cli/parser/position_parser.hpp>
#include <ensketch/cli/parser/shortcut_parser.hpp>
#include <ensketch/cli/parser_entry.hpp>
#include <ensketch/cli/parser_kernel.hpp>
#include <ensketch/cli/parser_list.hpp>

// static_assert(cli::generic_option_entry<decltype(cli::flag<"help">())>);
// static_assert(
//     cli::generic_option_entry<decltype(cli::flag<"verbose", true>())>);
// static_assert(cli::generic_option_entry<
//               decltype(cli::data<"input", cli::czstring, nullptr>())>);
// static_assert(
//     cli::generic_option_entry<decltype(cli::data<"number", int, -1>())>);
// static_assert(
//     cli::generic_option_entry<decltype(cli::data<"string", std::string_view>(
//         [] { return std::string_view{"default"}; }))>);
// static_assert(
//     cli::generic_option_entry<decltype(cli::data<"crazy", std::thread>([] {
//       return std::thread{};
//     }))>);

template <typename type>
struct std::formatter<std::vector<type>, char> {
  template <class context>
  constexpr auto parse(context& ctx) -> context::iterator {
    return ctx.begin();
  }
  template <class context>
  auto format(const vector<type>& data,
              context& ctx) const -> context::iterator {
    string str{};
    if (!data.empty()) {
      str += std::format("{}", data.front());
      for (size_t i = 1; i < data.size(); ++i)
        str += std::format(", {}", data[i]);
    }
    return ranges::copy(std::format("[{}]", str), ctx.out()).out;
  }
};

template <typename type>
struct std::formatter<std::optional<type>, char> {
  template <class context>
  constexpr auto parse(context& ctx) -> context::iterator {
    return ctx.begin();
  }
  template <class context>
  auto format(const optional<type>& data,
              context& ctx) const -> context::iterator {
    string str{};
    if (data)
      str = std::format("{}", data.value());
    else
      str = "false";
    return ranges::copy(str, ctx.out()).out;
  }
};

template <>
struct std::formatter<std::filesystem::path, char> {
  template <class context>
  constexpr auto parse(context& ctx) -> context::iterator {
    return ctx.begin();
  }
  template <class context>
  auto format(const filesystem::path& path,
              context& ctx) const -> context::iterator {
    return ranges::copy(path.string(), ctx.out()).out;
  }
};

int main(int argc, char* argv[]) {
  namespace cli = ensketch::cli;
  using cli::bind;
  using cli::flag;
  using cli::list;
  using cli::pos;
  using cli::var;

  auto options = cli::option_list{
      flag<"help">(),
      flag<"verbose", true>(),
      var<"validate", bool>(),
      var<"number", int>(),
      var<"scalar", float, 1.23f>(),
      var<"string", std::string_view, cli::meta::string{"initial"}>(),
      var<"maybe", std::optional<int>>(),
      var<"file", std::optional<std::filesystem::path>>(),
      list<"input", std::filesystem::path>(),
      pos<"output">([] -> std::filesystem::path { return "a.out"; }),
  };

  constexpr auto position_scheme =
      cli::meta::string_list<"output", "input", "input">{};
  constexpr auto bindings = cli::shortcuts(
      bind<'h', "help">, bind<'v', "verbose">, bind<'n', "number">,
      bind<'s', "string">, bind<'f', "file">);

  const auto print_options = [&options] {
    for_each(options.data(), []<auto name>(auto&& value) {
      std::println("{} = {}", name, std::forward<decltype(value)>(value));
    });
  };

  try {
    parse({argc, argv}, options, bindings, position_scheme);
  } catch (cli::parser_error& e) {
    std::println("ERROR: {}", e.what());
    print_options();
    return -1;
  }

  print_options();
}
