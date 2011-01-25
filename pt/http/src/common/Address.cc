// $Id: Address.cc,v 1.2 2011/01/25 17:36:47 banicz Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2009, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#include <iostream>
#include <sstream>

#include "pt/http/Address.h"
#include "toolbox/net/URL.h"
#include "toolbox/net/Utils.h"
#include "toolbox/exception/Exception.h"
#include "toolbox/net/exception/MalformedURL.h"


// only for debugging inet_ntoa
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

pt::http::Address::Address(const std::string& url, const std::string & service)
throw (pt::exception::InvalidAddress) 
{
	try
	{
		url_ = new toolbox::net::URL(url);
	} catch (toolbox::net::exception::MalformedURL& mfu)
	{
		XCEPT_RETHROW (pt::exception::InvalidAddress, "Cannot create http address", mfu);
	}
	
	if ((url_->getProtocol()) != "http")
	{
		std::string msg = "Cannot create pt::http address from url ";
		msg += url;
		msg += ", unsupported protocol";
		XCEPT_RAISE (pt::exception::InvalidAddress, msg.c_str());
	}
	
	if ( ( service != "soap" ) && ( service != "cgi" ) )
	{
		std::string msg = "Cannot create pt::http address from url ";
		msg += url;
		msg += ", unsupported service ";
		msg += service;
		XCEPT_RAISE (pt::exception::InvalidAddress, msg.c_str());
	}
	
	service_ = service;	
}

pt::http::Address::~Address()
{
	delete url_;
}

struct sockaddr_in pt::http::Address::getSocketAddress() throw (pt::exception::InvalidAddress) 
{
	struct sockaddr_in writeAddr;
	try 
	{
		writeAddr = toolbox::net::getSocketAddressByName (url_->getHost(), url_->getPort());
	
		//cout << "port number " <<  writeAddr.sin_port << endl;
		//cout << "IP addressin host format: " << inet_ntoa(writeAddr.sin_addr) << endl;
	} catch (toolbox::exception::Exception& e)
	{
		XCEPT_RETHROW (pt::exception::InvalidAddress, "Cannot retrieve socket address from http address", e);
	}
	return writeAddr;
}


std::string pt::http::Address::getService()
{
	return service_;
}

std::string pt::http::Address::getProtocol()
{
	return url_->getProtocol();
}

std::string pt::http::Address::toString()
{
	return url_->toString();
}

std::string pt::http::Address::getURL()
{
	return url_->toString();
}

std::string pt::http::Address::getHost()
{
	return url_->getHost();
}

std::string pt::http::Address::getPort()
{
    std::ostringstream o;
    if(url_->getPort() > 0)
        o <<  url_->getPort();
    return o.str();
}

std::string pt::http::Address::getPath()
{
	std::string path = url_->getPath();
        if ( path.empty()  ) {	
		return "/";
	}
	else
	{
		if ( path[0] == '/' )
		{
			return path;
		}
		else
		{
			path.insert(0,"/");
			return path;
		}
				
	}
	
}

std::string pt::http::Address::getServiceParameters()
{
	return url_->getPath();
}

bool pt::http::Address::equals( pt::Address::Reference address )
{
	return (( this->toString() == address->toString()) && ( this->getService() == address->getService()));
}
