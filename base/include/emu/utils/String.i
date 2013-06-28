///
/// @file   String.i
/// @author  <banicz@localhost>
/// @date   Fri Jun 28 13:42:45 2013
/// 
/// @brief  Templated functions for converting to and from string or CSV.
/// 
/// This file is to be included in the header file emu/utils/String.h only.
///


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

/// Put CSV (character-separated values) into an STL container (deque, list, set, multiset or vector).
///
/// @param s string of character-separated values
/// @param delimiter delimiter character, comma by default
///
/// @return STL container initialized from CSV
///
template <typename Container>
Container csvTo( const std::string& csv, const char delimiter = ',' )
{
  Container values;
  typename Container::value_type value;
  size_t start = 0;
  size_t found = csv.find( delimiter );
  while ( found != std::string::npos )
  {
    value = typename Container::value_type();
    std::istringstream iss( csv.substr( start, found - start ) );
    iss >> value;
    values.insert( values.end(), value );
    start = found + 1;
    found = csv.find( delimiter, start );
  }
  value = typename Container::value_type();
  std::istringstream last( csv.substr(start) );
  last >> value;
  values.insert( values.end(), value );
  return values;
}

/// Get CSV (character-separated values) from an STL container.
///
/// @param c STL container
/// @param delimiter delimiter character, comma by default
///
/// @return string of character-separated values
///
template <typename Container>
std::string csvFrom( const Container& c, const char delimiter = ',' )
{
  std::ostringstream oss;
  for ( typename Container::const_iterator it = c.begin(); it != c.end(); ++it ){
    typename Container::const_iterator i = it;
    ++i;
    oss << *it << ( i != c.end() ? std::string( &delimiter ): "" );
  }
  return oss.str();
}
