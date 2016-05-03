#ifndef __emu_supervisor_CIControl_h__
#define __emu_supervisor_CIControl_h__

#include "emu/supervisor/TCDSControl.h"

namespace emu{
  namespace supervisor{
    class CIControl : public TCDSControl{
    public:
      CIControl( xdaq::Application *parent, 
		 xdaq::ApplicationDescriptor* tcdsApplicationDescriptor, 
		 xdata::String partition );
      CIControl& setRunType( xdata::String & runType );
      CIControl& configureSequence();
      CIControl& enableSequence   ();
      CIControl& stopSequence     ();
    };
  }
}

#endif
