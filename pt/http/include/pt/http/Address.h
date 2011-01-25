// $Id: Address.h,v 1.2 2011/01/25 17:36:47 banicz Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2009, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _pt_http_Address_h
#define _pt_http_Address_h

#include "pt/Address.h"
#include "toolbox/net/URL.h"
#include <netinet/in.h>
#include "pt/exception/InvalidAddress.h"

namespace pt
{
namespace http
{

class Address: public pt::Address
{
	public:
	
	//! Create address from url
	Address (const std::string& url, const std::string & service) throw (pt::exception::InvalidAddress);
	
	virtual ~Address ();
	
	//! Get the URL protocol, e.g. http
	std::string getProtocol();
	
	//! Get the URL service if it exists, e.g. SyncSOAP
	std::string getService();
	
	//! Get additional parameters at the end of the URL
	std::string getServiceParameters();
	
	//! Return the URL in string form
	std::string toString();
	
	//! Get the host part of the url
	std::string getHost();
	
	//! Get the port number of the url if it exists
	std::string getPort();
	
	//! Get the url provided in the constructor
	std::string getURL();
	
	//! Get the path
	std::string getPath();
	
	//! Get the IP representation of the hostname/port address combination
	struct sockaddr_in getSocketAddress() throw (pt::exception::InvalidAddress);

	//! Compare with another address
	bool equals( pt::Address::Reference address );

	protected:
	
	toolbox::net::URL * url_;
	std::string service_;
};

}
}
#endif
