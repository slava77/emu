// $Id: ReceiverLoop.h,v 1.2 2011/01/25 17:36:47 banicz Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2009, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _pt_http_ReceiverLoop_h_
#define _pt_http_ReceiverLoop_h_

#include <map>
#include <vector>
#include <sys/select.h>

#include "pt/SOAPListener.h"
#include "xgi/Listener.h"
#include "pt/Address.h"
#include "pt/HTAccessSecurityPolicy.h"
#include "pt/http/Channel.h"
#include "pt/http/exception/Exception.h"
#include "toolbox/lang/Class.h"
#include "toolbox/task/WorkLoop.h"
#include "toolbox/task/Action.h"
#include "xgi/Utils.h"
#include "xgi/Method.h"
#include "log4cplus/logger.h"
#include "xdata/InfoSpace.h"
#include "xdata/UnsignedInteger32.h"
#include "xdaq/Object.h"

using namespace log4cplus;

namespace pt
{
namespace http
{

class ReceiverLoop: public xdaq::Object, public toolbox::lang::Class, http::Channel
{
	enum { MaxNoChannels = 1024 };

	public:
	
	ReceiverLoop(xdaq::Application* owner, pt::Address::Reference address, Logger & logger, xdata::InfoSpace * is) 
		throw (pt::http::exception::Exception);
	
	~ReceiverLoop();
	
	// override channel receive
	ssize_t receive(char * buf ,size_t len ) throw (pt::http::exception::Exception);
	
	void send(const char * buf, size_t len) throw (pt::http::exception::Exception);
	
	void disconnect() throw (pt::http::exception::Exception);
	
	void connect() throw (pt::http::exception::Exception); 
	
	void close() throw (pt::http::exception::Exception);
	
	bool process(toolbox::task::WorkLoop * wl);

	pt::Address::Reference getAddress();
	
	void addServiceListener (pt::Listener* listener);
	void removeServiceListener (pt::Listener* listener);
	void removeAllServiceListeners();
	
	void activate();
		
	private:
	
	int accept( std::string &ip, std::string & host) throw (pt::http::exception::Exception);
	
	bool isConnected() throw (pt::http::exception::Exception);
	bool isActive() throw (pt::http::exception::Exception);
	
	//! process incoming HTTP request
	//
	void onRequest(const std::string &ip, const std::string & host) throw (pt::http::exception::Exception);
	
	void reply (char* buffer, size_t length) throw (pt::http::exception::Exception);
	
	//! Auth
	bool authenticateUser(xgi::Input * in, xgi::Output * out, pt::SecurityPolicy * policy ) throw (xgi::exception::Exception);
	bool verifyAccess(xgi::Input * in, xgi::Output * out , pt::SecurityPolicy * policy) throw (xgi::exception::Exception);
	
	//! Return true if a browser is supported, false otherwise. \param name is the value of the HTTP "user-agent" field
	bool isBrowserSupported(const std::string& name);
	
	std::vector<int> sockets_;
	std::map<int, std::string, std::less<int> > clientIP_;
	std::map<int, std::string, std::less<int> > clientHost_;
	
	pt::Address::Reference  address_;
	fd_set fdset_;
	fd_set allset_;
	int maxfd_;
	int current_;
	int listenfd_;
	int accepted_;
	int nochannels_;
	int nready_;
	
	pt::SOAPListener* listener_;
	xgi::Listener* cgiListener_;
	toolbox::task::ActionSignature * process_;
	std::string httpRootDir_;
	Logger logger_;
	pt::HTAccessSecurityPolicy *  policy_;
	xdata::InfoSpace * is_;
	
	xdata::UnsignedInteger32T requestCounter_;
	std::map<std::string, std::string> aliases_;
};

}
}
#endif
