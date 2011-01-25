// $Id: Channel.h,v 1.3 2011/01/25 18:32:18 banicz Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2009, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _pt_http_Channel_h_
#define _pt_http_Channel_h_

#include <netinet/in.h>

#include "pt/Address.h"
#include "pt/http/exception/Exception.h"

#include "toolbox/BSem.h"

namespace pt
{
namespace http
{

class Channel 
{
	public:
	
	Channel(pt::Address::Reference address) throw (pt::http::exception::Exception);
		
	virtual ~Channel();
	
	//! connect channel according configuration
	virtual void connect() throw (pt::http::exception::Exception) = 0;
	
	//! disconnect but keep channel alive
	virtual void disconnect() throw (pt::http::exception::Exception) = 0;
	
	//! receive len characters into buf
	virtual ssize_t receive(char * buf, size_t len ) throw (pt::http::exception::Exception) = 0;
	
	//! send buffer of given lenght
	virtual void send(const char * buf, size_t len) throw (pt::http::exception::Exception) = 0;
	
	//! Close a connection definitely
	virtual void close() throw (pt::http::exception::Exception) = 0;
		
	//! Check if the connection is up
	virtual bool isConnected() throw (pt::http::exception::Exception) = 0;
	
	//! Get exclusive access to the Channel
	//
	void lock();
	
	//! Release exclusive access to the Channel
	//
	void unlock();
		
	protected:
	
	struct sockaddr_in sockaddress_;
	socklen_t sockaddressSize_;
	int socket_;
	toolbox::BSem mutex_;
	
	
};


class ClientChannel: public Channel
{
	public:
	
	ClientChannel(pt::Address::Reference address, unsigned long httpResponseTimeoutSec) throw (pt::http::exception::Exception);
	
	//! connect channel according configuration
	void connect() throw (pt::http::exception::Exception);
	
	//! disconnect
	void disconnect() throw (pt::http::exception::Exception);
	
	//! receive len characters into buf
	ssize_t receive(char * buf ,size_t len ) throw (pt::http::exception::Exception);
	
	//! send buffer of given lenght
	void send(const char * buf, size_t len) throw (pt::http::exception::Exception);
	
	//! Close a connection definitely
	void close() throw (pt::http::exception::Exception);
	
	//! Check if the connection is up
	bool isConnected() throw (pt::http::exception::Exception);
	
	private:
	
		bool connected_;
                unsigned long httpResponseTimeoutSec_;
};
}


/*
class ServerChannel: public Channel
{
	public:
	
	ServerChannel(http::Address* address) throw (pt::http::exception::Exception);
	
	//! reconnect a dropped incoming connection after disconnect
	void connect() throw (pt::http::exception::Exception);
	
	//! disconnect but keep channel alive
	void disconnect() throw (pt::http::exception::Exception);
	
	//! receive len characters into buf
	int receive(char * buf ,int len ) throw (pt::http::exception::Exception);
	
	//! send buffer of given lenght
	void send(const char * buf, int len) throw (pt::http::exception::Exception);
	
	//! Close a connection definitely
	void close() throw (pt::http::exception::Exception);
	
	//! Check if the connection is up
	bool isConnected() throw (pt::http::exception::Exception);
};
*/
}

#endif
