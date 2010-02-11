/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2007, CERN.                                        *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber, L. Orsini, R. Moser                             *
 *                                                                       *
 * For the licensing terms see LICENSE.                                  *
 * For the list of contributors see CREDITS.                             *
 *************************************************************************/

#include "pt/appweb/PeerTransportReceiver.h"
#include "pt/appweb/AccessHandler.h"
#include "pt/appweb/SOAPHandler.h"
#include "pt/appweb/CGIHandler.h"
#include "pt/appweb/exception/Exception.h"

#include "pt/SecurityPolicyFactory.h"
#include "xdata/String.h"
#include "xcept/tools.h"
#include "xgi/Listener.h"
#include "xgi/Output.h"
#include "toolbox/Runtime.h"

#include <iostream>
#include <vector>

#include "rom.cc"


pt::appweb::PeerTransportReceiver::PeerTransportReceiver(Logger & logger, xdata::InfoSpace* is)
	: logger_(logger), is_(is)
{
	runtime=new Mpr("ptappweb");

	//runtime->addListener(new MprLogToFile());
	//runtime->setLogSpec("stdout:4");

	runtime->start(MPR_SERVICE_THREAD);
	runtime->poolService->setMaxPoolThreads(1);

	appweb = new MaHttp();

	is_->addListener(this, "urn:xdaq-event:setDefaultValues");
}

pt::appweb::PeerTransportReceiver::~PeerTransportReceiver()
{
	if(appweb)	
		appweb->stop();
	if (server)
		delete server;
	if (appweb)
		delete appweb;
	if (romFileSystem_)
		delete romFileSystem_;
}

pt::TransportType pt::appweb::PeerTransportReceiver::getType()
{
	return pt::Receiver;
}

pt::Address::Reference pt::appweb::PeerTransportReceiver::createAddress( const std::string& url, const std::string & service )
throw (pt::exception::InvalidAddress)
{
	// newly created address pointer is ref counted and goes out of scope when not used anymore
	// function throws already correct exception
	return pt::Address::Reference(new pt::appweb::Address(url,service));
}

pt::Address::Reference 
pt::appweb::PeerTransportReceiver::createAddress( std::map<std::string, std::string, std::less<std::string> >& address )
throw (pt::exception::InvalidAddress)
{
	std::string protocol = address["protocol"];
	
	if (protocol == "http")
	{
		std::string url = protocol;
		
		XCEPT_ASSERT (address["hostname"] != "",pt::exception::InvalidAddress , "Cannot create address, hostname not specified");
		XCEPT_ASSERT (address["port"] != "",pt::exception::InvalidAddress , "Cannot create address, port number not specified");
		
		url += "://";
		url += address["hostname"];
		url += ":";
		url += address["port"];
		
		std::string service = address["service"];
		if (service != "")
		{
			if (!this->isServiceSupported(service))
			{
				std::string msg = "Cannot create address, specified service for protocol ";
				msg += protocol;
				msg += " not supported: ";
				msg += service;
				XCEPT_RAISE(pt::exception::InvalidAddress, msg);
			}
			
			//url += "/";
			//url += service;
		}
		else
		{
			std::string msg = "Cannot create address, service for protocol ";
			msg += protocol;
			msg += " not specified";
			XCEPT_RAISE(pt::exception::InvalidAddress, msg);
		}
		
		// throws already pt::exception::InvalidAddress, will be chained up
		return this->createAddress(url,service);
	}
	else 
	{
		std::string msg = "Cannot create address, protocol not supported: ";
		msg += protocol;
		XCEPT_RAISE(pt::exception::InvalidAddress, msg);
	}	
}

std::string pt::appweb::PeerTransportReceiver::getProtocol()
{
	return "http";
}

std::vector<std::string> pt::appweb::PeerTransportReceiver::getSupportedServices()
{
	std::vector<std::string> s;
	s.push_back("soap");
	s.push_back("cgi");
	return s;
}

bool pt::appweb::PeerTransportReceiver::isServiceSupported(const std::string& service )
{
	if (service == "soap") return true;
	if (service == "cgi") return true;
	return false;
}

void pt::appweb::PeerTransportReceiver::actionPerformed( xdata::Event& event)
{
  if (event.type() == "urn:xdaq-event:setDefaultValues")
  {
		try
		{
			std::vector<std::string> paths = toolbox::getRuntime()->expandPathName
      	(
					dynamic_cast<xdata::String*>(is_->find("documentRoot"))->toString()
				);

			if (paths.size() == 1)
			{
				documentRoot_ = paths[0];
			}
			else
			{
				std::stringstream msg;
				msg << "documentRoot path '" << dynamic_cast<xdata::String*>(is_->find("documentRoot"))->toString() << "' is ambiguous";
				XCEPT_RAISE (pt::appweb::exception::Exception, msg.str());
			}
		}
		catch (toolbox::exception::Exception& e)
		{
			std::stringstream msg;
			msg << "Failed to expand documentRoot path '" << dynamic_cast<xdata::String*>(is_->find("documentRoot"))->toString() << "'";
			documentRoot_ = ".";
			LOG4CPLUS_FATAL (logger_, msg.str());
		}
		catch (xdata::exception::Exception& e)
		{
			// no documentRoot declaration found
			documentRoot_ = ".";
			LOG4CPLUS_INFO (logger_, "documentRoot not found in configuration");
		}
		LOG4CPLUS_INFO(logger_,"documentRoot set to '" << documentRoot_ << "'");

		server = new MaServer(appweb, "default", (char*)documentRoot_.c_str());

		romFileSystem_ = new MaRomFileSystem(defaultRomFiles);
		server->setFileSystem(romFileSystem_);

		new MaCopyModule(0);
		new SOAPHandlerModule(this, logger_);
		new CGIHandlerModule(this, logger_);

		policy_ = 0;
		try
		{
			policy_ = dynamic_cast<pt::HTAccessSecurityPolicy*>(pt::getSecurityPolicyFactory()->getSecurityPolicy("urn:xdaq-security:htaccess"));
		}
		catch (pt::exception::Exception & e )
		{
			LOG4CPLUS_INFO(logger_,"No security policies on this server");
		}

		if(policy_ != 0)
			new AccessHandlerModule(this, logger_);

		try
		{			
			aliases_ = dynamic_cast<xdata::Vector< xdata::Bag<Alias> >*>(is_->find("aliases"));
			xdata::Vector< xdata::Bag<Alias> >::iterator ci;
			for (ci = aliases_->begin(); ci != aliases_->end(); ++ci)
			{
				std::vector<std::string> paths = toolbox::getRuntime()->expandPathName(
						(*ci).bag.path.toString()
					);
				if (paths.size() == 1)
				{
					(*ci).bag.path = paths[0];
				}
				else
				{
					std::stringstream msg;
					msg << "Alias path '" << (*ci).bag.path.toString() << "' is ambiguous";
					XCEPT_RAISE (pt::appweb::exception::Exception, msg.str());
				}

				LOG4CPLUS_INFO (logger_, "Setting alias from [" << (*ci).bag.name.toString() << "] to [" << (*ci).bag.path.toString() << "]");
			}
		}
		catch (xdata::exception::Exception& e)
		{
			// no alias declaration found
			aliases_ = NULL;
			LOG4CPLUS_INFO (logger_, "aliases not found in configuration");
		}

		try
		{
			threads_ = *dynamic_cast<xdata::UnsignedInteger32*>(is_->find("threads"));
		}
		catch (xdata::exception::Exception& e)
		{
			// no thread declaration found
			threads_ = 1;
			LOG4CPLUS_INFO (logger_, "threads not found in configuration");
		}

		LOG4CPLUS_INFO (logger_, "Number of threads: '" << threads_ << "'");
		runtime->poolService->setMaxPoolThreads(threads_);

		try
		{
			int maxbody = *dynamic_cast<xdata::UnsignedInteger32*>(is_->find("maxBody"));
			MaLimits *limits = appweb->getLimits();
			limits->maxBody = maxbody;
			LOG4CPLUS_INFO (logger_, "Maximum number of bytes in body: " << maxbody);
		}
		catch (xdata::exception::Exception& e)
		{
			// no maxbody declaration found
			LOG4CPLUS_INFO (logger_, "maxbody not found in configuration");
		}
	}
  else
  { 
    LOG4CPLUS_ERROR (logger_, "Received unsupported event type '" << event.type() << "'");
  }
}

void pt::appweb::PeerTransportReceiver::config( pt::Address::Reference address) throw (pt::exception::Exception)
{
	pt::appweb::Address& adr = dynamic_cast<pt::appweb::Address&>(*address);
	LOG4CPLUS_INFO(logger_, adr.toString());

	MaHost *host;
	host = server->newHost((char*)documentRoot_.c_str(), (char*)(adr.resolve()+":"+adr.getPort()).c_str());
	if (host == 0)
		return;

	// do not use chunked transfer as SOAPMessenger cannot handle it
	host->setChunking(false);

	if(policy_!=0)
	{
		if (server->loadModule("AccessHandler") == 0)
		{
			host->addHandler("AccessHandler", "");
		}
		else
		{
			LOG4CPLUS_INFO(logger_, "AccessHandler could not be loaded");
		}
	}

	if (server->loadModule("SOAPHandler") == 0)
	{
		host->addHandler("SOAPHandler", "");
	}
	else
	{
		LOG4CPLUS_INFO(logger_, "SOAPHandler could not be loaded");
	}
	if (server->loadModule("CGIHandler") == 0)
	{
		host->addHandler("CGIHandler", "");
	}
	else
	{
		LOG4CPLUS_INFO(logger_, "CGIHandler could not be loaded");
	}

	if (server->loadModule("copy") == 0)
	{
		host->addHandler("copyHandler", "");
	}
	else
	{
		LOG4CPLUS_INFO(logger_, "copyHandler could not be loaded");
	}

	if(aliases_ != NULL)
	{
		xdata::Vector< xdata::Bag<Alias> >::iterator ci;
		for (ci = aliases_->begin(); ci != aliases_->end(); ++ci)
		{
			// allow access to the specified path
			MaDir *dir = new MaDir(host);
			dir->setPath((char*)(*ci).bag.path.toString().c_str());
			host->insertDir(dir);

			// set alias
			MaAlias *ap = new MaAlias((char*)(*ci).bag.name.toString().c_str(), (char*)(*ci).bag.path.toString().c_str());
			host->insertAlias(ap);
		}
	}

	server->setFileSystem(new MprFileSystem()); 

	if (appweb->start() < 0)
		return;
}

// Overriding function to asynchrounously set the listener
//
void pt::appweb::PeerTransportReceiver::addServiceListener (pt::Listener* listener) throw (pt::exception::Exception)
{
	pt::PeerTransportReceiver::addServiceListener(listener);
	if (listener->getService() == "soap")
		listener_ = dynamic_cast<pt::SOAPListener *>(listener);
	if (listener->getService() == "cgi")
		cgiListener_ = dynamic_cast<xgi::Listener *>(listener);
}

void pt::appweb::PeerTransportReceiver::removeServiceListener (pt::Listener* listener ) throw (pt::exception::Exception)
{
	pt::PeerTransportReceiver::removeServiceListener(listener);
	if (listener->getService() == "soap")
		listener_ = 0;
	if (listener->getService() == "cgi")
		cgiListener_ = 0;
}

void pt::appweb::PeerTransportReceiver::removeAllServiceListeners()
{
	pt::PeerTransportReceiver::removeAllServiceListeners();
	listener_ = 0;
	cgiListener_ = 0;
}

pt::HTAccessSecurityPolicy* pt::appweb::PeerTransportReceiver::getAccessSecurityPolicy()
{
	return policy_;
}

int pt::appweb::PeerTransportReceiver::flush(MaRequest *rq, xgi::Output *out)
{
	cgicc::HTTPResponseHeader& header=out->getHTTPResponseHeader();

	const std::vector<std::string>& headers=header.getHeaders();
	std::vector<std::string>::const_iterator iter;

	int flags=rq->getFlags();
	MaDataStream *dynBuf;

	for(iter=headers.begin();iter!=headers.end();iter++)
	{
		rq->setHeader((char*)(*iter).c_str());
		LOG4CPLUS_DEBUG(logger_, "Header(response): " << *iter);
	}

	if(!(flags & MPR_HTTP_HEAD_REQUEST))
	{
		rq->setResponseCode(header.getStatusCode());
		dynBuf = rq->getDynBuf();
		rq->insertDataStream(dynBuf);

		std::string content=out->str();
		if (rq->write((char*)content.c_str(), content.size()) < 0)			
		{
			rq->finishRequest(MPR_HTTP_COMMS_ERROR, MPR_HTTP_CLOSE);
			return MPR_CMD_EOF;
		}
	}

	rq->flushOutput(MPR_HTTP_BACKGROUND_FLUSH, MPR_HTTP_FINISH_REQUEST);
	return MPR_HTTP_HANDLER_FINISHED_PROCESSING;
}

