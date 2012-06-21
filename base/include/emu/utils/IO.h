#ifndef __emu_utils_IO_h__
#define __emu_utils_IO_h__

#include <vector>
#include <map>
#include <set>
#include <valarray>
#include <iostream>

namespace emu { namespace utils {

/**
 * quick vector dump
 */
template< typename T >
std::ostream& operator<<(std::ostream& os, const std::vector< T >& t)
{
  os << "[";
  typename std::vector< T >::const_iterator it;
  for (it = t.begin(); it != t.end(); ++it)
    os << *it << (it + 1 != t.end() ? "," : "");
  os << "]";
  return os;
}

/**
 * quick set dump
 */
template < typename T >
std::ostream& operator<<(std::ostream& os, const std::set<T>& t)
{
  os << "{";
  if ( t.size() )
    {
      typename std::set<T>::iterator it, iti;
      for ( it = iti = t.begin(), ++iti; it != t.end(); ++it, ++iti )
	os << *it << (iti != t.end() ? "," : "");
    }
  os << "}";
  return os;
}

/**
 * quick pair dump
 */
template< typename T1, typename T2 >
std::ostream& operator<<(std::ostream& os, const std::pair< T1, T2 >& t)
{
  os << "(" << t.first << "|" << t.second << ")";
  return os;
}

/**
 * quick map dump
 */
template< typename T1, typename T2 >
std::ostream& operator<<(std::ostream& os, const std::map< T1, T2 >& t)
{
  os << "(";
  if ( t.size() )
    {
      typename std::map< T1, T2 >::const_iterator it, iti;
      for (it = iti = t.begin(), ++iti; it != t.end(); ++it, ++iti)
	os << it->first << "->" << it->second << (iti != t.end() ? "," : "");
    }
  os << ")";
  return os;
}

/**
 * quick multimap dump
 */
template< typename T1, typename T2 >
std::ostream& operator<<(std::ostream& os, const std::multimap< T1, T2 >& t)
{
  os << "(";
  if ( t.size() )
    {
      typename std::multimap< T1, T2 >::const_iterator it, iti;
      for (it = iti = t.begin(), ++iti; it != t.end(); ++it, ++iti)
	os << it->first << "->" << it->second << (iti != t.end() ? "," : "");
    }
  os << ")";
  return os;
}

/**
 * quick valarray dump
 */
template < typename T >
std::ostream& operator<<(std::ostream& os, const std::valarray<T>& t)
{
  os << "[";
  size_t i=0;
  while( i+1 < t.size() ){
    os << t[i] << ", ";
    ++i;
  }
  os << t[i] << "]";
  return os;
}

}}

#endif
