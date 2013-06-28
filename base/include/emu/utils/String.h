#ifndef __emu_utils_String_h__
#define __emu_utils_String_h__

#include <string>
#include <vector>
#include <sstream>

namespace emu { namespace utils {

#include "emu/utils/String.i"

/**
 * tokenize string by delimiter
 */
std::vector<std::string> splitString( const std::string& str, const std::string& delimiter );

/**
 * remove blanks from the string's beginning and end
 */
std::string shaveOffBlanks( const std::string& str );

}}


namespace emu { namespace pc {
#include "emu/utils/String.i"
}}


namespace emu { namespace step {
#include "emu/utils/String.i"
}}


#endif
