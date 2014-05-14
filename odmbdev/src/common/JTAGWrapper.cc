#include "emu/odmbdev/JTAGWrapper.h" 
#include "emu/odmbdev/Manager.h"
#include "emu/odmbdev/utils.h"
#include <iomanip>
using namespace std;
 
namespace emu { namespace odmbdev {
  JTAGWrapper::JTAGWrapper(VMEWrapper * vme_wrapper) : 
    vme_wrapper_(vme_wrapper)
  {
    cout << "JERR" << endl; 
  }
    
  JTAGWrapper::JTAGWrapper( const JTAGWrapper& other ) : 
    vme_wrapper_(other.vme_wrapper_)
  {
  }
    
  void JTAGWrapper::JTAGShift (unsigned short int IR, unsigned short int DR, unsigned int nBits, unsigned short int hdr_tlr_code, unsigned int slot) {
    cout << "JERR1" << endl;
    vme_wrapper_->VMEWrite(0x191C, IR, slot, "Set instruction register");
    cout << "JERR2" << endl;
    unsigned int addr_data_shift(0x1);
    addr_data_shift = (addr_data_shift<<4)|(nBits-1);
    addr_data_shift = (addr_data_shift<<4)|hdr_tlr_code; // hdr_tlr_code is 4, 8, or C,
                                                         // depending on nBits to shift
    vme_wrapper_->VMEWrite(addr_data_shift, DR, slot, "Shift data");
  }
  unsigned int JTAGWrapper::JTAGRead (unsigned short int DR, unsigned int nBits, unsigned int slot) {
    this->JTAGShift(0x3C2, DR, nBits, 12, slot);
    this->JTAGShift(0x3C3, 0, nBits, 12, slot);
    unsigned short int VMEresult;
    VMEresult = vme_wrapper_->VMERead(0x1014, slot, "Read TDO register");
    unsigned int formatted_result(VMEresult);
    return formatted_result;
  }
}
}
