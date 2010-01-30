// $Id: SOAPLoopbackMessenger.cc,v 1.1 2010/01/30 15:53:19 banicz Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2009, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#include "pt/SOAPListener.h"
#include "pt/http/SOAPLoopbackMessenger.h"
#include "pt/http/PeerTransportSender.h"
#include "pt/http/Utils.h"
#include "xoap/MessageReference.h"
#include "xoap/MessageFactory.h"
#include "xoap/exception/Exception.h"
#include "xoap/SOAPBody.h"
#include "xoap/SOAPMessage.h"
#include "xoap/SOAPFault.h"
#include "xoap/SOAPEnvelope.h"

pt::http::SOAPLoopbackMessenger::SOAPLoopbackMessenger(Logger & logger, pt::Address::Reference destination, pt::Address::Reference local) :
	logger_(logger)
{
	// Check for address correct already done in PeerTRansportSender::getMessenger()
	//
	//destination_ = dynamic_cast<http::Address*>(destination);
	//local_ = dynamic_cast<http::Address*>(local);
	
	destination_ = destination;
	local_ = local;
}

pt::http::SOAPLoopbackMessenger::~SOAPLoopbackMessenger()
{
}

pt::Address::Reference pt::http::SOAPLoopbackMessenger::getLocalAddress()
{
	return local_;
}

pt::Address::Reference pt::http::SOAPLoopbackMessenger::getDestinationAddress()
{
	return destination_;
}

xoap::MessageReference pt::http::SOAPLoopbackMessenger::send (xoap::MessageReference message) throw (pt::exception::Exception)
{
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

