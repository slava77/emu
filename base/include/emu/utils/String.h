#ifndef __emu_utils_String_h__
#define __emu_utils_String_h__

#include <string>
#include <sstream>
#include <vector>

namespace emu { namespace utils {

/**
 * convert string to any type that stringstream can >>
 */
template <typename T>
T stringTo( const std::string s )
{
  std::stringstream ss( s );
  T t;
  ss >> t;
  return t;
}

/**
 * convert to string from T&
 * T gotta have operator << defined
 */
template <typename T>
std::string stringFrom( const T& t )
{
  std::stringstream ss;
  ss << t;
  return ss.str();
}

/**
 * convert to string from T*
 * T gotta have operator << defined
 */
template <typename T>
std::string stringFrom( const T* t )
{
  std::stringstream ss;
  ss << *t;
  return ss.str();
}

/**
 * tokenize string by delimiter
 */
std::vector<std::string> splitString( const std::string& str, const std::string& delimiter );

/**
 * remove blanks from the string's beginning and end
 */
std::string shaveOffBlanks( const std::string& str );

}}

#endif
