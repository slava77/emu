// TF_hyperDAQ.cc

#include "TF_hyperDAQ.h"

#include "xdaq/NamespaceURI.h"
#include "xoap/Method.h"

XDAQ_INSTANTIATOR_IMPL(TF_hyperDAQ);

TF_hyperDAQ::TF_hyperDAQ(xdaq::ApplicationStub *stub)
		throw (xdaq::exception::Exception) :
		EmuApplication(stub)
{
	trigger_mode_ = "Monitor";

	getApplicationInfoSpace()->fireItemAvailable("triggerMode", &trigger_mode_);

	LOG4CPLUS_INFO(getApplicationLogger(), "TF_hyperDAQ");
}

// End of file
// vim: set sw=4 ts=4:
