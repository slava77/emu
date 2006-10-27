#ifndef __TTCCI_CONTROL_H__
#define __TTCCI_CONTROL_H__

#include "EmuApplication.h"

#include "xdata/String.h"

class TTCciControl : public EmuApplication
{
public:
	XDAQ_INSTANTIATOR();

	TTCciControl(xdaq::ApplicationStub *stub) throw (xdaq::exception::Exception);

private:
	xdata::String source_clock_;
	xdata::String source_orbit_;
	xdata::String source_trigger_;
	xdata::String source_bgo_;
};

#endif  // ifndef __TTCCI_CONTROL_H__
// vim: set sw=4 ts=4:
