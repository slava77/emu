/*****************************************************************************\
* $Id: Commander.cc,v 1.4 2009/06/13 17:59:08 paste Exp $
\*****************************************************************************/
#include "emu/fed/Commander.h"

#include <iostream>
#include <iomanip>

#include "xgi/Method.h"
#include "emu/fed/VMEController.h"
#include "emu/fed/DDU.h"
#include "emu/fed/DCC.h"
#include "emu/fed/Crate.h"

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



emu::fed::Commander::~Commander()
{
	for (size_t iCrate = 0; iCrate < crateVector_.size(); iCrate++) {
		delete crateVector_[iCrate];
	}
}



void emu::fed::Commander::webDefault(xgi::Input *in, xgi::Output *out)
{
	
	configure();
	
}



void emu::fed::Commander::configure()
throw(emu::fed::exception::SoftwareException)
{

}
