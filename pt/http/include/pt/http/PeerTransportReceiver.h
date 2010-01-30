// $Id: PeerTransportReceiver.h,v 1.1 2010/01/30 15:53:06 banicz Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2009, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _pt_http_PeerTransportReceiver_h
#define _pt_http_PeerTransportReceiver_h

#include <string>
#include <vector>

#include "pt/PeerTransportReceiver.h"
#include "pt/exception/Exception.h"
#include "pt/http/Address.h"
#include "pt/http/ReceiverLoop.h"


#include "log4cplus/logger.h"

using namespace log4cplus;

namespace pt
{
namespace http
{

//! There is only one peer transport receiver. It has a number of ReceiverLoop
//! objects, one per receiver port
//
class PeerTransportReceiver: public pt::PeerTransportReceiver
{
	public:
	
	PeerTransportReceiver(Logger & logger, xdata::InfoSpace * is);
	~PeerTransportReceiver();
	
	pt::TransportType getType();
	
	pt::Address::Reference createAddress( const std::string& url, const std::string& service  )
		throw (pt::exception::InvalidAddress);
		
	pt::Address::Reference createAddress( std::map<std::string, std::string, std::less<std::string> >& address )
		throw (pt::exception::InvalidAddress);
	
	void addServiceListener (pt::Listener* listener) throw (pt::exception::Exception);
	void removeServiceListener (pt::Listener* listener ) throw (pt::exception::Exception);
	void removeAllServiceListeners();
	
	std::string getProtocol();
	
	std::vector<std::string> getSupportedServices();	
	
	bool isServiceSupported(const std::string& service );
	
	void config (pt::Address::Reference address) throw (pt::exception::Exception);
		
	protected:
			
	std::vector<http::ReceiverLoop*> loop_;
	Logger logger_;
	xdata::InfoSpace * is_;
};

}
}
#endif
