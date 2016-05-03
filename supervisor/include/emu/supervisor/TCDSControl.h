#ifndef __emu_supervisor_TCDSControl_h__
#define __emu_supervisor_TCDSControl_h__

#include "xdata/String.h"
#include "xdata/Boolean.h"
#include "xdaq/ApplicationDescriptor.h"

#include "emu/soap/Messenger.h"
#include "emu/supervisor/TCDSHardwareLease.h"

using namespace std;

namespace emu{
  namespace supervisor{
    class TCDSControl{
    public:
      enum RunType_t { global, local, AFEBcalibration, CFEBcalibration, UNKNOWN };
      TCDSControl( xdaq::Application *parent, 
		   xdaq::ApplicationDescriptor* tcdsApplicationDescriptor, 
		   xdata::String partition,
		   string tcdsDeviceName );
      virtual ~TCDSControl();
      virtual TCDSControl& setRunType( xdata::String & runType );
      // Individual commands implemented in the TCDS SOAP interface
      TCDSControl& configure( xdata::String& hardwareConfigurationString );
      TCDSControl& enable( xdata::UnsignedInteger& runNumber );
      TCDSControl& halt();
      TCDSControl& stop();
      TCDSControl& pause();
      TCDSControl& resume();
      TCDSControl& sendL1A();
      TCDSControl& sendBgo( xdata::String& bgoName );
      TCDSControl& sendBgo( xdata::UnsignedInteger& bgoNumber );
      TCDSControl& sendBgoTrain( xdata::String& bgoTrainName );
      TCDSControl& sendBCommand( xdata::UnsignedInteger& bcommandData,
				 xdata::String& bcommandType );
      TCDSControl& sendBCommand( xdata::UnsignedInteger& bcommandData,
				 xdata::String& bcommandType,
				 xdata::UnsignedInteger& bcommandAddress,
				 xdata::UnsignedInteger& bcommandSubAddress,
				 xdata::String& bcommandAddressType );
      TCDSControl& initCyclicGenerators();
      string readHardwareConfiguration();
      // Sequences of SOAP commands
      virtual TCDSControl& configureSequence(){ return *this; };
      virtual TCDSControl& enableSequence   (){ return *this; };
      virtual TCDSControl& stopSequence     (){ return *this; };
      // FSM-related methods
      string getState();
      string getSteadyState();
      bool waitForState( const string& targetState, const int timeoutSeconds );
      static bool isSteadyState( const string& state );
    protected:
      TCDSControl();
      string waitForASteadyState( const int timeoutSeconds );
      void mSleep( unsigned int milliseconds ) const;
      xdaq::Application * const parentApplication_;
      emu::soap::Messenger * const messenger_;
      xdata::String hardwareConfiguration_;
      xdata::String partition_;
      xdata::String actionRequestorId_;
      xdata::String fedEnableMask_;
      xdaq::ApplicationDescriptor* const tcdsApplicationDescriptor_;
      RunType_t runType_;
      auto_ptr<TCDSHardwareLease> hardwareLease_;
    };
  }
}

#endif
