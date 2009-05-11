#ifndef __emu_farmer_utils_Cgi_h__
#define __emu_farmer_utils_Cgi_h__

#include "xcept/Exception.h"
#include "xgi/Method.h"
#include "xgi/Utils.h"

using namespace std;

namespace emu { namespace farmer { namespace utils {

  map<string,string> selectFromQueryString( std::vector<cgicc::FormEntry>& fev, const string namePattern )
    throw( xcept::Exception );

  void redirect(xgi::Input *in, xgi::Output *out);

  void redirectTo( const string newURL, xgi::Input *in, xgi::Output *out );

}}}

#endif
