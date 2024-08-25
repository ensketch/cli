#pragma once
#include <ensketch/cli/parser_error.hpp>
#include <ensketch/cli/parser_list.hpp>

namespace ensketch::cli {

constexpr void parse(czstring str, int& out) {
  std::stringstream input{str};
  input >> out;
  if (!input)
    throw std::runtime_error(
        std::format("Failed to parse int from '{}'.", str));
}

constexpr void parse(czstring str, std::string_view& out) {
  out = std::string_view{str};
}

constexpr void parse(czstring str, bool& out) {
  std::string_view view{str};
  if (view == "true")
    out = true;
  else if (view == "false")
    out = false;
  else
    throw std::runtime_error(std::format("invalid boolean value '{}'", str));
}

constexpr void parse(czstring str, arg_list& args, std::string_view& out) {
  out = std::string_view{str};
}

constexpr void parse(czstring current, arg_list& args, bool& out) {
  std::string_view view{current};
  if (view == "true")
    out = true;
  else if (view == "false")
    out = false;
  else {
    args.unpop_front();
    throw parser_error(
        current, args,
        format("invalid boolean value '{}' in '{}'", view, args.front()));
  }
}

constexpr void parse(czstring current, arg_list& args, integral auto& out) {
  string_view view{current};
  auto [ptr, ec] = std::from_chars(view.data(), view.data() + view.size(), out);
  if (ec == std::errc()) {
    if (ptr == view.end()) return;
    args.unpop_front();
    throw parser_error(current, args,
                       format("invalid integral value '{}'", view));
  } else if (ec == std::errc::invalid_argument) {
    args.unpop_front();
    throw parser_error(current, args,
                       format("invalid integral value '{}'", view));
  } else if (ec == std::errc::result_out_of_range) {
    args.unpop_front();
    throw parser_error(current, args,
                       format("out-of-range integral value '{}'", view));
  }
}

constexpr void parse(czstring current,
                     arg_list& args,
                     floating_point auto& out) {
  string_view view{current};
  auto [ptr, ec] = std::from_chars(view.data(), view.data() + view.size(), out);
  if (ec == std::errc()) {
    if (ptr == view.end()) return;
    args.unpop_front();
    throw parser_error(current, args,
                       format("invalid floating-point value '{}'", view));
  } else if (ec == std::errc::invalid_argument) {
    args.unpop_front();
    throw parser_error(current, args,
                       format("invalid floating-point value '{}'", view));
  } else if (ec == std::errc::result_out_of_range) {
    args.unpop_front();
    throw parser_error(current, args,
                       format("out-of-range floating-point value '{}'", view));
  }
}

template <typename type>
constexpr void parse(czstring current, arg_list& args, optional<type>& out) {
  type t{};
  parse(current, args, t);
  out = make_optional(t);
}

constexpr void parse(czstring current, arg_list& args, filesystem::path& path) {
  path = current;
}

template <typename option_list, typename parser_list>
constexpr void parse(arg_list args,
                     option_list& options,
                     parser_list&& parsers) {
  // Assume that the first argument is the name of the program.
  args.pop_front();

  // Process all arguments in the list one after another.
  while (args) {
    const auto current = args.pop_front();

    // Traverse the static prefix tree with the current argument
    // and call the specific parsing routine when a prefix could be matched.
    const auto prefix_matched =
        traverse(meta::radix_tree_from(parser_list::names),  //
                 current, [&]<meta::string prefix>(czstring tail) {
                   invoke(value<prefix>(parsers.data()), tail, args, options);
                 });
    if (prefix_matched) continue;

    // If no prefix could be matched then we failed to parse the given argument.
    args.unpop_front();
    throw parser_error(
        current, args,
        format("no matching parser rule for '{}'", args.front()));
  }
}

}  // namespace ensketch::cli
