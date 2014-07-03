#ifndef H_PACKET
#define H_PACKET

#include <string>
#include <vector>
#include <stdint.h>
#include <utility>
#include <set>

namespace Packet{
  namespace io{
    const std::string bold("\033[1m");
    const std::string fg_black("\033[30m");
    const std::string fg_red("\033[31m");
    const std::string fg_green("\033[32m");
    const std::string fg_yellow("\033[33m");
    const std::string fg_blue("\033[34m");
    const std::string fg_magenta("\033[35m");
    const std::string fg_cyan("\033[36m");
    const std::string fg_white("\033[37m");
    const std::string bg_black("\033[40m");
    const std::string bg_red("\033[41m");
    const std::string bg_green("\033[42m");
    const std::string bg_yellow("\033[43m");
    const std::string bg_blue("\033[44m");
    const std::string bg_magenta("\033[45m");
    const std::string bg_cyan("\033[46m");
    const std::string bg_white("\033[47m");
    const std::string normal("\033[0m");
  }

  typedef std::vector<uint16_t> svu;

  template<typename t1, typename t2, typename t3> bool InRange(const t1& x,
                                                               const t2& low,
                                                               const t3& high){
    return x>=low && x<=high;
  }

  bool AllInRange(const svu&, const unsigned, const unsigned,
                  const uint16_t, const uint16_t);
  void PutInRange(unsigned&, unsigned&, const unsigned, const unsigned);
  typedef std::pair<uint_least16_t, std::pair<uint_least8_t, std::pair<uint_least8_t, uint_least8_t> > > dcfeb_data;
}

template<typename T>
std::set<T> GetUniques(const std::vector<T>& vec){
  std::set<T> set;
  for(unsigned i(0); i<vec.size(); ++i){
    set.insert(vec.at(i));
  }
  return set;
}

bool GetBit(const unsigned, const unsigned);
void PrintWithStars(const std::string&, const unsigned);

template<typename T>
double Sum(T begin, T end){
  double sum(0.0);
  volatile double correction(0.0);
  for(; begin!=end; ++begin){
    const double corrected_val(*begin-correction);
    const double temp_sum(sum+corrected_val);
    correction=(temp_sum-sum)-corrected_val;
    sum=temp_sum;
  }
  return sum;
}

template<typename T>
double Mean(T begin, T end){
  return Sum(begin, end)/static_cast<double>(std::distance(begin,end));
}

template<typename T>
double Variance(T begin, T end){
  const double mean(Mean(begin, end));
  std::vector<double> residuals_squared(0);
  for(T it(begin); it!=end; ++it){
    const double residual(*it-mean);
    residuals_squared.push_back(residual*residual);
  }
  return Sum(residuals_squared.begin(), residuals_squared.end())/static_cast<double>(std::distance(begin,end));
}

#endif
