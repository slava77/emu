#ifndef __emu_farmer_utils_FileSystem_h__
#define __emu_farmer_utils_FileSystem_h__

#include "log4cplus/logger.h"
#include "xcept/Exception.h"

#include <string>
#include <vector>

using namespace std;

namespace emu { namespace farmer { namespace utils {

  string readFile( const string fileName ) throw ( xcept::Exception );

  void writeFile( const string fileName, const string content ) throw ( xcept::Exception );

  vector<string> execShellCommand( const string shellCommand ) throw ( xcept::Exception );

}}}

#endif
