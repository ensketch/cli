#pragma once
#include <ensketch/cli/utility.hpp>

namespace ensketch::cli {

/// Standard Exception for Parsing Errors
/// It also provides the currently processed 'arg_list'.
/// So, after catching the exception,
/// it can be used to go on with the parsing.
struct parser_error : exception {
  parser_error() = default;

  /// Constructor to provide list of arguments and an error message.
  parser_error(const arg_list& args, generic::forwardable<string> auto&& text)
      : state{args}, msg{forward<string>(text)} {}

  auto what() const noexcept -> czstring override { return msg.c_str(); }
  auto args() const noexcept -> const arg_list& { return state; }

 private:
  string msg{};
  arg_list state{};
};

}  // namespace ensketch::cli
