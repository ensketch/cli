#include <iomanip>
#include <iostream>
#include <sstream>
//
#include <ensketch/cli/version.hpp>

using namespace std;
using namespace ensketch::cli;

int main() {
  // All the tests will be run asynchronously.
  // This leads to interleaved output in the terminal.
  // To prevent this for the version output, everything is written
  // into a 'stringstream' that is printed at the end at once.
  //
  stringstream output;
  output << "\nensketch cli version:\n\n";

#define CAPTURE(X) \
  output << left << setw(20) << #X << " = " << right << setw(40) << X << '\n'

  CAPTURE(version::number);
  CAPTURE(version::string);
  CAPTURE(version::id);
  CAPTURE(version::full);
  CAPTURE(version::major);
  CAPTURE(version::minor);
  CAPTURE(version::patch);
  CAPTURE(version::snapshot_sn);
  CAPTURE(version::snapshot_id);

#undef CAPTURE

  cout << output.str() << endl;
}
