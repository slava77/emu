#ifndef __emu_odmbdev_JTAGWrapper_h__
#define __emu_odmbdev_JTAGWrapper_h__

#include <iostream>
#include <sstream>
#include <vector>
#include "xgi/exception/Exception.h"
#include "xgi/Input.h"
#include "xgi/Output.h"
#include "emu/utils/String.h"
#include "emu/pc/Crate.h"
#include "emu/pc/VMEController.h"
#include "emu/odmbdev/VMEWrapper.h" 


/******************************************************************************
 * The JTAGWrapper Class
 *
 * This allows you to send JTAG commands via a more intuitive and efficient interface.
 * The JTAGShift function is called by the JTAGRead function multiple times 
 *****************************************************************************/

using namespace std;
using namespace emu::pc;

// forward declarations:
namespace emu { 
  namespace pc {
    class Crate;
  }

  namespace odmbdev {
    class VMEWrapper;
    class JTAGWrapper {
    public:
      JTAGWrapper(emu::odmbdev::VMEWrapper* vme_wrapper);
      JTAGWrapper(const JTAGWrapper&);
      virtual ~JTAGWrapper() { };
      void JTAGShift (unsigned short int, unsigned short int, unsigned int, unsigned short int=12, unsigned int=7);
      unsigned int JTAGRead (unsigned short int, unsigned int, unsigned int=7);
    
    protected:
      emu::odmbdev::VMEWrapper* vme_wrapper_;
    private:
      JTAGWrapper(JTAGWrapper * jtag_wrapper=0){cout << "Fail." << endl;}
    };
  }
}

#endif
