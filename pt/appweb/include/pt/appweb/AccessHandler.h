/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2007, CERN.                                        *
 * All rights reserved.                                                  *
 * Authors: R. Moser                                                     *
 *                                                                       *
 * For the licensing terms see LICENSE.                                  *
 * For the list of contributors see CREDITS.                             *
 *************************************************************************/

#ifndef _pt_appweb_AccessHandler_h
#define _pt_appweb_AccessHandler_h

#include "appweb/appweb.h"
#include "pt/appweb/PeerTransportReceiver.h"

#include "log4cplus/logger.h"

using namespace log4cplus;

namespace pt
{
 
namespace appweb
{

class PeerTransportReceiver;

class AccessHandler : public MaHandler
{
	public:
	AccessHandler(pt::appweb::PeerTransportReceiver *ptr, Logger & logger);
	~AccessHandler();
	MaHandler *cloneHandler();
//	int setup(MaRequest *rq);
	int matchRequest(MaRequest *rq, char *uri, int urilen);
//	void postData(MaRequest *rq, char *buf, int buflen);
	int run(MaRequest *rq);

	private:
	Logger logger_;
	pt::appweb::PeerTransportReceiver *ptr_;

	protected:
	int flush401(MaRequest *rq);
};

class AccessHandlerService : public MaHandlerService
{
	public:
	AccessHandlerService(pt::appweb::PeerTransportReceiver *ptr, Logger & logger);
	~AccessHandlerService();
	MaHandler *newHandler(MaServer *server, MaHost *host, char *ext);
	int setup();

	private:
	pt::appweb::PeerTransportReceiver *ptr_;
	Logger logger_;
};

class AccessHandlerModule : public MaModule
{
	public:
	AccessHandlerModule(appweb::PeerTransportReceiver *ptr, Logger & logger);
	~AccessHandlerModule();

	private:
	AccessHandlerService *service;
};

}

}

#endif
