#ifndef __emu_utils_IO_h__
#define __emu_utils_IO_h__

#include <ostream>

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
 * quick pair dump
 */
template< typename T1, typename T2 >
std::ostream& operator<<(std::ostream& os, const std::pair< T1, T2 >& t)
{
  std::cout << "(" << t.first << ":" << t.second << ")";
  return os;
}

/**
 * quick map dump
 */
template< typename T1, typename T2 >
std::ostream& operator<<(std::ostream& os, const std::map< T1, T2 >& t)
{
  std::cout << "(";
  typename std::map< T1, T2 >::const_iterator it;
  for (it = t.begin(); it != t.end(); ++it)
    std::cout << it->first << ":" << it->second << ","; //(it+1 != t.end() ? "," : ""); // no match for 'operator+' in 'it + 1'...
  std::cout << ")";
  return os;
}
    
}}

#endif
