/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2007, CERN.                                        *
 * All rights reserved.                                                  *
 * Authors: R. Moser                                                     *
 *                                                                       *
 * For the licensing terms see LICENSE.                                  *
 * For the list of contributors see CREDITS.                             *
 *************************************************************************/

#include "pt/appweb/AccessHandler.h"

#include "pt/HTAccessSecurityPolicy.h"
#include "xcept/tools.h"
#include "xoap/domutils.h"
#include "xercesc/util/Base64.hpp"
#include "xgi/Utils.h"
#include "xgi/Output.h"
#include "toolbox/net/Utils.h"

#include <string>

pt::appweb::AccessHandler::AccessHandler(pt::appweb::PeerTransportReceiver *ptr, Logger & logger) :
	MaHandler("AccessHandler", "",
	MPR_HANDLER_ALWAYS | MPR_HTTP_CREATE_ENV | MPR_HANDLER_NEED_ENV),
	logger_(logger)
{
	ptr_=ptr;
}

pt::appweb::AccessHandler::~AccessHandler()
{
}

MaHandler *pt::appweb::AccessHandler::cloneHandler()
{
	return new AccessHandler(ptr_, logger_);
}

int pt::appweb::AccessHandler::matchRequest(MaRequest *rq, char *uri, int urilen)
{
	return 1;
}

int pt::appweb::AccessHandler::run(MaRequest *rq)
{
	pt::HTAccessSecurityPolicy *policy;
	policy = ptr_->getAccessSecurityPolicy();
	if(policy == 0)
	{
		LOG4CPLUS_DEBUG (logger_, "No Policy specified for incoming request");
		return 0;
	}

	std::string method;
	method = rq->getVar(MA_REQUEST_OBJ, "REQUEST_METHOD", "");

	if ( policy->isAccessLimited(method) )
	{
		std::string remotehost;
		std::string remoteaddr;

		remotehost = rq->getVar(MA_REQUEST_OBJ, "REMOTE_HOST", "");
		remoteaddr = rq->getVar(MA_REQUEST_OBJ, "REMOTE_ADDR", "");

		if(!policy->checkAccess(remotehost, remoteaddr))
		{
			xgi::Output out;
			out.clear();
			(void) out.getHTTPResponseHeader().getStatusCode(403);
			(void) out.getHTTPResponseHeader().getReasonPhrase("Forbidden");
			(void) out.getHTTPResponseHeader().addHeader("Content-Type", "text/html");
			out << xgi::Utils::getSimpleFailurePage("403 Forbidden", "Access denied invalid host ("+remotehost+"). Contact your XDAQ administrator.");
			return ptr_->flush(rq, &out);
		}
	}

	if ( policy->isAuthLimited(method) )
	{
		std::string remoteuser="";

		char *authdetails = rq->getAuthDetails();
		if(authdetails != NULL)
		{
			// std::cout << "Authdetails: " << authdetails << std::endl;
			unsigned int decodedLength;
			XMLCh* decoded = Base64::decode(xoap::XStr(authdetails), &decodedLength);
			remoteuser = xoap::XMLCh2String(decoded);
		}

		rq->setVar(MA_REQUEST_OBJ, "REMOTE_USER", (char*)remoteuser.c_str());
		// std::cout << "Remote-User: " << remoteuser << std::endl;

		if(remoteuser.empty() || !policy->checkAuth(remoteuser))
		{
			xgi::Output out;
			out.clear();
			(void) out.getHTTPResponseHeader().getStatusCode(401);
			(void) out.getHTTPResponseHeader().getReasonPhrase("Unauthorized");
			(void) out.getHTTPResponseHeader().addHeader("Content-Type", "text/html");
			(void) out.getHTTPResponseHeader().addHeader("WWW-Authenticate", "Basic realm=\"cgicc\"");

			std::ostringstream oss;
			oss << "This server could not verify that you are "
				<< "authorized to access the document requested. Either you "
				<< "supplied the wrong credentials (e.g., bad password), or "
				<< "your browser doesn't understand how to supply the "
				<< "credentials required.";

			out << xgi::Utils::getSimpleFailurePage("401 Authorization Required", oss.str());
			return ptr_->flush(rq, &out);
		}
	}

	return 0;
}



pt::appweb::AccessHandlerService::AccessHandlerService(pt::appweb::PeerTransportReceiver *ptr, Logger & logger) :
	MaHandlerService("AccessHandler"), logger_(logger)
{
	ptr_=ptr;
}

pt::appweb::AccessHandlerService::~AccessHandlerService()
{
}

MaHandler* pt::appweb::AccessHandlerService::newHandler(MaServer *server, MaHost *host, char *ext)
{
	return new AccessHandler(ptr_, logger_);
}

int pt::appweb::AccessHandlerService::setup()
{
	return 0;
}



pt::appweb::AccessHandlerModule::AccessHandlerModule(pt::appweb::PeerTransportReceiver *ptr, Logger & logger) :
	MaModule("AccessHandler", 0)
{
	service = new AccessHandlerService(ptr, logger);
}

pt::appweb::AccessHandlerModule::~AccessHandlerModule()
{
	delete service;
}

