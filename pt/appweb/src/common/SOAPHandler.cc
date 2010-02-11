/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2007, CERN.                                        *
 * All rights reserved.                                                  *
 * Authors: R. Moser                                                     *
 *                                                                       *
 * For the licensing terms see LICENSE.                                  *
 * For the list of contributors see CREDITS.                             *
 *************************************************************************/

#include "pt/appweb/SOAPHandler.h"
#include "xgi/Input.h"

#include "xoap/domutils.h"
#include "xoap/MessageReference.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPBody.h"
#include "xoap/SOAPFault.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPPart.h"

#include "xcept/tools.h"

#include <string>

pt::appweb::SOAPHandler::SOAPHandler(char *extensions, pt::appweb::PeerTransportReceiver *ptr, Logger & logger) :
	MaHandler("SOAPHandler", extensions,
	MPR_HANDLER_POST | MPR_HANDLER_TERMINAL | MPR_HANDLER_MAP_VIRTUAL |
  MPR_HTTP_CREATE_ENV | MPR_HANDLER_NEED_ENV),
	logger_(logger)
{
	ptr_=ptr;
}

pt::appweb::SOAPHandler::~SOAPHandler()
{
}

MaHandler *pt::appweb::SOAPHandler::cloneHandler()
{
	return new SOAPHandler(extensions, ptr_, logger_);
}

int pt::appweb::SOAPHandler::setup(MaRequest *rq)
{
	return 1;
}

int pt::appweb::SOAPHandler::matchRequest(MaRequest *rq, char *uri, int urilen)
{
	char *line;

	line = rq->getVar(MA_HEADERS_OBJ, "CONTENT_TYPE", 0);
	if(line!=NULL)
	{
		std::string contenttype=line;
		size_t size=contenttype.find("application/soap+xml");
		if(size==std::string::npos)
			size=contenttype.find("text/xml");
		// single part message
		if(size==0)
			return 1;
		// multipart message
		if(size!=std::string::npos && contenttype.find("multipart/related")==0)
			return 1;
	}

	line = rq->getVar(MA_HEADERS_OBJ, "SOAPACTION", 0);
	if(line!=NULL)
		return 1;

	return 0;
}

void pt::appweb::SOAPHandler::postData(MaRequest *rq, char *buf, int buflen)
{
	ios_.write(buf, buflen);
	if(buflen<0 && rq->getRemainingContent()>0)
	{
		LOG4CPLUS_DEBUG (logger_, "Incoming message was incomplete");
		rq->requestError(400, "Incomplete post data");
		return;
	}

	if(rq->getRemainingContent()<=0)
	{
		run(rq);
	}
}

int pt::appweb::SOAPHandler::run(MaRequest *rq)
{
	int flags;

	flags = rq->getFlags();
	if(flags & MPR_HTTP_POST_REQUEST && rq->getRemainingContent() > 0)
		return MPR_HTTP_HANDLER_FINISHED_PROCESSING;

	pt::SOAPListener *listener=0;
	try
	{
		listener=dynamic_cast<pt::SOAPListener*>(ptr_->getListener("soap"));
	}
	catch(pt::exception::Exception &e)
	{
	}
	if (listener == 0)
	{
		LOG4CPLUS_FATAL(logger_, "No listener for HTTP/SOAP available, message discarded");
		rq->finishRequest(MPR_HTTP_COMMS_ERROR, MPR_HTTP_CLOSE); 
		return MPR_CMD_EOF;
	}

	xgi::Output out;
	xoap::MessageReference reply;
	try
	{
		std::string s;
		s=ios_.str();

		if (s.size() == 0)
		{
			reply = xoap::createMessage();
			xoap::SOAPBody b = reply->getSOAPPart().getEnvelope().getBody();
			xoap::SOAPFault f = b.addFault();
			f.setFaultCode ("Server");
			f.setFaultString ("Empty SOAP message");
		}
		else
		{
			xoap::MessageReference msg = xoap::createMessage((char*)s.c_str(), s.size());

			// Copy all http header information and pass it together with the SOAP message
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
							msg->getMimeHeaders()->setHeader(el->name, el->string);
						}
						el=mprGetNextProperty(v, el, MPR_ENUM_FUNCTIONS|MPR_ENUM_DATA);
					}
				}
			}

			LOG4CPLUS_DEBUG(logger_, "SOAPEnvelope: " << ios_.str());
			reply = listener->processIncomingMessage( msg );
		}
	}
	catch (xoap::exception::Exception & soe)
	{
		reply = xoap::createMessage();
		xoap::SOAPBody b = reply->getSOAPPart().getEnvelope().getBody();
		xoap::SOAPFault f = b.addFault();
		f.setFaultCode ("Server");
		f.setFaultString (xcept::stdformat_exception_history(soe));
	}
	catch (pt::exception::Exception & pte)
	{
		reply = xoap::createMessage();
		xoap::SOAPBody b = reply->getSOAPPart().getEnvelope().getBody();
		xoap::SOAPFault f = b.addFault();
		f.setFaultCode ("Server");
		f.setFaultString (xcept::stdformat_exception_history(pte));
	}
	catch (...)
	{
		reply = xoap::createMessage();
		xoap::SOAPBody b = reply->getSOAPPart().getEnvelope().getBody();
		xoap::SOAPFault f = b.addFault();
		f.setFaultCode ("Server");
		f.setFaultString ("Caught unknown exception while processing incoming SOAP message");
	}

	if (reply->countAttachments() > 0)
	{
		std::string contentType = "multipart/related; type=\"application/soap+xml\"; boundary=\"";
		contentType += reply->getMimeBoundary();
		contentType += "\"";
		out.getHTTPResponseHeader().addHeader("Content-Type", contentType );
		out.getHTTPResponseHeader().addHeader("Content-Description", "SOAP Message with attachments");
	} 
	else
	{
		out.getHTTPResponseHeader().addHeader("Content-Type", "application/soap+xml");
		out.getHTTPResponseHeader().addHeader("Content-Description", "SOAP Message");
	}
  
	reply->writeTo (out); // write SOAP contents to a buffer
    
	// Add the content length to the header
	size_t replyLen = out.tellp();
	if (replyLen != 0) 
	{
		std::stringstream lenStr;
		lenStr <<  replyLen;
		out.getHTTPResponseHeader().addHeader("Content-Length", lenStr.str());
	}

  return ptr_->flush(rq, &out);
}



pt::appweb::SOAPHandlerService::SOAPHandlerService(pt::appweb::PeerTransportReceiver *ptr, Logger & logger) :
	MaHandlerService("SOAPHandler"), logger_(logger)
{
	ptr_=ptr;
}

pt::appweb::SOAPHandlerService::~SOAPHandlerService()
{
}

MaHandler* pt::appweb::SOAPHandlerService::newHandler(MaServer *server, MaHost *host, char *extensions)
{
	return new SOAPHandler(extensions, ptr_, logger_);
}

int pt::appweb::SOAPHandlerService::setup()
{
	return 0;
}



pt::appweb::SOAPHandlerModule::SOAPHandlerModule(pt::appweb::PeerTransportReceiver *ptr, Logger & logger) :
	MaModule("SOAPHandler", 0)
{
	service = new SOAPHandlerService(ptr, logger);
}

pt::appweb::SOAPHandlerModule::~SOAPHandlerModule()
{
	delete service;
}

