/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2007, CERN.                                        *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber, L. Orsini, R. Moser                             *
 *                                                                       *
 * For the licensing terms see LICENSE.                                  *
 * For the list of contributors see CREDITS.                             *
 *************************************************************************/

#ifndef _pt_appweb_PeerTransportReceiver_h
#define _pt_appweb_PeerTransportReceiver_h

#include <string>
#include <vector>

#include "pt/appweb/Address.h"
#include "pt/appweb/Alias.h"

#include "pt/SOAPListener.h"
#include "pt/PeerTransportReceiver.h"
#include "pt/HTAccessSecurityPolicy.h"
#include "pt/exception/Exception.h"
#include "xgi/Listener.h"
#include "xgi/Output.h"
#include "xdata/InfoSpace.h"
#include "xdata/UnsignedInteger32.h"
#include "xdata/Boolean.h"

#include "log4cplus/logger.h"

#include "appweb/appweb.h"

using namespace log4cplus;

namespace pt
{

namespace appweb
{

//! There is only one peer transport receiver. It has a number of ReceiverLoop
//! objects, one per receiver port
//
class PeerTransportReceiver:
	public pt::PeerTransportReceiver,
	public xdata::ActionListener
{
	public:
	
	PeerTransportReceiver(Logger & logger, xdata::InfoSpace * is);
	~PeerTransportReceiver();

	void actionPerformed ( xdata::Event& e );

	pt::TransportType getType();
	
	pt::Address::Reference createAddress( const std::string& url, const std::string& service  )
		throw (pt::exception::InvalidAddress);
		
	pt::Address::Reference createAddress( std::map<std::string, std::string, std::less<std::string> >& address )
		throw (pt::exception::InvalidAddress);
	
	void addServiceListener (pt::Listener* listener) throw (pt::exception::Exception);
	void removeServiceListener (pt::Listener* listener ) throw (pt::exception::Exception);
	void removeAllServiceListeners();

	int flush(MaRequest *rq, xgi::Output *out);
	
	std::string getProtocol();
	
	std::vector<std::string> getSupportedServices();	
	
	bool isServiceSupported(const std::string& service );
	
	void config (pt::Address::Reference address) throw (pt::exception::Exception);

	pt::HTAccessSecurityPolicy* getAccessSecurityPolicy();		

	protected:

	Logger logger_;
	xdata::InfoSpace * is_;

	pt::SOAPListener* listener_;
	xgi::Listener* cgiListener_;

	MaHttp *appweb;
	MaServer  *server;

	MaRomFileSystem *romFileSystem_;

	std::string documentRoot_;

	xdata::Vector< xdata::Bag<Alias> >* aliases_;
	uint32_t threads_;
	Mpr *runtime;
	pt::HTAccessSecurityPolicy *  policy_;
};

}

}

#endif

