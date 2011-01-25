// $Id: PeerTransportReceiver.cc,v 1.2 2011/01/25 17:36:47 banicz Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2009, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#include "pt/http/PeerTransportReceiver.h"
#include <iostream>
#include "xgi/Listener.h"

pt::http::PeerTransportReceiver::PeerTransportReceiver( xdaq::Application* owner, Logger & logger, xdata::InfoSpace* is)
	:xdaq::Object(owner), logger_(logger), is_(is)
{
}

pt::http::PeerTransportReceiver::~PeerTransportReceiver()
{	
	// Remove all messengers. At this point none of them
	// should be touched by a user application anymore
	
	for (std::vector<http::ReceiverLoop*>::size_type i = 0; i < loop_.size(); i++)
	{
		// stop and delete the tasks
#warning "Missing implementation for stopping and deleting receiver tasks."
		delete loop_[i];
	}
	
}

pt::TransportType pt::http::PeerTransportReceiver::getType()
{
	return pt::Receiver;
}

pt::Address::Reference pt::http::PeerTransportReceiver::createAddress( const std::string& url, const std::string & service )
throw (pt::exception::InvalidAddress)
{
	// newly created address pointer is ref counted and goes out of scope when not used anymore
	// function throws already correct exception
	return pt::Address::Reference(new http::Address(url,service));
}

pt::Address::Reference 
pt::http::PeerTransportReceiver::createAddress( std::map<std::string, std::string, std::less<std::string> >& address )
throw (pt::exception::InvalidAddress)
{
	std::string protocol = address["protocol"];
	
	if (protocol == "http")
	{
		std::string url = protocol;
		
		XCEPT_ASSERT (address["hostname"] != "",pt::exception::InvalidAddress , "Cannot create address, hostname not specified");
		XCEPT_ASSERT (address["port"] != "",pt::exception::InvalidAddress , "Cannot create address, port number not specified");
		
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

std::string pt::http::PeerTransportReceiver::getProtocol()
{
	return "http";
}

std::vector<std::string> pt::http::PeerTransportReceiver::getSupportedServices()
{
	std::vector<std::string> s;
	s.push_back("soap");
	s.push_back("cgi");
	// in the future also have a http "message" service for GET and POST operations
	return s;
}

bool pt::http::PeerTransportReceiver::isServiceSupported(const std::string& service )
{
	if (service == "soap") return true;
	if (service == "cgi") return true;
	return false;
}


void pt::http::PeerTransportReceiver::config( pt::Address::Reference address) throw (pt::exception::Exception)
{
	// check if a receiver 
	for (std::vector<http::ReceiverLoop*>::size_type i = 0; i < loop_.size(); i++)
	{
		if (loop_[i]->getAddress()->equals(address))
		{
			std::string msg = "Receiver for given address already configured: ";
			msg += address->toString();
			XCEPT_RAISE (pt::exception::Exception, msg);
		}
	}
	
	// Create a new receiver loop
	http::ReceiverLoop* loop = new http::ReceiverLoop(this->getOwnerApplication(),address, logger_, is_);
	
	// attempt to add listener if present
	if ( this->isExistingListener("soap") ) 
	{
		// I can safely get it now
		pt::SOAPListener* l = dynamic_cast<pt::SOAPListener*>(this->getListener("soap"));
	
		try
		{
			loop->addServiceListener(l);
		}
		catch(pt::exception::Exception & ex1)
		{
			// Fatal error, it should never happen
			throw ex1;
		}
	}
	
	// attempt to add listener if present
	if ( this->isExistingListener("cgi") ) 
	{
		// I can safely get it now
		xgi::Listener* l = dynamic_cast<xgi::Listener*>(this->getListener("cgi"));
	
		try
		{
			loop->addServiceListener(l);
		}
		catch(pt::exception::Exception & ex2)
		{
			// Fatal error, it should never happen
			throw ex2;
		}
	}
	loop_.push_back(loop);	

	// activate receiver loop as a thread at config point
	try 
	{
		loop->activate();
	} catch (...)
	{
		std::string msg = "Activation of HTTP receiver thread failed.";
		XCEPT_RAISE (pt::exception::Exception, msg);
	}
}

// Overriding function to asynchrounously set the listener
//
void pt::http::PeerTransportReceiver::addServiceListener (pt::Listener* listener) throw (pt::exception::Exception)
{
	pt::PeerTransportReceiver::addServiceListener(listener);
	for (std::vector<http::ReceiverLoop*>::size_type i = 0; i < loop_.size(); i++)
	{
		loop_[i]->addServiceListener(listener);
	}
}

void pt::http::PeerTransportReceiver::removeServiceListener (pt::Listener* listener ) throw (pt::exception::Exception)
{
	pt::PeerTransportReceiver::removeServiceListener(listener);
	for (std::vector<http::ReceiverLoop*>::size_type  i = 0; i < loop_.size(); i++)
	{
		loop_[i]->removeServiceListener(listener);
	}
}

void pt::http::PeerTransportReceiver::removeAllServiceListeners()
{
	pt::PeerTransportReceiver::removeAllServiceListeners();
	for (std::vector<http::ReceiverLoop*>::size_type i = 0; i < loop_.size(); i++)
	{
		loop_[i]->removeAllServiceListeners();
	}
}
