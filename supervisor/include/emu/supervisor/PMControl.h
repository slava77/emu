#ifndef __emu_supervisor_PMControl_h__
#define __emu_supervisor_PMControl_h__

#include "emu/supervisor/TCDSControl.h"

namespace emu{
  namespace supervisor{
    class PMControl : public TCDSControl{
    public:
      PMControl( xdaq::Application *parent, 
		 xdaq::ApplicationDescriptor* tcdsApplicationDescriptor, 
		 xdata::String partition );
      PMControl& setRunType( xdata::String & runType );
      PMControl& configure( xdata::String& hardwareConfigurationString );
      PMControl& configureSequence();
      PMControl& enableSequence   ();
      PMControl& stopSequence     ();
    };
  }
}

#endif
