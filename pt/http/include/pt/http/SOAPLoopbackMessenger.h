// $Id: SOAPLoopbackMessenger.h,v 1.1 2010/01/30 15:53:06 banicz Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2009, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _pt_http_SOAPLoopbackMessenger_h
#define _pt_http_SOAPLoopbackMessenger_h

#include "xoap/MessageReference.h"
#include "pt/SOAPMessenger.h"
#include "pt/exception/Exception.h"
#include "pt/Address.h"
#include "pt/http/Channel.h"

#include "log4cplus/logger.h"

using namespace log4cplus;

namespace pt
{
namespace http
{

class SOAPLoopbackMessenger: public pt::SOAPMessenger
{
	public:
	
	SOAPLoopbackMessenger(Logger & logger, pt::Address::Reference destination, pt::Address::Reference local);

	//! Destructor only deletes channel object
	//
	virtual ~SOAPLoopbackMessenger();
	
	pt::Address::Reference getLocalAddress();
	pt::Address::Reference getDestinationAddress();
	
	xoap::MessageReference send (xoap::MessageReference message) throw (pt::exception::Exception);
	
	private:
	
	pt::Address::Reference local_;
	pt::Address::Reference destination_;
	Logger logger_;
};

}
}

#endif
