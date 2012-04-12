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
  std::cout << "[";
  typename std::vector< T >::const_iterator it;
  for (it = t.begin(); it != t.end(); ++it)
    std::cout << *it << (it + 1 != t.end() ? "," : "");
  std::cout << "]";
  return os;
}

/**
 * quick set dump
 */
template < typename T >
std::ostream& operator<<(std::ostream& os, const std::set<T>& t)
{
  std::cout << "{";
  typename std::set<T>::iterator it, iti;
  for ( it = iti = t.begin(), ++iti; it != t.end(); ++it, ++iti )
    std::cout << *it << (iti != t.end() ? "," : "");
  std::cout << "}";
  return os;
}

/**
 * quick pair dump
 */
template< typename T1, typename T2 >
std::ostream& operator<<(std::ostream& os, const std::pair< T1, T2 >& t)
{
  std::cout << "(" << t.first << "|" << t.second << ")";
  return os;
}

/**
 * quick map dump
 */
template< typename T1, typename T2 >
std::ostream& operator<<(std::ostream& os, const std::map< T1, T2 >& t)
{
  std::cout << "(";
  typename std::map< T1, T2 >::const_iterator it, iti;
  for (it = iti = t.begin(), ++iti; it != t.end(); ++it, ++iti)
    std::cout << it->first << "->" << it->second << (iti != t.end() ? "," : "");
  std::cout << ")";
  return os;
}

/**
 * quick valarray dump
 */
template < typename T >
std::ostream& operator<<(std::ostream& os, const std::valarray<T>& t)
{
  std::cout << "[";
  size_t i=0;
  while( i+1 < t.size() ){
    std::cout << t[i] << ", ";
    ++i;
  }
  std::cout << t[i] << "]";
  return os;
}

}}

#endif
