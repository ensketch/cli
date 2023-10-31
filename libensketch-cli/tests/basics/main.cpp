#include <iomanip>
#include <iostream>
//
#include <ensketch/cli/arg_list.hpp>
#include <ensketch/cli/option_list.hpp>

using namespace std;
// using ensketch::cli::arg_list;
using namespace ensketch::cli;

template <static_zstring n, static_zstring d>
struct flag {
  static consteval auto name() { return n; }
  static consteval auto description() { return d; }
  bool value = false;
};

constexpr bool parse(instance::flag auto& option,
                     czstring current,
                     arg_list& args) {
  if (*current) return false;
  return option.value = true;
}

template <static_zstring n,
          static_zstring d,
          typename type = czstring,
          type default_value = type{}>
struct attachment {
  static consteval auto name() { return n; }
  static consteval auto description() { return d; }
  type value = default_value;
};

constexpr bool parse(instance::attachment auto& option,
                     czstring current,
                     arg_list& args) {
  if (*current) return false;
  if (args.empty()) {
    args.unpop_front();
    throw parser_error(
        args, string("No given value for option '") + args.front() + "'.");
  }
  option.value = args.pop_front();
  return true;
}

using options_type = option_list<flag<"help", "Print the help message.">,
                                 flag<"version", "Print the program version.">,
                                 flag<"key", "Set the key of the program.">>;
options_type options{};

static_assert(generic::option_list<options_type>);

int main(int argc, char* argv[]) {
  arg_list args{argc, argv};
  while (args) cout << args.pop_front() << endl;

  for_each(options, [](auto& option) {
    cout << left << setw(20) << option.name() << '\t' << option.description()
         << endl;
  });
}
