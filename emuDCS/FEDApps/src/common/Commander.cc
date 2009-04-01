/*****************************************************************************\
* $Id: Commander.cc,v 1.1 2009/04/01 21:08:24 paste Exp $
\*****************************************************************************/
#include "emu/fed/Commander.h"

#include "xgi/Method.h"

XDAQ_INSTANTIATOR_IMPL(emu::fed::Commander)

emu::fed::Commander::Commander(xdaq::ApplicationStub *stub):
xdaq::WebApplication(stub),
Application(stub)
{
	// Variables that are to be made available to other applications
	getApplicationInfoSpace()->fireItemAvailable("xmlFileName", &xmlFile_);
	
	// HyperDAQ pages
	xgi::bind(this, &emu::fed::Commander::webDefault, "Default");
	//xgi::bind(this, &emu::fed::Commander::getAJAX, "getAJAX");
}



void emu::fed::Commander::webDefault(xgi::Input *in, xgi::Output *out)
{
}



void emu::fed::Commander::configure()
throw(emu::fed::exception::SoftwareException)
{
}
