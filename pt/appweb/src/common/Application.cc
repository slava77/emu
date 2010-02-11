/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2007, CERN.                                        *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber, L. Orsini, R. Moser                             *
 *                                                                       *
 * For the licensing terms see LICENSE.                                  *
 * For the list of contributors see CREDITS.                             *
 *************************************************************************/

#include "pt/appweb/Application.h"

#include "pt/PeerTransportAgent.h"
#include "xdata/String.h"
#include "xdata/UnsignedInteger32.h"

XDAQ_INSTANTIATOR_IMPL(pt::appweb::Application)

pt::appweb::Application::Application(xdaq::ApplicationStub * s)
 throw(xdaq::exception::Exception)
 : xdaq::Application(s)
{
	getApplicationDescriptor()->setAttribute("icon","/pt/images/PeerTransport64x64.gif");
	getApplicationDescriptor()->setAttribute("icons","/pt/images");

	threads_      = 1;
	maxBody_      = 10000000;
	char *xdaqRoot = std::getenv("XDAQ_ROOT");
	if (xdaqRoot == (char*) 0)
		documentRoot_ = ".";
	else
		documentRoot_ = xdaqRoot;

	getApplicationInfoSpace()->fireItemAvailable("threads", &threads_);
	getApplicationInfoSpace()->fireItemAvailable("maxBody", &maxBody_);
	getApplicationInfoSpace()->fireItemAvailable("documentRoot", &documentRoot_);
	getApplicationInfoSpace()->fireItemAvailable("aliases", &aliases_);

	pts_ = new pt::appweb::PeerTransportSender(getApplicationLogger(), getApplicationInfoSpace());
	ptr_ = new pt::appweb::PeerTransportReceiver(getApplicationLogger(), getApplicationInfoSpace());
	pt::PeerTransportAgent* pta = pt::getPeerTransportAgent();
	pta->addPeerTransport(pts_);
	pta->addPeerTransport(ptr_);	
}

pt::appweb::Application::~Application()
{
	pt::PeerTransportAgent* pta = pt::getPeerTransportAgent();	
	pta->removePeerTransport(pts_);
	pta->removePeerTransport(ptr_);	
	delete pts_;
	delete ptr_;
}

