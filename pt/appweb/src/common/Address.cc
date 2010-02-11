/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2007, CERN.                                        *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber, L. Orsini, R. Moser                             *
 *                                                                       *
 * For the licensing terms see LICENSE.                                  *
 * For the list of contributors see CREDITS.                             *
 *************************************************************************/

#include <iostream>
#include <sstream>

#include "pt/appweb/Address.h"

#include "toolbox/net/URL.h"
#include "toolbox/net/Utils.h"
#include "toolbox/exception/Exception.h"
#include "toolbox/net/exception/MalformedURL.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

pt::appweb::Address::Address(const std::string& url, const std::string & service)
throw (pt::exception::InvalidAddress) 
{
	try
	{
		url_ = new toolbox::net::URL(url);
	} catch (toolbox::net::exception::MalformedURL& mfu)
	{
		XCEPT_RETHROW (pt::exception::InvalidAddress, "Cannot create appweb address", mfu);
	}
	
	if ((url_->getProtocol()) != "http")
	{
		std::string msg = "Cannot create pt::appweb address from url ";
		msg += url;
		msg += ", unsupported protocol";
		XCEPT_RAISE (pt::exception::InvalidAddress, msg.c_str());
	}
	
	if ( ( service != "soap" ) && ( service != "cgi" ) )
	{
		std::string msg = "Cannot create pt::appweb address from url ";
		msg += url;
		msg += ", unsupported service ";
		msg += service;
		XCEPT_RAISE (pt::exception::InvalidAddress, msg.c_str());
	}
	
	service_ = service;	
}

pt::appweb::Address::~Address()
{
	delete url_;
}

std::string pt::appweb::Address::resolve() throw (pt::exception::InvalidAddress)
{
	std::string ip;

  struct sockaddr_in writeAddr;
  try 
  {
    writeAddr = toolbox::net::getSocketAddressByName (url_->getHost(), url_->getPort());
		char buf[16];
		inet_ntop(AF_INET, &writeAddr.sin_addr, buf, sizeof(buf));
		ip=buf;
  }
	catch (toolbox::exception::Exception& e)
	{
		XCEPT_RETHROW (pt::exception::InvalidAddress, "Cannot retrieve socket address from http address", e);
	}

	return ip;
}

std::string pt::appweb::Address::getService()
{
	return service_;
}

std::string pt::appweb::Address::getProtocol()
{
	return url_->getProtocol();
}

std::string pt::appweb::Address::toString()
{
	return url_->toString();
}

std::string pt::appweb::Address::getURL()
{
	return url_->toString();
}

std::string pt::appweb::Address::getHost()
{
	return url_->getHost();
}

std::string pt::appweb::Address::getPort()
{
    std::ostringstream o;
    if(url_->getPort() > 0)
        o <<  url_->getPort();
    return o.str();
}

std::string pt::appweb::Address::getPath()
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

std::string pt::appweb::Address::getServiceParameters()
{
	return url_->getPath();
}

bool pt::appweb::Address::equals( pt::Address::Reference address )
{
	return (( this->toString() == address->toString()) && ( this->getService() == address->getService()));
}

