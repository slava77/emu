/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2007, CERN.                                        *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber, L. Orsini, R. Moser                             *
 *                                                                       *
 * For the licensing terms see LICENSE.                                  *
 * For the list of contributors see CREDITS.                             *
 *************************************************************************/

#include "pt/appweb/PeerTransportSender.h"
#include "pt/appweb/SOAPMessenger.h"
#include "pt/appweb/SOAPLoopbackMessenger.h"
#include "pt/appweb/Address.h"

#include <string>

pt::appweb::PeerTransportSender::PeerTransportSender(Logger & logger, xdata::InfoSpace* is)
  : logger_(logger), is_(is)
{
}

pt::appweb::PeerTransportSender::~PeerTransportSender()
{
}

pt::Messenger::Reference pt::appweb::PeerTransportSender::getMessenger (pt::Address::Reference destination, pt::Address::Reference local)
	throw (pt::exception::UnknownProtocolOrService)
{
	// Look if a messenger from the local to the remote destination exists and return it.	
	// If it doesn't exist, create it and return it.
	// It accept the service to be null, in this case, it assume that it is soap service. Of course this is not secure but it provide a
	// useful omission as default
	if (((destination->getService()  == "soap")) && ((local->getService() == "soap")) &&
		(destination->getProtocol()  == "http") && (local->getProtocol() == "http"))
	{
		if (destination->equals(local))
		{
			// create a local messenger
			pt::appweb::SOAPLoopbackMessenger* m = new pt::appweb::SOAPLoopbackMessenger(logger_, destination, local);
			return pt::Messenger::Reference(m);
		}
		else
		{
			try 
			{	
				// create a remote messenger
				pt::appweb::SOAPMessenger* m = new pt::appweb::SOAPMessenger(logger_, destination, local);
				return pt::Messenger::Reference(m);
			}
			catch(pt::appweb::exception::Exception & e)
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


pt::TransportType pt::appweb::PeerTransportSender::getType()
{
	return pt::Sender;
}

pt::Address::Reference 
pt::appweb::PeerTransportSender::createAddress( const std::string& url, const std::string& service )
throw (pt::exception::InvalidAddress)
{
	// url looks as follows: http://<hostname>:<port>
	

	// Safe: The newly created pointer is reference counted and goes out of scope when not used anymore
	return pt::Address::Reference(new pt::appweb::Address(url,service));
}

pt::Address::Reference 
pt::appweb::PeerTransportSender::createAddress( std::map<std::string, std::string, std::less<std::string> >& address )
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

std::string pt::appweb::PeerTransportSender::getProtocol()
{
	return "http";
}

std::vector<std::string> pt::appweb::PeerTransportSender::getSupportedServices()
{
	std::vector<std::string> s;
	s.push_back("soap");
	return s;
}

bool pt::appweb::PeerTransportSender::isServiceSupported(const std::string& service )
{
	if (service == "soap") return true;
	else return false;
}
