#pragma once
#include <ensketch/cli/defaults.hpp>

namespace ensketch::cli {

/// Standard Exception for Parsing Errors
/// It also provides the currently processed 'arg_list'.
/// So, after catching the exception,
/// it can be used to go on with the parsing.
///
struct parser_error : std::runtime_error {
  using base = runtime_error;

  parser_error(czstring current, const arg_list& args, const string& text)
      : base{text}, ptr{current}, state{args} {}

  auto current() const noexcept -> czstring { return ptr; }
  auto args() const noexcept -> const arg_list& { return state; }

 private:
  czstring ptr{};
  arg_list state{};
};

struct parser_error_base {
  virtual auto what() -> string { return ""; }
  czstring current{};
  arg_list args{};
};
struct unknown_option_name : parser_error_base {
  auto what() -> string override {
    return std::format("unknown option name '{}' in '{}'", current,
                       args.previous());
  }
};

}  // namespace ensketch::cli
