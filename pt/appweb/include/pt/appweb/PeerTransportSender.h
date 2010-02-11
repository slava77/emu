/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2007, CERN.                                        *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber, L. Orsini, R. Moser                             *
 *                                                                       *
 * For the licensing terms see LICENSE.                                  *
 * For the list of contributors see CREDITS.                             *
 *************************************************************************/

#ifndef _pt_appweb_PeerTransportSender_h
#define _pt_appweb_PeerTransportSender_h

#include "pt/appweb/SOAPMessenger.h"

#include "pt/PeerTransportSender.h"
#include "pt/PeerTransportReceiver.h"
#include "pt/PeerTransportAgent.h"
#include "xdata/InfoSpace.h"
#include "xdata/Boolean.h"

#include "log4cplus/logger.h"

using namespace log4cplus;

namespace pt
{

namespace appweb
{

class PeerTransportSender: public pt::PeerTransportSender
{
	public:
	
	PeerTransportSender(Logger & logger, xdata::InfoSpace *is);
	
	virtual ~PeerTransportSender();
	
	pt::TransportType getType();
	
	pt::Address::Reference createAddress( const std::string& url, const std::string& service )
		throw (pt::exception::InvalidAddress);
	
	pt::Address::Reference createAddress( std::map<std::string, std::string, std::less<std::string> >& address )
		throw (pt::exception::InvalidAddress);
	
	std::string getProtocol();
	
	std::vector<std::string> getSupportedServices();
	
	bool isServiceSupported(const std::string& service );
	
	pt::Messenger::Reference getMessenger (pt::Address::Reference destination, pt::Address::Reference local)
		throw (pt::exception::UnknownProtocolOrService);
	
	protected:
		
	Logger logger_;
	xdata::InfoSpace * is_;
};

}

}

#endif
