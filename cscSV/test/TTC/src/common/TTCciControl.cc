// TTCciControl.cc

#include "TTCciControl.h"

#include "xdaq/NamespaceURI.h"
#include "xoap/Method.h"

XDAQ_INSTANTIATOR_IMPL(TTCciControl);

TTCciControl::TTCciControl(xdaq::ApplicationStub *stub)
		throw (xdaq::exception::Exception) :
		EmuApplication(stub)
{
	source_clock_ = "CTC";
	source_orbit_ = "LTC";
	source_trigger_ = "TTL";
	source_bgo_ = "LOCAL";

	getApplicationInfoSpace()->fireItemAvailable("ClockSource", &source_clock_);
	getApplicationInfoSpace()->fireItemAvailable("OrbitSource", &source_orbit_);
	getApplicationInfoSpace()->fireItemAvailable("TriggerSource", &source_trigger_);
	getApplicationInfoSpace()->fireItemAvailable("BGOSource", &source_bgo_);

	LOG4CPLUS_INFO(getApplicationLogger(), "TTCciControl");
}

// End of file
// vim: set sw=4 ts=4:
