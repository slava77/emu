#include "emu/supervisor/RegDumpPreprocessor.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;
//using namespace emu::supervisor;

int main( int argc, char** argv ){
  stringstream ss;
  fstream inFile( argv[1], fstream::in );
  while ( inFile.good() )
    {
      char c = char( inFile.get() );
      if (inFile.good()) ss << c;
    }

  RegDumpPreprocessor pp;
  ostringstream msg;    // Use this to collect messages in.
  pp.setOptions( RegDumpPreprocessor::expandRanges ).setMessageStream( msg );
  cout << "Unprocessed regDump:\n"     << ss.str()
       << "\nPreprocessed regDump:\n"  << pp.process( ss.str() );
  cout << "\nPreprocessor messages:\n" << msg.str() << endl;

  return 0;
}
