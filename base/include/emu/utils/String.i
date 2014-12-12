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

/// Convert type T to binary number in ASCI string.
///
/// @param t Number of type T.
///
/// @return  Binary number in ASCI string.
///
template <typename T>
std::string binaryFrom( const T& t ){
  std::stringstream ss;
  const int nBits( sizeof( T ) * 8 );
  const T mask( 1 );
  for ( int i=nBits-1; i>=0; --i ) ss << ((t>>i) & mask);
  return ss.str();
}

/// Convert binary number in ASCI string to type T.
/// Spaces, underscores, etc. can be used, it ignores everything that is not '0' or '1'.
///
/// @param s Binary number in ASCI string.
///
/// @return  Number of type T.
///
template <typename T>
T binaryTo( const std::string s )
{
  T t( 0 );
  const T one( 1 );
  const size_t nBits( sizeof( T ) * 8 );
  size_t iBit( 0 );
  for ( std::string::const_reverse_iterator i=s.rbegin(); i!=s.rend() && iBit<nBits; ++i ){
    if      ( *i == '0' ) ++iBit;
    else if ( *i == '1' ){
      t |= one<<iBit;
      ++iBit;
    }
  }
  return t;
}
