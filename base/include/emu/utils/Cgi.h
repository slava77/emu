#ifndef __emu_utils_Cgi_h__
#define __emu_utils_Cgi_h__

#include "xcept/Exception.h"
#include "xgi/Method.h"
#include "xgi/Utils.h"


namespace emu { namespace utils {

/**
 * select elements from cgi response with names regex-mathcing the namePattern
 */
std::map<std::string,std::string> selectFromQueryString( std::vector<cgicc::FormEntry>& fev, const std::string namePattern ) throw( xcept::Exception );

/**
 * Redirect for the action not to be repeated if the 'reload' button is hit.
 */
void redirect(xgi::Input *in, xgi::Output *out);

/**
 * simple redirect to new URL
 */
void redirectTo( const std::string newURL, xgi::Output *out );

}}

#endif
