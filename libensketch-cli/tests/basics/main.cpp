#include <iomanip>
#include <iostream>
//
#include <ensketch/cli/arg_list.hpp>
#include <ensketch/cli/option_list.hpp>
#include <ensketch/cli/parser/name_parser.hpp>
#include <ensketch/cli/parser/position_parser.hpp>
#include <ensketch/cli/parser/shortcut_parser.hpp>
#include <ensketch/cli/parser_kernel.hpp>
#include <ensketch/cli/parser_list.hpp>

using namespace std;
// using ensketch::cli::arg_list;
using namespace ensketch::cli;

template <static_zstring name,
          static_zstring description,
          typename type = czstring>
using var = attachment<name, description, type>;

using options_type = option_list<flag<"help", "Print the help message.">,
                                 flag<"version", "Print the program version.">,
                                 flag<"verbose", "Verbose output.">,
                                 var<"key", "Set the key of the program.", int>,
                                 var<"input", "Set the input file path.">,
                                 var<"output", "Set the output file path.">>;

constexpr auto position_scheme = static_zstring_list<"output", "input">{};

constexpr auto binding_scheme = type_list<char_binding<'v', "verbose">,
                                          char_binding<'h', "help">,
                                          char_binding<'i', "input">,
                                          char_binding<'o', "output">>{};

using parser_list =
    named_tuple<static_identifier_list<"--", "-", "">,
                std::tuple<name_parser<options_type>,
                           shortcut_parser<binding_scheme, options_type>,
                           position_parser<position_scheme, options_type>>>;

int main(int argc, char* argv[]) {
  arg_list args{argc, argv};
  options_type options{};
  parser_list parsers{};
  parse(args, options, parsers);

  cout << "usage:\n" << argv[0] << " [options]";
  for_each(position_scheme, []<auto name> { cout << " <" << name << ">"; });
  cout << endl;

  static_assert(!generic::reducible_named_tuple<decltype(options.tuple())>);

  ensketch::xstd::for_each(options.tuple(), [&](auto& option) {
    cout << left << setw(20) << option.name() << '\t' << option.description()
         << endl;

    // for_each(parsers, [&]<static_zstring prefix>(auto& parser) {
    //   cout << prefix << option.name() << endl;
    // });

    cout << boolalpha << "value = " << option.value << endl << endl;
  });
}
