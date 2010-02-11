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

#include "pt/appweb/SOAPLoopbackMessenger.h"
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

#include "appweb/appweb.h"

pt::appweb::SOAPLoopbackMessenger::SOAPLoopbackMessenger(Logger &logger, pt::Address::Reference destination, pt::Address::Reference local) 
throw (pt::appweb::exception::Exception ) :
	logger_(logger)
{
	// Check for address correct already done in PeerTransportSender::getMessenger()
	//
	destination_ = destination;
	local_ = local;
}

pt::appweb::SOAPLoopbackMessenger::~SOAPLoopbackMessenger()
{
}

pt::Address::Reference pt::appweb::SOAPLoopbackMessenger::getLocalAddress()
{
	return local_;
}

pt::Address::Reference pt::appweb::SOAPLoopbackMessenger::getDestinationAddress()
{
	return destination_;
}

xoap::MessageReference pt::appweb::SOAPLoopbackMessenger::send (xoap::MessageReference message) 
	throw (pt::exception::Exception)
{
	std::string requestBuffer;

	std::multimap<std::string, std::string, std::less<std::string> >& headers = message->getMimeHeaders()->getAllHeaders();
	std::multimap<std::string, std::string, std::less<std::string> >::iterator i=headers.find("SOAPAction");
	if(i!=headers.end())
		message->getMimeHeaders()->setHeader("Content-Location" , (*i).second);

	pt::SOAPListener* l = dynamic_cast<pt::SOAPListener*>(pt::getPeerTransportAgent()->getListener("soap"));
	try
	{
		return l->processIncomingMessage(message);
	}
	catch (pt::exception::Exception& pte)
	{
		xoap::MessageReference reply = xoap::createMessage();
		xoap::SOAPBody b = reply->getSOAPPart().getEnvelope().getBody();
		xoap::SOAPFault f = b.addFault();
		f.setFaultCode ("Server");
		f.setFaultString (pte.what());
		return reply;
	}
	catch (...)
	{
		XCEPT_RAISE (pt::exception::Exception, "Caught unknown exception raised during local SOAP messaging");
	}
}

