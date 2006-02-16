#include "EmuMonitor.h"
#include "xdaq.h"
#include <iostream>

class EmuMonitorSO: public xdaqSO {
public:
  void init() {
    monitor_ = new EmuMonitor();
    executive->load(monitor_);
  }

  void shutdown() {
    delete monitor_;
  }

protected:
  EmuMonitor * monitor_;
};


extern "C" void * init_EmuMonitor()
{
	return ((void *) new EmuMonitorSO());
}

