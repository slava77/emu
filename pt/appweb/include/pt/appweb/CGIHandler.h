/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2007, CERN.                                        *
 * All rights reserved.                                                  *
 * Authors: R. Moser                                                     *
 *                                                                       *
 * For the licensing terms see LICENSE.                                  *
 * For the list of contributors see CREDITS.                             *
 *************************************************************************/

#ifndef _pt_appweb_CGIHandler_h
#define _pt_appweb_CGIHandler_h

#include "appweb/appweb.h"
#include "pt/appweb/PeerTransportReceiver.h"

#include "xgi/Input.h"
#include "log4cplus/logger.h"

using namespace log4cplus;

namespace pt
{
 
namespace appweb
{

class PeerTransportReceiver;

class CGIHandler : public MaHandler
{
	public:
	CGIHandler(char *extensions, pt::appweb::PeerTransportReceiver *ptr, Logger & logger);
	~CGIHandler();
	MaHandler *cloneHandler();
	int setup(MaRequest *rq);
	int matchRequest(MaRequest *rq, char *uri, int urilen);
	void postData(MaRequest *rq, char *buf, int buflen);
	int run(MaRequest *rq);

	private:
	Logger logger_;
	pt::appweb::PeerTransportReceiver *ptr_;
	std::string content_;
	xgi::Input in_;
};

class CGIHandlerService : public MaHandlerService
{
	public:
	CGIHandlerService(pt::appweb::PeerTransportReceiver *ptr, Logger & logger);
	~CGIHandlerService();
	MaHandler *newHandler(MaServer *server, MaHost *host, char *ext);
	int setup();

	private:
	pt::appweb::PeerTransportReceiver *ptr_;
	Logger logger_;
};

class CGIHandlerModule : public MaModule
{
	public:
	CGIHandlerModule(pt::appweb::PeerTransportReceiver *ptr, Logger & logger);
	~CGIHandlerModule();

	private:
	CGIHandlerService *service;
};

}

}

#endif
