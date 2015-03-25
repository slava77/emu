#include "emu/supervisor/RegDumpPreprocessor.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;
//using namespace emu::supervisor;

int main( int argc, char** argv ){
  stringstream original;
  fstream originalFile( argv[1], fstream::in );
  while ( originalFile.good() ){
      char c = char( originalFile.get() );
      if (originalFile.good()) original << c;
  }

  RegDumpPreprocessor pp;
  ostringstream msg;    // Use this to collect messages in.
  pp.setOptions( RegDumpPreprocessor::expandRanges ).setMessageStream( msg );
  cout << "Unprocessed regDump:\n"     << original.str()
       << "\nPreprocessed regDump:\n"  << pp.process( original.str() );
  cout << "\nPreprocessor messages:\n" << msg.str() << endl;

  if ( argc == 3 ){
    // We have a second argument. Substitute the registers in it for the meatching registers in the original regDump.
    stringstream substitutes;
    fstream substitutesFile( argv[2], fstream::in );
    while ( substitutesFile.good() ){
      char c = char( substitutesFile.get() );
      if (substitutesFile.good()) substitutes << c;
    }
    cout << "Substitute registers:\n"                       << substitutes.str()
	 << "\nPreprocessed regDump with substitutions:\n"  << pp.process( original.str(), substitutes.str() );
    cout << "\nPreprocessor messages:\n"                    << msg.str() << endl;
  }

  return 0;
}
