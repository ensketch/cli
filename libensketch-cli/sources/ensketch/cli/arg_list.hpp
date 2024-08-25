#pragma once
#include <ensketch/cli/defaults.hpp>

namespace ensketch::cli {

// The 'arg_list' can be seen as a stream of arguments
// that will be provided as input for the parser.
// It will enable the lookahead for LL or push parsers.
//
struct arg_list {
  constexpr arg_list() = default;

  // Constructor for completeness
  //
  constexpr arg_list(int argc, czstring* argv, int i = 0)
      : count(argc), values{argv}, index(i) {}

  /// Constructor used to construct the stream directly
  /// from the arguments of the main function.
  /// The values of 'argv' will not be copied and
  /// should not be altered after calling the constructor.
  /// Providing an index should be used to set the count
  /// of already processed arguments.
  /// This enables us to also recursively handle multiple parsing strategies.
  ///
  constexpr arg_list(int argc, char** argv, int i = 0)
      : count(argc), values{const_cast<czstring*>(argv)}, index(i) {}

  /// Access the remaining unprocessed arguments by their index.
  /// This is for convenience and should rarely be used.
  ///
  constexpr auto operator[](size_t i) const noexcept -> czstring {
    return values[index + i];
  }

  /// Get the count of the remaining unprocessed arguments in the list.
  ///
  constexpr auto size() const noexcept -> size_t { return count - index; }

  /// Check whether all arguments of the list have already been processed.
  ///
  constexpr bool empty() const noexcept { return index >= count; }
  //
  constexpr operator bool() const noexcept { return !empty(); }

  /// Get the current unprocessed argument and mark it as processed.
  /// This function does not delete the processed argument.
  /// We can always use 'unpop_front' to mark it again as unprocessed.
  ///
  constexpr auto pop_front() noexcept -> czstring { return values[index++]; }

  /// Mark the last argument that has been removed from the list
  /// as unprocessed and put it back into the start of the list.
  ///
  constexpr void unpop_front() noexcept { --index; }

  /// Get the current unprocessed argument without extracting it from the stream.
  /// It will not be marked as processed
  /// and will again be accessed when 'pop_front' is called.
  ///
  constexpr auto front() const noexcept -> czstring { return values[index]; }

  /// Same as 'front' and provided for consistency.
  ///
  constexpr auto peek_front() const noexcept -> czstring {
    return values[index];
  }

  /// Count of all processed and unprocessed arguments
  ///
  size_t count{};

  /// C strings of arguments typically provided by program execution
  ///
  czstring* values{};

  /// Index to start of the unprocessed arguments
  ///
  size_t index{};
};

}  // namespace ensketch::cli
