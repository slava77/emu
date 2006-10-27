#ifndef __TF_HYPERDAQ_H__
#define __TF_HYPERDAQ_H__

#include "EmuApplication.h"

#include "xdata/String.h"

class TF_hyperDAQ : public EmuApplication
{
public:
	XDAQ_INSTANTIATOR();

	TF_hyperDAQ(xdaq::ApplicationStub *stub) throw (xdaq::exception::Exception);

private:
	xdata::String trigger_mode_;
};

#endif  // ifndef __TF_HYPERDAQ_H__
// vim: set sw=4 ts=4:
