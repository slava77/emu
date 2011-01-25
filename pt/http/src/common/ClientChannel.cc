// $Id: ClientChannel.cc,v 1.4 2011/01/25 18:32:18 banicz Exp $

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
#include "linux/version.h"
#include "config/PackageInfo.h"

/* Amount of seconds to wait after an HTTP request has be sent until a response byte is received */
#define HTTP_RESPONSE_TIMEOUT_SEC 30

pt::http::ClientChannel::ClientChannel(pt::Address::Reference address, unsigned long httpResponseTimeoutSec) 
	throw (pt::http::exception::Exception): http::Channel (address), httpResponseTimeoutSec_(httpResponseTimeoutSec)
{
	connected_ = false;
}

bool pt::http::ClientChannel::isConnected()  throw (pt::http::exception::Exception)
{
	return connected_;
}

void pt::http::ClientChannel::connect()  throw (pt::http::exception::Exception)
{
	if (!connected_)
	{
		if ( socket_ == -1) 
		{
			// re-create the soccket

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
			
			// Send and receive timeouts to avoid endless blocking in case of pathologic errors, hard to 5 seconds
			//
			struct timeval tv;
			//tv.tv_sec = HTTP_RESPONSE_TIMEOUT_SEC;
			tv.tv_sec = httpResponseTimeoutSec_;
			tv.tv_usec = 0;
			
			if (setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(tv)) < 0)
  			{
				XCEPT_RAISE (pt::http::exception::Exception, strerror(errno));
  			}
			
			//if (setsockopt(socket_, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(tv)) < 0)
  			//{
			//	XCEPT_RAISE (pt::http::exception::Exception, strerror(errno));
  			//}
			
#if OS_VERSION_CODE < OS_VERSION(2,6,0)
#warning "Linux Kernel version smaller than 2.6, using TCP_NODELAY flag for HTTP communication"

         		if (setsockopt(socket_, IPPROTO_TCP, TCP_NODELAY, (char *)&optval, sizeof(optval)) < 0)
			{
				XCEPT_RAISE (pt::http::exception::Exception, strerror(errno));
			}
#endif
		}

		errno = 0;	

		if (::connect(socket_, (struct sockaddr *)&sockaddress_, sockaddressSize_) == -1)
		{
			this->close();
			XCEPT_RAISE (pt::http::exception::Exception, strerror(errno));
		}
		connected_ = true;
	}	
}
	
	
void pt::http::ClientChannel::disconnect()  throw (pt::http::exception::Exception)
{
	if (socket_ != -1)
	{
		connected_ = false;
		::close(socket_);
		socket_ = -1;
	}		
}

void pt::http::ClientChannel::close()  throw (pt::http::exception::Exception)
{
	this->disconnect();
}	
	
	
ssize_t pt::http::ClientChannel::receive(char * buf, size_t len ) throw (pt::http::exception::Exception)
{
	errno = 0;
	ssize_t r = ::recv (socket_, buf, len, 0);
	if (r < 0)
	{
		if (errno == EWOULDBLOCK)
		{ 
			this->close();
			std::stringstream msg;
			msg << "Timeout during receive of " << len << " byte(s), error: " << strerror(errno);
			XCEPT_RAISE (pt::http::exception::Exception, msg.str());
		}
		else
		{
			this->close();
			XCEPT_RAISE (pt::http::exception::Exception, strerror(errno));
		}
	}
	return r;
}
	
void pt::http::ClientChannel::send(const char * buf, size_t len)  throw (pt::http::exception::Exception)
{
	size_t toWrite = len;
        ssize_t nBytes = 0;

	// std::cout << "Send length: " << len << std::endl;

        while (toWrite > 0) 
	{
		errno = 0;
		nBytes = ::write(socket_,(char*)&buf[nBytes],toWrite);
		if (nBytes < 0)
		{
			this->close();
			XCEPT_RAISE (pt::http::exception::Exception, strerror(errno));
		}
		toWrite -= nBytes;
        }
}
