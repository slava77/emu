// $Id: PeerTransportHTTP.cc,v 1.3 2011/01/25 18:32:18 banicz Exp $

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
	getApplicationInfoSpace()->fireItemAvailable("aliases",&aliases_);
	getApplicationInfoSpace()->fireItemAvailable("documentRoot", &documentRoot_);
	getApplicationInfoSpace()->fireItemAvailable("httpResponseTimeoutSec", &httpResponseTimeoutSec_);
	
	pts_ = new pt::http::PeerTransportSender(getApplicationLogger(), (unsigned long)(httpResponseTimeoutSec_) );
	ptr_ = new pt::http::PeerTransportReceiver(this,getApplicationLogger(), getApplicationInfoSpace());
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
