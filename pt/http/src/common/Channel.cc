// $Id: Channel.cc,v 1.1 2010/01/30 15:53:19 banicz Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2009, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#include "pt/http/Channel.h"
#include "pt/http/Address.h"
#include "pt/http/exception/Exception.h"

#include <string.h>
#include <string>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include "config/PackageInfo.h"

pt::http::Channel::Channel(pt::Address::Reference address) throw (pt::http::exception::Exception): mutex_(toolbox::BSem::FULL)
{	
	http::Address& a = dynamic_cast<http::Address&>(*address);
		
	try
	{
		sockaddress_ = a.getSocketAddress();
	}
	catch(pt::exception::InvalidAddress & e)
	{
		XCEPT_RETHROW(pt::http::exception::Exception, "cannot create channel", e);
	}
	
	sockaddressSize_ = sizeof (sockaddress_);
	
	errno = 0;
	socket_ = socket(AF_INET, SOCK_STREAM, 0);
   	if (socket_ == -1)
  	{
		XCEPT_RAISE (pt::http::exception::Exception, strerror(errno));
	}
	int optval = 1;
	errno = 0;
   	if (setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval)) < 0)
  	{
		XCEPT_RAISE (pt::http::exception::Exception, strerror(errno));
  	}
	
#if OS_VERSION_CODE < OS_VERSION(2,6,0)
#warning "Linux Kernel version smaller than 2.6, using TCP_NODELAY flag for HTTP communication"

	if (setsockopt(socket_, IPPROTO_TCP, TCP_NODELAY, (char *)&optval, sizeof(optval)) < 0)
	{
		XCEPT_RAISE (pt::http::exception::Exception, strerror(errno));
	}
#endif
}

void pt::http::Channel::lock()
{
	mutex_.take();
}

void pt::http::Channel::unlock()
{
	mutex_.give();
}


pt::http::Channel::~Channel() 
{
	if (socket_ > 0) ::close(socket_);
}


