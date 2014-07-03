#include "utils.hpp"

#include <iostream>
#include <vector>
#include <stdint.h>
#include <string>

namespace Packet{
  bool AllInRange(const svu& vec, const unsigned start, const unsigned end,
                  const uint16_t low, const uint16_t high){
    bool all_in_range(true);
    for(unsigned index(start); index<vec.size() && index<end && all_in_range; ++index){
      if(!InRange(vec.at(index), low, high)) all_in_range=false;
    }
    return all_in_range;
  }

  void PutInRange(unsigned& a, unsigned& b, const unsigned min, const unsigned max){
    if(max<=min){
      a=max;
      b=max;
    }else{
      if(a<min || a>max) a=min;
      if(b<min || b>max) b=min;
      if(b<a) a=b;
    }
  }
}

bool GetBit(const unsigned x, const unsigned bit){
  return (x>>bit) & 1u;
}

void PrintWithStars(const std::string& text, const unsigned full_width){
  const unsigned spares(full_width-text.length());
  if(spares>=2 && full_width>text.length()){
    for(unsigned i(0); i<3 && i<spares-1; ++i) std::cout << '*';
    std::cout << ' ' << text;
    if(spares>=6){
      std::cout << ' ';
      for(unsigned i(0); i<spares-5; ++i) std::cout << '*';
    }
    std::cout << std::endl;
  }else{
    std::cout << text << std::endl;
  }
}
