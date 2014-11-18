#ifndef __emu_supervisor_TCDSHardwareLease_h__
#define __emu_supervisor_TCDSHardwareLease_h__

#include "xdata/String.h"
#include "xdaq/ApplicationDescriptor.h"
#include "toolbox/task/Timer.h"
#include "toolbox/task/TimerListener.h"
#include "toolbox/TimeInterval.h"

#include "emu/soap/Messenger.h"

using namespace std;

namespace emu{
  namespace supervisor{
    class TCDSHardwareLease : public toolbox::task::TimerListener{
    public:
      TCDSHardwareLease( xdaq::Application *parent, 
			 xdaq::ApplicationDescriptor* tcdsApplicationDescriptor,
			 const xdata::String& actionRequestorId,
			 const toolbox::TimeInterval& interval );
      ~TCDSHardwareLease();
    private:
      TCDSHardwareLease();
      void renew();
      void timeExpired( toolbox::task::TimerEvent& event );
      xdaq::Application           *parentApplication_;
      emu::soap::Messenger        *messenger_;
      xdata::String                actionRequestorId_;
      xdaq::ApplicationDescriptor *tcdsApplicationDescriptor_;
      toolbox::TimeInterval        interval_;
      toolbox::task::Timer        *timer_;
    };
  }
}

#endif
