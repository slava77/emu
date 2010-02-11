/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2007, CERN.                                        *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber, L. Orsini, R. Moser                             *
 *                                                                       *
 * For the licensing terms see LICENSE.                                  *
 * For the list of contributors see CREDITS.                             *
 *************************************************************************/

#ifndef _pt_appweb_SOAPMessenger_h
#define _pt_appweb_SOAPMessenger_h

#include "pt/appweb/exception/Exception.h"

#include "pt/SOAPMessenger.h"
#include "pt/exception/Exception.h"
#include "pt/Address.h"
#include "xdata/Boolean.h"
#include "xoap/MessageReference.h"
#include "log4cplus/logger.h"

using namespace log4cplus;

namespace pt
{

namespace appweb
{

class SOAPMessenger: public pt::SOAPMessenger
{
	public:
			
		SOAPMessenger(Logger &logger, pt::Address::Reference destination, pt::Address::Reference local)
		    throw (appweb::exception::Exception);
			
		//! Destructor only deletes channel object
		//
		virtual ~SOAPMessenger();
			
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

