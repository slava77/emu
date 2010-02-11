/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2007, CERN.                                        *
 * All rights reserved.                                                  *
 * Authors: R. Moser                                                     *
 *                                                                       *
 * For the licensing terms see LICENSE.                                  *
 * For the list of contributors see CREDITS.                             *
 *************************************************************************/

#include "pt/appweb/CGIHandler.h"
#include "xgi/Input.h"

#include "xgi/Listener.h"
#include "xgi/Input.h"
#include "xgi/Output.h"
#include "xgi/Utils.h"

#include "xdata/String.h"

#include "xcept/tools.h"

#include <string>

pt::appweb::CGIHandler::CGIHandler(char *extensions, pt::appweb::PeerTransportReceiver *ptr, Logger & logger) :
	MaHandler("CGIHandler", extensions,
	MPR_HANDLER_GET | MPR_HANDLER_POST | MPR_HANDLER_TERMINAL | MPR_HANDLER_MAP_VIRTUAL |
	MPR_HTTP_CREATE_ENV | MPR_HANDLER_NEED_ENV),
	logger_(logger),
	in_(NULL, 0)
{
	ptr_=ptr;
	content_="";
}

pt::appweb::CGIHandler::~CGIHandler()
{
}

MaHandler *pt::appweb::CGIHandler::cloneHandler()
{
	return new CGIHandler(extensions, ptr_, logger_);
}

int pt::appweb::CGIHandler::setup(MaRequest *rq)
{
	return 1;
}

int pt::appweb::CGIHandler::matchRequest(MaRequest *rq, char *uri, int urilen)
{
	std::string path=uri;
	if(path=="/" || path.find ("/urn:xdaq-application:")==0)
		return 1;

	return 0;
}

void pt::appweb::CGIHandler::postData(MaRequest *rq, char *buf, int buflen)
{
	if(buflen<0 && rq->getRemainingContent()>0)
	{
		rq->requestError(400, "Incomplete post data");
		return;
	}

	in_.write(buf, buflen);
	if(rq->getRemainingContent()<=0)
		run(rq);
}

int pt::appweb::CGIHandler::run(MaRequest *rq)
{
	int flags;

	flags = rq->getFlags();
	if(flags & MPR_HTTP_POST_REQUEST && rq->getRemainingContent() > 0)
		return MPR_HTTP_HANDLER_FINISHED_PROCESSING;

	xgi::Output out;
	out.clear();

	xgi::Listener *listener=0;
	try
	{
		listener=dynamic_cast<xgi::Listener*>(ptr_->getListener("cgi"));
	}
	catch(pt::exception::Exception &e)
	{
	}
	if (listener == 0)
	{
		LOG4CPLUS_FATAL(logger_, "No listener for CGI available");
		out.getHTTPResponseHeader().getStatusCode(501);
		out.getHTTPResponseHeader().getReasonPhrase(xgi::Utils::getResponsePhrase (501));
		return ptr_->flush(rq, &out);
	}

	std::string uri=rq->getUri();
	if(uri=="/")
	{
		rq->redirect(301, "/urn:xdaq-application:service=hyperdaq");
		rq->flushOutput(MPR_HTTP_BACKGROUND_FLUSH, MPR_HTTP_FINISH_REQUEST);
		return MPR_HTTP_HANDLER_FINISHED_PROCESSING;
	}

	size_t start_script = uri.find ("/urn:xdaq-application:");
	size_t end_script = std::string::npos;
	std::string script = "";
	std::string path_info = "";
	if (start_script == 0)
	{
		// find end of urn, i.e. a '/'
		end_script = uri.find ("/", start_script+1);
		if (end_script != std::string::npos)
		{
			// without the leading '/'
			script = uri.substr (start_script+1, end_script - (start_script+1));
			path_info = uri.substr(end_script+1);
		}
		else
		{
			script = uri.substr(start_script+1);
		}
	}

	MprVar* var=rq->getVariables();
	for(int cont=0;cont<MA_HTTP_OBJ_MAX;++cont)
	{
		struct MprVar *v=&var[cont];
		struct MprVar *el;
		if (v->type == MPR_TYPE_OBJECT && v->properties != NULL)
		{
			el=mprGetFirstProperty(v, MPR_ENUM_FUNCTIONS|MPR_ENUM_DATA);
			while(el!=0)
			{
				if (el->type == MPR_TYPE_STRING)
				{
					LOG4CPLUS_DEBUG(logger_, el->name << " : " << el->string);
					if(strcmp(el->name, "SCRIPT_NAME")==0)
						in_.putenv(el->name, script);
					else if(strcmp(el->name, "PATH_INFO")==0)
						in_.putenv(el->name, path_info);
				else
						in_.putenv(el->name, el->string);
				}
				el=mprGetNextProperty(v, el, MPR_ENUM_FUNCTIONS|MPR_ENUM_DATA);
			}
		} 
	}
	in_.putenv("PATH_TRANSLATED", rq->getOriginalUri());

	try
	{
		// Force page expiration, may be overridden by user callback
		out.getHTTPResponseHeader().addHeader ("Expires", "0");

		in_.cin().seekg(0);
		listener->processIncomingMessage (&in_, &out);
	}
	catch (pt::exception::Exception& pte)
	{
		(void) out.getHTTPResponseHeader().getStatusCode(400);
		(void) out.getHTTPResponseHeader().getReasonPhrase(xgi::Utils::getResponsePhrase(400));
		(void) out.getHTTPResponseHeader().addHeader("Content-Type", "text/html");
		out << xgi::Utils::getFailurePage("HTTP Request Error", xcept::htmlformat_exception_history(pte));
	}
	catch (...)
	{
		(void) out.getHTTPResponseHeader().getStatusCode(400);
		(void) out.getHTTPResponseHeader().getReasonPhrase(xgi::Utils::getResponsePhrase(400));
		(void) out.getHTTPResponseHeader().addHeader("Content-Type", "text/html");
		out << xgi::Utils::getFailurePage("HTTP Request Error", "Caught unknown exception while dispatching CGI request");
	}

	return ptr_->flush(rq, &out);
}



pt::appweb::CGIHandlerService::CGIHandlerService(pt::appweb::PeerTransportReceiver *ptr, Logger & logger) :
	MaHandlerService("CGIHandler"),
	logger_(logger)
{
	ptr_=ptr;
}

pt::appweb::CGIHandlerService::~CGIHandlerService()
{
}

MaHandler* pt::appweb::CGIHandlerService::newHandler(MaServer *server, MaHost *host, char *extensions)
{
	return new CGIHandler(extensions, ptr_, logger_);
}

int pt::appweb::CGIHandlerService::setup()
{
	return 0;
}



pt::appweb::CGIHandlerModule::CGIHandlerModule(pt::appweb::PeerTransportReceiver *ptr, Logger & logger) :
	MaModule("CGIHandler", 0)
{
	service = new CGIHandlerService(ptr, logger);
}

pt::appweb::CGIHandlerModule::~CGIHandlerModule()
{
	delete service;
}

