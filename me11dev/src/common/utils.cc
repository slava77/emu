#include "emu/me11dev/utils.h"

/*****************************************************************************
 * Utility functions
 *****************************************************************************/

namespace emu{ 
  namespace me11dev{


    unsigned int BinaryString_to_UInt(std::string str){
      // brute force way to turn a string of 0 and 1 into an unsigned int
      unsigned int uint=0; // return value
      int nstr = str.size(); // # bits in string
      int nmax = 8*int(sizeof(unsigned int)); // # bits in unsigned int
    
      for(int i = nstr-1; i >= 0 && i >= nstr-nmax; i--){
	if(str[i] != '0') uint = uint | (0x1 << (nstr-i-1));
      }
      return uint;
    }

  }
}

