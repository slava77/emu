// $Id: SOAPMessenger.cc,v 1.3 2011/01/25 18:32:18 banicz Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2009, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#include<map>

#include "pt/http/SOAPMessenger.h"
#include "pt/http/PeerTransportSender.h"
#include "pt/http/Utils.h"
#include "pt/http/Address.h"
#include "xoap/MessageReference.h"
#include "xoap/MessageFactory.h"
#include "xoap/exception/Exception.h"
#include "pt/http/exception/Exception.h"

pt::http::SOAPMessenger::SOAPMessenger(Logger & logger, pt::Address::Reference destination, pt::Address::Reference local, unsigned long httpResponseTimeoutSec) 
throw (pt::http::exception::Exception ) :
	logger_(logger), httpResponseTimeoutSec_(httpResponseTimeoutSec)
{
	// Check for address correct already done in PeerTRansportSender::getMessenger()
	//
	//destination_ = dynamic_cast<http::Address>(destination);
	destination_ = destination;
	local_ = local;
	
	try 
	{
		//local_ = dynamic_cast<http::Address*>(local);
	        channel_ = new http::ClientChannel(destination_, httpResponseTimeoutSec_);
	}
	catch (pt::http::exception::Exception & e )
	{
		XCEPT_RETHROW(pt::http::exception::Exception, "cannot create SOAP messenger", e);
	}
}

pt::http::SOAPMessenger::~SOAPMessenger()
{
	delete channel_;
}

pt::Address::Reference pt::http::SOAPMessenger::getLocalAddress()
{
	return local_;
}

pt::Address::Reference pt::http::SOAPMessenger::getDestinationAddress()
{
	return destination_;
}

xoap::MessageReference pt::http::SOAPMessenger::send (xoap::MessageReference message) 
	throw (pt::exception::Exception)
{
	std::string requestBuffer;
		
	try
	{
		// serialize message
		message->writeTo (requestBuffer);
 	}
	catch (xoap::exception::Exception& e)
	{		
		// Invalid request, cannot handle it
		XCEPT_RETHROW(pt::exception::Exception, "SOAP request message invalid, cannot be serialized", e); 	
	}
		
	size_t size = requestBuffer.size();
	
	// WRITE TO SOCKET
	char* replyBuffer = 0;
	
	// Fill the headers
	std::multimap<std::string, std::string, std::less<std::string> >& allHeaders = message->getMimeHeaders()->getAllHeaders();
	std::multimap<std::string, std::string, std::less<std::string> >::iterator i;
	
	std::string headers = "";
	for (i = allHeaders.begin(); i != allHeaders.end(); i++)
	{
		headers += (*i).first;
		headers += ": ";
		headers += (*i).second;
		headers += "\r\n";
	}
	
	channel_->lock();
	try
	{
		// Connect is intelligent, tries to connect only if socket not already connected
		//
		channel_->connect();
		
		http::Address& d = dynamic_cast<http::Address&>(*destination_);
		
		http::Utils::sendTo( channel_, 
				(char*)d.getPath().c_str(),
				(char*)d.getHost().c_str(),
				(char*)d.getPort().c_str(),
				(char*)requestBuffer.c_str(), 
				size,
				(char*) headers.c_str() );
	
		size = 0;
		std::string header;
		replyBuffer = http::Utils::receiveFrom ( channel_, &size, &header);
		channel_->disconnect();
	}
	catch (pt::http::exception::Exception& httpe)
	{
		if (replyBuffer != 0) delete replyBuffer;
		channel_->unlock();
		std::stringstream msg;
		msg << "SOAP request to " << destination_->toString() << " failed";
		XCEPT_RETHROW (pt::exception::Exception, msg.str(), httpe);
	}
	channel_->unlock();
	
	if (size == 0)
	{
		if (replyBuffer != 0)
			delete replyBuffer;

		xoap::MessageReference reply(0);
		return reply;
	}

	try
	{
		xoap::MessageReference reply = xoap::createMessage(replyBuffer , size);
		delete replyBuffer;			
		return reply;
	} 
	catch (xoap::exception::Exception& e)
	{
		delete replyBuffer;
		// Invalid reply, cannot handle it
		XCEPT_RETHROW(pt::exception::Exception, "SOAP reply cannot be processed, may be corrupted", e); 	
	}
}

