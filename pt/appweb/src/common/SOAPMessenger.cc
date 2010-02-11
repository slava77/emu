/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2007, CERN.                                        *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber, L. Orsini, R. Moser                             *
 *                                                                       *
 * For the licensing terms see LICENSE.                                  *
 * For the list of contributors see CREDITS.                             *
 *************************************************************************/

#include<map>

#include "pt/appweb/SOAPMessenger.h"
#include "pt/appweb/Address.h"
#include "pt/appweb/PeerTransportSender.h"
#include "pt/appweb/exception/Exception.h"

#include "pt/SOAPListener.h"
#include "xoap/MessageReference.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPBody.h"
#include "xoap/SOAPMessage.h"
#include "xoap/SOAPFault.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/exception/Exception.h"

#include "toolbox/string.h"

#include "appweb/appweb.h"

pt::appweb::SOAPMessenger::SOAPMessenger(Logger &logger, pt::Address::Reference destination, pt::Address::Reference local) 
throw (pt::appweb::exception::Exception ) :
	logger_(logger)
{
	// Check for address correct already done in PeerTransportSender::getMessenger()
	//
	destination_ = destination;
	local_ = local;
}

pt::appweb::SOAPMessenger::~SOAPMessenger()
{
}

pt::Address::Reference pt::appweb::SOAPMessenger::getLocalAddress()
{
	return local_;
}

pt::Address::Reference pt::appweb::SOAPMessenger::getDestinationAddress()
{
	return destination_;
}

xoap::MessageReference pt::appweb::SOAPMessenger::send (xoap::MessageReference message) 
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
		
	int size = requestBuffer.size();

	MaClient client;

	std::multimap<std::string, std::string, std::less<std::string> >& allHeaders = message->getMimeHeaders()->getAllHeaders();
	std::multimap<std::string, std::string, std::less<std::string> >::iterator i;

	pt::appweb::Address& d = dynamic_cast<pt::appweb::Address&>(*destination_);
	MprBuf hdr;
	hdr.setBuf(MPR_HTTP_CLIENT_BUFSIZE, MPR_HTTP_MAX_BODY);
	hdr.putFmt("POST %s HTTP/1.1\r\n", d.getPath().c_str());
	hdr.putFmt("Host: %s:%s\r\n", d.getHost().c_str(), d.getPort().c_str());
	for (i = allHeaders.begin(); i != allHeaders.end(); i++)
	{
		hdr.putFmt("%s: %s\r\n", (*i).first.c_str(), (*i).second.c_str());
		LOG4CPLUS_DEBUG(logger_, "Header: '" << (*i).first.c_str() << ":" << (*i).second.c_str() << "'");
	}

	if (message->countAttachments() > 0)
	{
		std::string contentType = "multipart/related; type=\"application/soap+xml\"; boundary=\"";
		contentType += message->getMimeBoundary();
		contentType += "\"";
		hdr.putFmt("Content-Type: %s\r\n", contentType.c_str());
		hdr.putFmt("Content-Description: SOAP Message with attachments\r\n");
	}
	else
	{
		hdr.putFmt("Content-Type: application/soap+xml; charset=utf-8\r\n");
		hdr.putFmt("Content-Description: SOAP Message\r\n");
	}
	hdr.putFmt("Content-Length: %ld\r\n", size);
	hdr.putFmt("\r\n");

	std::stringstream ss;
	unsigned int port;
	ss << d.getPort();
	ss >> port;
	int rc = client.sendRequest((char*)d.resolve().c_str(), port, &hdr, (char*)requestBuffer.c_str(), size);
	if (rc < 0)
	{
		std::stringstream ss;
		if(rc > MPR_ERR_BASE)
		{
			// return code is the negated errno from a call to the socket or connect function
			ss << "Could not send SOAP request to \"" << d.toString() << "\" (errno=" << (long)(-rc) << ")";
		}
		else
		{
			// return code contains an appweb error code
			ss << "Could not send SOAP request to \"" << d.toString() << "\" (internal appweb error code=" << (long)rc << ")";
		}
		LOG4CPLUS_DEBUG (logger_, ss.str());
		XCEPT_RAISE(pt::exception::Exception, ss.str());
	}

	int code = client.getResponseCode();
	if (code < 0)
	{
		std::stringstream ss;
		if(code == -1)
		{
			// not even the first line in the http response could be parsed
			ss << "Could not parse incoming HTTP response \"" << d.toString() << "\" (internal appweb error code=" << (long)code << ")";
		}
		else
		{
			// return code contains an appweb error code
			ss << "Could not receive SOAP response from \"" << d.toString() << "\" (internal appweb error code=" << (long)code << ")";
		}
		LOG4CPLUS_DEBUG (logger_, ss.str());
		XCEPT_RAISE(pt::exception::Exception, ss.str());
	}
	else if (code != 200 && code !=202)
	{
		std::stringstream ss;
		ss << "SOAP request failed with HTTP code " << (long)code;
		LOG4CPLUS_DEBUG (logger_, ss.str());
		XCEPT_RAISE(pt::exception::Exception, ss.str());
	}

	char *content;
	content = client.getResponseContent(&size);

	// compare content-length header and real length of content
	char *cl = client.getHeaderVar("content-length");
	if (cl != NULL)
	{
		try
		{
			unsigned long contentLengthHeader = toolbox::toUnsignedLong(cl);
			if(contentLengthHeader != size)
			{
				// number of read characters does not match header information
				std::stringstream ss;
				ss << "Content-length mismatch between header (" << contentLengthHeader << ") and received content (" << size << ")";
				LOG4CPLUS_DEBUG (logger_, ss.str());
				XCEPT_RAISE(pt::exception::Exception, ss.str());
			}
		}
		catch( toolbox::exception::Exception & e)
		{
			// could not parse header properly
			std::stringstream ss;
			ss << "Content-length header of SOAP response could not be parsed (" << cl << ")";
			LOG4CPLUS_DEBUG (logger_, ss.str());
			XCEPT_RAISE(pt::exception::Exception, ss.str());
		}
	}

	if (size == 0)
	{
		xoap::MessageReference reply(0);
		return reply;
	}

	try
	{
		xoap::MessageReference reply = xoap::createMessage(content, size);
		return reply;
	} 
	catch (xoap::exception::Exception& e)
	{
		// Invalid reply, cannot handle it
		XCEPT_RETHROW(pt::exception::Exception, "SOAP reply cannot be processed, may be corrupted", e);   
	}
}

