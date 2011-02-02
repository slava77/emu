// $Id: PeerTransportSender.h,v 1.4 2011/02/02 12:52:30 banicz Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2009, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _pt_http_PeerTransportSender_h
#define _pt_http_PeerTransportSender_h

#include "pt/PeerTransportSender.h"
#include "pt/PeerTransportReceiver.h"
#include "pt/PeerTransportAgent.h"
#include "pt/http/SOAPMessenger.h"

#include "toolbox/BSem.h"


#include "log4cplus/logger.h"

using namespace log4cplus;

namespace pt 
{
namespace http
{

class PeerTransportSender: public pt::PeerTransportSender
{
	public:
	
        PeerTransportSender(Logger & logger, xdata::UnsignedLong* httpResponseTimeoutSec);
	
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
		
 	toolbox::BSem * sync_;
	toolbox::BSem * mutex_;
	Logger logger_;
        xdata::UnsignedLong* httpResponseTimeoutSec_;
};

}
}
#endif
