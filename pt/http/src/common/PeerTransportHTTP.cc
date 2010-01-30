// $Id: PeerTransportHTTP.cc,v 1.1 2010/01/30 15:53:19 banicz Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2009, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#include "pt/http/PeerTransportHTTP.h"
#include "pt/PeerTransportAgent.h"

XDAQ_INSTANTIATOR_IMPL(pt::http::PeerTransportHTTP)

pt::http::PeerTransportHTTP::PeerTransportHTTP(xdaq::ApplicationStub * s)
 throw(xdaq::exception::Exception)
 : xdaq::Application(s)
{
	getApplicationDescriptor()->setAttribute("icon","/pt/images/PeerTransport64x64.gif");
	getApplicationDescriptor()->setAttribute("icons","/pt/images");
	
	getApplicationInfoSpace()->fireItemAvailable("aliasName",&aliasName_);
	getApplicationInfoSpace()->fireItemAvailable("aliasPath",&aliasPath_);	
	getApplicationInfoSpace()->fireItemAvailable("documentRoot", &documentRoot_);
	
	pts_ = new pt::http::PeerTransportSender(getApplicationLogger());
	ptr_ = new pt::http::PeerTransportReceiver(getApplicationLogger(), getApplicationInfoSpace());
	pt::PeerTransportAgent* pta = pt::getPeerTransportAgent();
	pta->addPeerTransport(pts_);
	pta->addPeerTransport(ptr_);	
}

pt::http::PeerTransportHTTP::~PeerTransportHTTP()
{
	pt::PeerTransportAgent* pta = pt::getPeerTransportAgent();	
	pta->removePeerTransport(pts_);
	pta->removePeerTransport(ptr_);	
	delete pts_;	
	delete ptr_;		
}
