#ifndef __emu_me11dev_utils_h__
#define __emu_me11dev_utils_h__

#include <string>

namespace emu{ 
  namespace me11dev{

    unsigned int BinaryString_to_UInt(std::string str);
    std::string withoutSpecialChars(std::string s);
    
  }
}


#endif //__emu_me11dev_utils_h__
