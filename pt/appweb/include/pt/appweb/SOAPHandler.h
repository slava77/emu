/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2007, CERN.                                        *
 * All rights reserved.                                                  *
 * Authors: R. Moser                                                     *
 *                                                                       *
 * For the licensing terms see LICENSE.                                  *
 * For the list of contributors see CREDITS.                             *
 *************************************************************************/

#ifndef _pt_appweb_SOAPHandler_h
#define _pt_appweb_SOAPHandler_h

#include "appweb/appweb.h"
#include "pt/appweb/PeerTransportReceiver.h"

#include "log4cplus/logger.h"

using namespace log4cplus;

namespace pt
{
 
namespace appweb
{

class PeerTransportReceiver;

class SOAPHandler : public MaHandler
{
	public:
	SOAPHandler(char *extensions, pt::appweb::PeerTransportReceiver *ptr, Logger & logger);
	~SOAPHandler();
	MaHandler *cloneHandler();
	int setup(MaRequest *rq);
	int matchRequest(MaRequest *rq, char *uri, int urilen);
	void postData(MaRequest *rq, char *buf, int buflen);
	int run(MaRequest *rq);

	private:
	Logger logger_;
	pt::appweb::PeerTransportReceiver *ptr_;
	std::stringstream ios_;
};

class SOAPHandlerService : public MaHandlerService
{
	public:
	SOAPHandlerService(pt::appweb::PeerTransportReceiver *ptr, Logger & logger);
	~SOAPHandlerService();
	MaHandler *newHandler(MaServer *server, MaHost *host, char *ext);
	int setup();

	private:
	pt::appweb::PeerTransportReceiver *ptr_;
	Logger logger_;
};

class SOAPHandlerModule : public MaModule
{
	public:
	SOAPHandlerModule(pt::appweb::PeerTransportReceiver *ptr, Logger & logger);
	~SOAPHandlerModule();

	private:
	SOAPHandlerService *service;
};

}

}

#endif

