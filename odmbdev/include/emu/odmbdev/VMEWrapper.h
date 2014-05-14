#ifndef __emu_odmbdev_VMEWrapper_h__
#define __emu_odmbdev_VMEWrapper_h__

#include <iostream>
#include <sstream>
#include <vector>
#include "xgi/exception/Exception.h"
#include "xgi/Input.h"
#include "xgi/Output.h"
#include "emu/utils/String.h"
#include "emu/pc/Crate.h"
#include "emu/pc/VMEController.h"


/******************************************************************************
 * The VMEWrapper Class
 *
 * This allows you to send VME commands via a more intuitive interface.
 * Note that VME writes are voids and VME reads return unsigned short ints.
 *****************************************************************************/

using namespace std;
using namespace emu::pc;


namespace emu { 
// forward declarations:
namespace pc {
class Crate;
}

namespace odmbdev {
    class VMEWrapper {
    public:
      VMEWrapper(emu::pc::Crate * crate);
      VMEWrapper(const VMEWrapper&);
      virtual ~VMEWrapper() { };
      void VMEWrite (unsigned short int, unsigned short int, unsigned int=7, string="");
      unsigned short int VMERead (unsigned short int, unsigned int=7, string="");
      unsigned int JTAGShift (unsigned short int, unsigned short int, unsigned int, unsigned short int=12, unsigned int=7);
      unsigned int JTAGRead (unsigned short int, unsigned int, unsigned int=7);
      string ODMBVitals (unsigned int);
      string SYSMONReport (unsigned int);
      string BurnInODMBs();
    
    protected:
    	emu::pc::Crate * crate_;
    	unsigned int port_number_;
    	string logFile_;
    	ofstream logger_;
    private:
    	VMEWrapper(VMEWrapper * vme_wrapper=0){cout << "Fail." << endl;}
    };
  }
}

#endif
