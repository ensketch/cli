#include <print>
//
#include <ensketch/cli/defaults.hpp>

namespace ensketch::cli {

template <typename... cmds>
using program_base =
    named_tuple<meta::name_list<cmds::name()...>, std::tuple<cmds...>>;

template <typename... cmds>
struct program : program_base<cmds...> {
  using base = program_base<cmds...>;

  static consteval auto commands() noexcept {
    return meta::name_list<cmds::name()...>{};
  }

  constexpr program(auto&&... x) : base{std::forward<decltype(x)>(x)...} {}

  void operator()(int argc, char* argv[]) {
    if (argc < 1) return;
    call = argv[0];
    if (argc < 2) return;

    czstring cmd = argv[1];
    const auto matched =
        visit(meta::radix_tree_from(commands()),  //
              cmd, [&]<meta::string command> {
                std::invoke(value<command>(static_cast<base&>(*this)));
              });

    if (!matched) std::println("unknown command");
  }

  czstring call;
};

template <typename... commands>
program(commands&&...) -> program<std::unwrap_ref_decay_t<commands>...>;

template <typename... parameters>
using parameter_specification_base =
    named_tuple<meta::name_list<parameters::name()...>,
                std::tuple<parameters...>>;

template <typename... parameters>
struct parameter_specification : parameter_specification_base<parameters...> {};

template <meta::string str, typename params, typename exec>
struct command_entry : params, exec {
  static constexpr auto name() noexcept { return str; }
};

template <meta::string str, typename functor>
struct command : functor {
  using functor::operator();
  static constexpr auto name() noexcept { return str; }
};

template <meta::string name>
constexpr auto cmd(auto&& function) noexcept {
  return command<name, std::unwrap_ref_decay_t<decltype(function)>>{
      std::forward<decltype(function)>(function)};
}

}  // namespace ensketch::cli

int main(int argc, char* argv[]) {
  namespace cli = ensketch::cli;

  auto program = cli::program{
      cli::cmd<"version">([] { std::println("program version 0.1.0"); }),
      cli::cmd<"help">([] { std::println("program help"); }),
  };

  program(argc, argv);
}
