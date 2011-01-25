// $Id: PeerTransportSender.cc,v 1.3 2011/01/25 18:32:18 banicz Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2009, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#include "pt/http/PeerTransportSender.h"
#include "pt/http/SOAPLoopbackMessenger.h"
#include "pt/http/SOAPMessenger.h"
#include "pt/http/Address.h"
#include "toolbox/BSem.h"
#include <string>

pt::http::PeerTransportSender::PeerTransportSender(Logger & logger, unsigned long httpResponseTimeoutSec): logger_(logger), httpResponseTimeoutSec_(httpResponseTimeoutSec)
{
	sync_  = new toolbox::BSem(toolbox::BSem::EMPTY);
	mutex_ = new toolbox::BSem(toolbox::BSem::FULL);
}

pt::http::PeerTransportSender::~PeerTransportSender()
{
	delete sync_;
	delete mutex_;
}

pt::Messenger::Reference pt::http::PeerTransportSender::getMessenger (pt::Address::Reference destination, pt::Address::Reference local)
	throw (pt::exception::UnknownProtocolOrService)
{
	// Look if a messenger from the local to the remote destination exists and return it.	
	// If it doesn't exist, create it and return it.
	// It accept the service to be null, in this case, it assume that it is soap service. Of course this is not secure but it provide a
	// useful omission as default
	if (((destination->getService()  == "soap")) && ((local->getService() == "soap")) &&
		(destination->getProtocol()  == "http") && (local->getProtocol() == "http"))
	{
		pt::http::Address & da = dynamic_cast<pt::http::Address &>(*destination);
		pt::http::Address & la = dynamic_cast<pt::http::Address &>(*local);
		if (( da.getHost() == la.getHost() ) && ( da.getPort() == la.getPort()) )
//		if (destination->equals(local))
		{
			// create a local messenger
			http::SOAPLoopbackMessenger* m = new http::SOAPLoopbackMessenger(logger_, destination, local);
			return pt::Messenger::Reference(m);
		}
		else 
		{	
			try 
			{	
				// create remote messenger
			        http::SOAPMessenger* m = new http::SOAPMessenger(logger_, destination, local, httpResponseTimeoutSec_ );
				return pt::Messenger::Reference(m);
			}
			catch(pt::http::exception::Exception & e)
			{
			       XCEPT_RETHROW(pt::exception::UnknownProtocolOrService, "cannot create SOAP messenger", e);
			}
		}
	} else
	{
		std::string msg = "Cannot handle protocol service combination, destination protocol was :";
		msg +=  destination->getProtocol();
		msg += " destination service was:";
		msg +=  destination->getService();
		msg += " while local protocol was:";
		msg += local->getProtocol();
		msg += "and local service was:";
		msg += local->getService();
		
		XCEPT_RAISE(pt::exception::UnknownProtocolOrService,msg);
	}
}


pt::TransportType pt::http::PeerTransportSender::getType()
{
	return pt::Sender;
}

pt::Address::Reference 
pt::http::PeerTransportSender::createAddress( const std::string& url, const std::string& service )
throw (pt::exception::InvalidAddress)
{
	// url looks as follows: http://<hostname>:<port>
	

	// Safe: The newly created pointer is reference counted and goes out of scope when not used anymore
	return pt::Address::Reference(new http::Address(url,service));
}

pt::Address::Reference 
pt::http::PeerTransportSender::createAddress( std::map<std::string, std::string, std::less<std::string> >& address )
throw (pt::exception::InvalidAddress)
{
	std::string protocol = address["protocol"];
	
	if (protocol == "http")
	{
		std::string url = protocol;
		
		XCEPT_ASSERT (address["hostname"] != "", pt::exception::InvalidAddress, "Cannot create address, hostname not specified");
		XCEPT_ASSERT (address["port"] != "", pt::exception::InvalidAddress, "Cannot create address, port number not specified");
		
		url += "://";
		url += address["hostname"];
		url += ":";
		url += address["port"];
		
		std::string service = address["service"];
		if (service != "")
		{
			if (!this->isServiceSupported(service))
			{
				std::string msg = "Cannot create address, specified service for protocol ";
				msg += protocol;
				msg += " not supported: ";
				msg += service;
				XCEPT_RAISE(pt::exception::InvalidAddress, msg);
			}
			
			//url += "/";
			//url += service;
		}
		else
		{
			std::string msg = "Cannot create address, service for protocol ";
			msg += protocol;
			msg += " not specified";
			XCEPT_RAISE(pt::exception::InvalidAddress, msg);
		}
		
		// throws already pt::exception::InvalidAddress, will be chained up
		return this->createAddress(url,service);
	}
	else 
	{
		std::string msg = "Cannot create address, protocol not supported: ";
		msg += protocol;
		XCEPT_RAISE(pt::exception::InvalidAddress, msg);
	}	
}

std::string pt::http::PeerTransportSender::getProtocol()
{
	return "http";
}

std::vector<std::string> pt::http::PeerTransportSender::getSupportedServices()
{
	std::vector<std::string> s;
	s.push_back("soap");
	return s;
}

bool pt::http::PeerTransportSender::isServiceSupported(const std::string& service )
{
	if (service == "soap") return true;
	else return false;
}
