/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2007, CERN.                                        *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber, L. Orsini, R. Moser                             *
 *                                                                       *
 * For the licensing terms see LICENSE.                                  *
 * For the list of contributors see CREDITS.                             *
 *************************************************************************/

#ifndef _pt_appweb_Application_h_
#define _pt_appweb_Application_h_

#include "xdaq/Application.h"
#include "xdata/String.h"
#include "xdata/Boolean.h"

#include "pt/appweb/Alias.h"
#include "pt/appweb/PeerTransportSender.h"
#include "pt/appweb/PeerTransportReceiver.h"

namespace pt
{

namespace appweb
{

//! this is the XDAQ Peer Transport Appliction Wrapper
//
class Application: public xdaq::Application
{
	public:
	
	XDAQ_INSTANTIATOR();

	Application(xdaq::ApplicationStub * s) throw(xdaq::exception::Exception);
	virtual ~Application();
	
	private:
	
	pt::appweb::PeerTransportSender* pts_;
	pt::appweb::PeerTransportReceiver* ptr_;
	
	xdata::UnsignedInteger32 threads_;
	xdata::UnsignedInteger32 maxBody_;
	xdata::String documentRoot_;
	xdata::Vector< xdata::Bag<Alias> > aliases_;
};

}

}

#endif

