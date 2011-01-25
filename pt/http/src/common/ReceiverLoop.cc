// $Id: ReceiverLoop.cc,v 1.2 2011/01/25 17:36:47 banicz Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2009, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#include "pt/http/ReceiverLoop.h"
#include "pt/http/Address.h"
#include "pt/http/Utils.h"
#include "pt/http/Alias.h"
#include "pt/http/exception/Exception.h"
#include "pt/http/exception/CannotConnect.h"
#include "pt/exception/Exception.h"

#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <streambuf>
#include <sstream>
#include <fstream>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iomanip>

#include "xcept/tools.h"

#include "xoap/domutils.h"
#include "xoap/MessageReference.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPBody.h"
#include "xoap/SOAPFault.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPPart.h"
#include "xdata/String.h"

#include "toolbox/task/WorkLoopFactory.h"
#include "toolbox/task/WorkLoop.h"
#include "toolbox/string.h"
#include "toolbox/TimeVal.h"
#include "toolbox/Runtime.h"
#include "toolbox/net/Utils.h"
#include "toolbox/stl.h"
#include "cgicc/HTTPStatusHeader.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTTPResponseHeader.h"
#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"
#include "cgicc/HTTPRedirectHeader.h"

#include "xgi/Input.h"
#include "xgi/Output.h"  
#include "xgi/Utils.h"

#include "xercesc/util/Base64.hpp"

#include "xcept/tools.h"
//
// Log4CPLUS
//
#include "log4cplus/logger.h"
#include "log4cplus/consoleappender.h"
#include "log4cplus/socketappender.h"
#include "log4cplus/nullappender.h"
#include "log4cplus/fileappender.h"
#include "log4cplus/helpers/appenderattachableimpl.h"
#include "log4cplus/helpers/loglog.h"
#include "log4cplus/helpers/pointer.h"
#include "log4cplus/spi/loggingevent.h"
#include "log4cplus/layout.h"

#include "pt/SecurityPolicyFactory.h"

using namespace log4cplus;
using namespace log4cplus::helpers;
using namespace log4cplus::spi;

pt::http::ReceiverLoop::ReceiverLoop(xdaq::Application* owner, pt::Address::Reference address, Logger & logger, xdata::InfoSpace* is) 
	throw (pt::http::exception::Exception) : 
	xdaq::Object(owner),pt::http::Channel(address), logger_(logger), is_(is), requestCounter_(0)
{

	try
		{			


			try 
			{ 
        			xdata::String aliasName = dynamic_cast<xdata::String*>(is_->find("aliasName"))->toString(); 
        			xdata::String aliasPath = dynamic_cast<xdata::String*>(is_->find("aliasPath"))->toString();           

				std::vector<std::string> paths = toolbox::getRuntime()->expandPathName(aliasPath);
                        	if (paths.size() == 1)
                       		{
					aliases_[aliasName.toString()] = paths[0];
					//std::cout << "----->" << aliases_[aliasName] << " and --->>>" << aliasName.toString() << std::endl;
                        	}
                        	else
                        	{
                                	std::stringstream msg;
                                	msg << "Alias path '" << aliasPath.toString() << "' is ambiguous";
                                	XCEPT_RAISE (pt::http::exception::Exception, msg.str());
                        	}

			}  
			catch (xdata::exception::Exception& e) 
			{ 
         			//ignore  
 			} 


			xdata::Vector< xdata::Bag<pt::http::Alias> >* aliases = dynamic_cast<xdata::Vector< xdata::Bag<pt::http::Alias> >*>(is_->find("aliases"));
			xdata::Vector< xdata::Bag<pt::http::Alias> >::iterator ci;
			for (ci = aliases->begin(); ci != aliases->end(); ++ci)
			{
				std::vector<std::string> paths = toolbox::getRuntime()->expandPathName( (*ci).bag.path.toString());
				if (paths.size() == 1)
				{
					//(*ci).bag.path = paths[0];
					aliases_[(*ci).bag.name.toString()] = paths[0];
				}
				else
				{
					std::stringstream msg;
					msg << "Alias path '" << (*ci).bag.path.toString() << "' is ambiguous";
					XCEPT_RAISE (pt::http::exception::Exception, msg.str());
				}

				LOG4CPLUS_INFO (logger_, "Setting alias of [" << (*ci).bag.name.toString() << "] to [" << aliases_[(*ci).bag.name] << "]");
			}
		}
		catch (xdata::exception::Exception& e)
		{
			// no alias declaration found
			LOG4CPLUS_INFO (logger_, "aliases not found in configuration");
		}


	try
	{		
		std::vector<std::string> paths = toolbox::getRuntime()->expandPathName
							(
								dynamic_cast<xdata::String*>(is_->find("documentRoot"))->toString()
							);
		if (paths.size() == 1)
		{
			httpRootDir_ = paths[0];
		}
		else
		{
			std::stringstream msg;
			msg << "documentRoot path '" << dynamic_cast<xdata::String*>(is_->find("documentRoot"))->toString() << "' is ambiguous";
			XCEPT_RAISE (pt::http::exception::Exception, msg.str());
		}
	}
	catch (toolbox::exception::Exception& e)
	{
		std::stringstream msg;
		msg << "Failed to expand documentRoot path '" << dynamic_cast<xdata::String*>(is_->find("documentRoot"))->toString() << "'";
		XCEPT_RETHROW (pt::http::exception::Exception, msg.str(), e);
	}
	catch (xdata::exception::Exception& e)
	{
		char* hr = std::getenv("XDAQ_ROOT");
	
		if (hr == (char*) 0)
		{
			httpRootDir_ = ".";
		} 
		else
		{
			httpRootDir_ = hr;
		}
	}
	
	// dectect if security policy is installed
	policy_  = 0;
	try 
	{
		policy_ = dynamic_cast<pt::HTAccessSecurityPolicy*>(pt::getSecurityPolicyFactory()->getSecurityPolicy("urn:xdaq-security:htaccess"));
	}
	catch (pt::exception::Exception & e )
	{
		LOG4CPLUS_INFO(logger_,"No security policies on this server");
	}
		
	listener_ = 0;
	cgiListener_ = 0;
	// address_ = new http::Address(address->getURL());
	address_ = address;

	errno = 0;
	
	if (::bind(socket_, (struct sockaddr *)&sockaddress_, sockaddressSize_) == -1)
   	{
		std::string msg = "Cannot start HTTP receiver loop. ";
		msg += strerror(errno);
      		::close(socket_);
      		socket_ = 0;
		XCEPT_RAISE(pt::http::exception::Exception, msg);
   	}

	errno = 0;
	if (::listen (socket_, MaxNoChannels) == -1)
   	{ 
		std::string msg = "Cannot start HTTP receiver loop. ";
		msg += strerror(errno);
     		::close(socket_);
      		socket_ = 0;
		XCEPT_RAISE(pt::http::exception::Exception, msg);
   	}

	listenfd_ = socket_;
	
	
	FD_ZERO(&allset_);
	FD_SET(listenfd_,&allset_);
	accepted_ = 0;
	nochannels_ = 0;
	sockets_.resize(MaxNoChannels);

	maxfd_ = listenfd_;
	FD_ZERO(&fdset_);

	for (size_t i = 0; i < sockets_.size(); i++ ) 
	{
        	 sockets_[i] = -1;
	}
	
	current_ = -1;
	
	process_ = toolbox::task::bind(this, &http::ReceiverLoop::process, "process");
}
	
pt::http::ReceiverLoop::~ReceiverLoop()
{
	// cancel workloop
	std::string name = "urn:toolbox-task-workloop:";
	name += address_->toString();
	toolbox::task::getWorkLoopFactory()->getWorkLoop(name, "waiting")->cancel();

	for (size_t i = 0; i < sockets_.size(); i++ )
        {
                 if (( sockets_[i] != -1 ) && ( sockets_[i] != -2 ))
		 {
			::close(sockets_[i]);
		 }
        }
}

void pt::http::ReceiverLoop::addServiceListener (pt::Listener* listener)
{
	if (listener->getService() == "soap")
	{
		listener_ = dynamic_cast<pt::SOAPListener *>(listener);
	}
	if (listener->getService() == "cgi")
	{
		cgiListener_ = dynamic_cast<xgi::Listener *>(listener);
	}
}

void pt::http::ReceiverLoop::removeServiceListener (pt::Listener* listener)
{
	if (listener->getService() == "soap")
	{
		listener_ = 0;
	}
	if (listener->getService() == "cgi")
	{
		cgiListener_ = 0;
	}
}

void pt::http::ReceiverLoop::removeAllServiceListeners()
{
	listener_ = 0;
	cgiListener_ = 0;
}

pt::Address::Reference pt::http::ReceiverLoop::getAddress()
{
	return address_;
}
	

void pt::http::ReceiverLoop::disconnect() throw (pt::http::exception::Exception) 
{
	if ( current_ >= 0 ) 
	{
		::close(sockets_[current_]);
		FD_CLR(sockets_[current_], &allset_);
		sockets_[current_] = -2; // closed by sender, one can still reply by reconnecting
	}
	else 
	{
		XCEPT_RAISE(pt::http::exception::Exception, "Disconnect error, no receive socket available");
	}
}

	
// the implementation of connect for the server is actually a re-connect to the client	
void pt::http::ReceiverLoop::connect() throw (pt::http::exception::Exception) 
{
	if ( current_ >= 0 ) 
	{
		errno = 0;
	    	if (::connect(sockets_[current_], (struct sockaddr *)&sockaddress_, sockaddressSize_) == -1)
	    	{
			std::string msg = strerror(errno);
			::close(socket_);
			XCEPT_RAISE(http::exception::CannotConnect, msg);
	    	}
    	}
    	else
    	{
		XCEPT_RAISE(http::exception::CannotConnect, "re-connect error, no free socket available");
	}	
}
	
	
ssize_t pt::http::ReceiverLoop::receive(char * buf, size_t len ) throw (pt::http::exception::Exception) 
{
	ssize_t length;
	if ( current_ >= 0 ) 
	{
		errno = 0;
		length = ::recv(sockets_[current_], buf, len, 0);

		if (length == -1)
		{
			std::string msg = strerror(errno);
			//::close(sockets_[current_]);
			//sockets_[current_] = 0;
			//this->close();
			XCEPT_RAISE(pt::http::exception::Exception, msg );

		}
		if ( length == 0 ) 
		{ 
			std::string msg = strerror(errno);
			//::close(sockets_[current_]);
			//sockets_[current_] = 0;
			//this->close();
			XCEPT_RAISE(pt::http::exception::Exception, "connection reset by peer");
		}
	    
    	}
    	else
    	{
		XCEPT_RAISE(pt::http::exception::Exception, "receive error, no socket available");
	}	

	return length;
}
	
void pt::http::ReceiverLoop::activate()
{
	//
	// URN of the work loop:
	// urn:toolbox-task-workloop:http/PeerTransportReceiver@101.102.103.104,40000
	//
	std::string name = "urn:toolbox-task-workloop:";
	name += address_->toString();
	toolbox::task::getWorkLoopFactory()->getWorkLoop(name, "waiting")->submit(process_);
	toolbox::task::getWorkLoopFactory()->getWorkLoop(name, "waiting")->activate();
}
	
void pt::http::ReceiverLoop::close() throw (pt::http::exception::Exception) 
{
	if ( current_ >= 0 ) 
	{
		::close(sockets_[current_]);
		FD_CLR(sockets_[current_], &allset_);
		sockets_[current_] = -1;
	}
	else
    	{
		XCEPT_RAISE(pt::http::exception::Exception, "close error, no socket available");
	}	
}
	
	
void pt::http::ReceiverLoop::send(const char * buf, size_t len) throw (pt::http::exception::Exception) 
{
	if ( current_ < 0 ) 
	{
		XCEPT_RAISE(pt::http::exception::Exception, "send error, no socket available");
	}	

	size_t toWrite = len;
        ssize_t nBytes = 0;

        while (toWrite > 0) 
	{
		errno = 0;

		//std::cout << "Going to send " << len << " bytes" << std::endl;
		
   		nBytes = ::send(sockets_[current_], buf, len, 0); 
		
		//std::cout << "Sent: " << nBytes << ", remaining: " << toWrite-nBytes;
		//std::cout << ", total: " << len << std::endl;
		
   		if ( nBytes == -1)
   		{  
			std::string msg = strerror(errno);
      			this->close();
			XCEPT_RAISE(pt::http::exception::Exception, msg);
   		}
   		if ( nBytes == 0 ) 
		{ 
			std::string msg = strerror(errno);
			this->close();
			XCEPT_RAISE(pt::http::exception::Exception, msg);
   		}
		
                toWrite -= nBytes;
        }
}
	
	
bool pt::http::ReceiverLoop::isConnected() throw (pt::http::exception::Exception) 
{
	if ( current_ >= 0 ) 
	{
		// the channel is neither dead nor disconnected
		return (sockets_[current_] != -1 && (sockets_[current_] != -2));
	}
	else
    	{
		XCEPT_RAISE(pt::http::exception::Exception, "connection state, no socket available");
	}	
}
	
	
bool pt::http::ReceiverLoop::isActive() throw (pt::http::exception::Exception) 
{
	if ( current_ >= 0 ) 
	{
		// the channel is still alive, just  disconnected
		return (sockets_[current_] == -2);
	}
	else
    	{
		XCEPT_RAISE(pt::http::exception::Exception, "socket state, no socket available");
	}	
}
	
	
int pt::http::ReceiverLoop::accept(std::string & ip, std::string & hname) throw (pt::http::exception::Exception) 
{
	int newsock;
	struct sockaddr_in  readAddr;
	socklen_t readAddrLen = sizeof(readAddr);
	
	errno = 0;
	if ( (newsock = ::accept( listenfd_, (struct sockaddr *) &readAddr, &readAddrLen) ) == -1 )
   	{
		std::string msg = strerror(errno);
 		if ( errno == EMFILE )
                {
                        XCEPT_RAISE(pt::http::exception::Exception, msg);
                }
                else if (errno == ECONNABORTED )
                {
                        XCEPT_RAISE(pt::http::exception::Exception, msg);
                }
                else if ( errno == ENFILE )
                {
                        XCEPT_RAISE(pt::http::exception::Exception, msg);
                }
                else
                {
                        ::close(listenfd_);
                        listenfd_ = 0;
                        XCEPT_RAISE(pt::http::exception::Exception, msg);
                }

   	}
	
	ip = inet_ntoa(readAddr.sin_addr);
	struct hostent * h = gethostbyaddr((char*)&readAddr.sin_addr.s_addr, sizeof(readAddr.sin_addr.s_addr), AF_INET);
     	std::string remoteHostName;
	if ( h != 0 ) 
		hname = h->h_name;
	else 
		hname = "";	

	//std::cout << "-------[" << ip << "]-----------" << std::endl;
	//std::cout << "-------[" << hname << "]-----------" << std::endl;
	return newsock;

    //strncpy(addr->host,inet_ntoa(readAddr_.sin_addr),256);
}	

	
bool pt::http::ReceiverLoop::process (toolbox::task::WorkLoop * wl) 
{
		
	//for (;;)
	//{
		fdset_ = allset_;

		// Waiting for incoming connections and data. There is no timeout used right now
		//
		struct timeval timeout;
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		nready_  = ::select(maxfd_+1, &fdset_, 0,  0, &timeout);

		if (nready_ == 0 ) 
		{
			return true;
			// continue; // nothing to read
		}

		if ( FD_ISSET(listenfd_,&fdset_) ) 
		{  
			std::string ip = "";
			std::string host = "";
			// accept connection and return FALSE
			//
			try
			{
				int newsock = this->accept(ip, host);
	
				if (newsock > MaxNoChannels ) 
				{
					std::string msg = toolbox::toString ("Incoming connection refused. Maximum number of %d channels reached", MaxNoChannels);
					LOG4CPLUS_WARN(logger_, msg);
					//continue;
					return true;
				}
	
				if (newsock > maxfd_ ) maxfd_ = newsock;
	
				FD_SET(newsock,&allset_);
	
				sockets_[newsock] = newsock;
				clientIP_[newsock] = ip;
				clientHost_[newsock] = host;

				accepted_++;
			}
			catch (pt::http::exception::Exception & e )
			{
  				LOG4CPLUS_WARN(logger_, "failed to accept connection: " <<  xcept::stdformat_exception_history(e));
                                //XCEPT_DECLARE_NESTED (pt::http::exception::Exception, ex, "Failed to accept incoming connection", e);
                                //this->getOwnerApplication()->notifyQualified("error", ex);

			}
			return true;
		}

		for (size_t i=0; i< sockets_.size(); i++ ) 
		{ 
			// check all clients for data
			//
			if (sockets_[i] < 0 )
				continue;

			if ( FD_ISSET(sockets_[i],&fdset_) )
			{
				current_ = i;
				try
				{
					// Socket is closed inside if needed
					this->onRequest(clientIP_[current_], clientHost_[current_]);
				}
				catch(pt::http::exception::Exception & e)
				{
					LOG4CPLUS_ERROR(logger_,xcept::stdformat_exception_history(e));
					// connection already closed
				}	
			}
		}
	//}
	return true;
}

void pt::http::ReceiverLoop::onRequest(const std::string & ip, const std::string & host) throw (pt::http::exception::Exception) 
{
	toolbox::TimeVal starttime = toolbox::TimeVal::gettimeofday();
	std::string method, url, protocol;
	// Extract urn from http header and pass it together with the SOAP message
	char* buffer;
	try
	{
		buffer = http::Utils::receiveHeaderFrom(this);	
		
		//std::cout << "-->" << buffer << "<--" << std::endl;
	} 
	catch (pt::http::exception::Exception& he)
	{
		this->close();
		//XCEPT_RETHROW(pt::http::exception::Exception, "Cannot receive HTTP header", he);
		LOG4CPLUS_TRACE(logger_, xcept::stdformat_exception_history(he));
		return;
	}
	
	std::auto_ptr<char> requestHeader(buffer);
	
	std::istringstream hin(requestHeader.get());

	hin >> method >> url >> protocol >> std::ws; // read the POST/GET method, the url and the HTTP protocol version
		
	LOG4CPLUS_TRACE(logger_, "HTTP Request [" << method << "] [" << url << "] [" << protocol << "]" );
	
	std::string name, value;	
	std::map<std::string,std::string, std::less<std::string> > headers;

	headers["request_method"] = method;
	headers["request_uri"] = url;
	headers["server_protocol"] = protocol;
	
	std::stringstream ss;
	ss << std::fixed << std::setprecision(6) << (double)starttime;
	headers["x-xdaq-receivetimestamp"] = ss.str();
	headers["x-xdaq-remote-addr"] = ip;
	headers["x-xdaq-remote-host"] = host;

	xgi::Output out; // stream for html/soap page reply
	xgi::Output headerOut;
	headerOut.clear(); // make sure that the streams are empty
	out.clear();
	
	while (! hin.eof())
	{
		name = "";
		value = "";		
		getline(hin, name, ':');
		if ( name == "\r\n" ) break;
		getline(hin,value,'\n');
		value = toolbox::trim(value," \v\t"); /* added */  
	
		if ( (value.size() == 0 ) || ( *(--value.end())!= '\r') ) 
		{
			out.getHTTPResponseHeader().getStatusCode(500);
			out.getHTTPResponseHeader().getReasonPhrase(xgi::Utils::getResponsePhrase(500));
			// Send only HTTP header
			std::ostringstream s;
			s << out.getHTTPResponseHeader();
			this->reply((char*)s.str().c_str(), s.tellp() );
			this->close(); // close connection
			return;
		}
		
		name = toolbox::tolower(name);
		
		if  ( headers.find(name) != headers.end() )
		{
			headers[name] += ",";
			headers[name] += value.erase(value.size()-1, 1);
		}
		else
		{
			headers[name] = value.erase(value.size()-1, 1);
		}
		
	}

	std::map<std::string,std::string, std::less<std::string> >::iterator iter;
	for( iter = headers.begin() ; iter != headers.end() ; ++iter)
	{	
		LOG4CPLUS_TRACE(logger_, "Header: [" << (*iter).first << "], value: " << (*iter).second );
	}

	// std::cout << "Agent: " << headers["user-agent"] << std::endl;
	LOG4CPLUS_TRACE (logger_, headers["user-agent"]);
	
	// Read Body
	std::string cl = headers["content-length"];
	size_t requestSize = 0;
	std::auto_ptr<char> requestBuffer;
	if ((cl != "") && (method == "POST"))
	{
		// requestSize = std::atoi(headers["content-length"].c_str());
		try
		{
			requestSize = toolbox::toUnsignedLong(headers["content-length"]);
			// std::cout << "Receive size: " << requestSize << std::endl;
		}
		catch (toolbox::exception::Exception& te)
		{
			this->close();
			LOG4CPLUS_WARN(logger_, xcept::stdformat_exception_history(te));
			return;
		}
		
		try
		{
			// if ((requestSize != 0 ) && (headers["connection"] == "keep-alive" ))
			if (requestSize != 0 )
			{
				requestBuffer.reset(http::Utils::receiveBodyFrom(this, &requestSize));
			}
			/*
			else
			{
				// A POST request MUST always have a content-length information field
				std::ostringstream s;
				out.getHTTPResponseHeader().getStatusCode(400);
				out.getHTTPResponseHeader().getReasonPhrase(xgi::Utils::getResponsePhrase (400));
				s << out.getHTTPResponseHeader();
				this->reply((char*)s.str().c_str(), s.tellp() );
				this->close(); // close connection
				return;
			}
			*/
		} 
		catch (pt::http::exception::Exception& he)
		{
			this->close();
			LOG4CPLUS_TRACE(logger_, xcept::stdformat_exception_history(he));
			//XCEPT_RETHROW (pt::http::exception::Exception, "Cannot receive HTTP body", he);
			return;
		}
	}
	else
	{
		// check if it is a GET or HEAD
		// if ((method != "GET") && (method != "HEAD"))
		if (method != "GET")
		{
			std::ostringstream s;
			out.getHTTPResponseHeader().getStatusCode(405);
			out.getHTTPResponseHeader().getReasonPhrase(xgi::Utils::getResponsePhrase (405));
			out.getHTTPResponseHeader().addHeader("Allow", "GET POST");
			s << out.getHTTPResponseHeader();
			this->reply((char*)s.str().c_str(), s.tellp() );
			this->close(); // close connection
			return;
		}
	}

	// std::cout << "Buffer: [" << requestBuffer.get() << "]" << std::endl;

	bool isSOAP = false;
	// Handle SOAP request, if HTTP headers contain a content-location field and content-type contains application/soap+xml
	if ( headers.find("content-location") != headers.end() )
	{		
		if (headers.find("content-type") != headers.end())
		{		
			if ( (headers["content-type"].find("application/soap+xml") != std::string::npos) ||
			     (headers["content-type"].find("text/xml") != std::string::npos )) 
			    {
				isSOAP = true;
			    }
		}
	}

	// Also accept old deprecated SOAPAction header field 
	if ( (isSOAP == false) && (headers.find("soapaction") != headers.end()) )
	{
		isSOAP = true;
		LOG4CPLUS_DEBUG (logger_, "Received SOAP request from [" << host << "] to [" << headers["soapaction"] << " with deprecated field 'SOAPAction', use Content-Location instead. In addition set Content-Type to 'application/soap+xml'.");
	}
		
	if(isSOAP)
	{
		xoap::MessageReference reply;
	
		if (requestBuffer.get() == 0)
		{
			reply = xoap::createMessage();
			xoap::SOAPBody b = reply->getSOAPPart().getEnvelope().getBody();
			xoap::SOAPFault f = b.addFault();
			f.setFaultCode ("Server");
			f.setFaultString ("Empty SOAP message");
			reply->writeTo (out); // write SOAP contents to a buffer
		}
		else if (listener_ == 0)
		{
			LOG4CPLUS_FATAL(logger_, "No listener for HTTP/SOAP available, message discarded");
			
			reply = xoap::createMessage();
			xoap::SOAPBody b = reply->getSOAPPart().getEnvelope().getBody();
			xoap::SOAPFault f = b.addFault();
			f.setFaultCode ("Server");
			f.setFaultString ("No listener for HTTP/SOAP available, message discarded");
			reply->writeTo (out); // write SOAP contents to a buffer
		}
		else
		{
			++requestCounter_;

			/*
			if ((requestCounter_ % 500) == 0)
			{
				std::cout << "Received 500 requests, time: " << toolbox::TimeVal::gettimeofday().toString(toolbox::TimeVal::loc) << std::endl;
			}
			*/

			try 
			{
				xoap::MessageReference msg = xoap::createMessage(requestBuffer.get(), requestSize);
				// Extract urn from http header and pass it together with the SOAP message
				// msg->getMimeHeaders()->addHeader("Content-Location", urn);

				// Add all mime headers to the xoap message
				xoap::MimeHeaders* mimeHeaders = msg->getMimeHeaders();			
				std::map<std::string,std::string, std::less<std::string> >::iterator hi;
				for (hi = headers.begin(); hi != headers.end(); ++hi)
				{						
					mimeHeaders->addHeader( (*hi).first, (*hi).second );
				}

				reply = listener_->processIncomingMessage( msg );
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

			// serialize the SOAP reply message into the reply text buffer
			// reply->writeTo (replyStringBuffer);

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
		}
	}
	else
	{
		if (cgiListener_ == 0)
		{
			std::ostringstream s;
			out.getHTTPResponseHeader().getStatusCode(501);
			out.getHTTPResponseHeader().getReasonPhrase(xgi::Utils::getResponsePhrase (501));
			s << out.getHTTPResponseHeader();
			this->reply((char*)s.str().c_str(), s.tellp() );
			this->close(); // close connection
			return;
		}
		else
		{
			// XCEPT_ASSERT (cgiListener_ != 0, pt::http::exception::Exception, "no listener for HTTP/CGI available, message discared");
			// Handle HTTP request {GET | POST}, others are rejected
			
			// std::cout << "URL encoded: " << url << std::endl;
			// see also: www.notepad.co.uk/articles/php_tutorial_005.php
			url = cgicc::form_urldecode(url);
			
			xgi::Input in(requestBuffer.get(),requestSize);

			// prepare environment
			http::Address& a = dynamic_cast<http::Address&>(*address_);
			
			in.putenv("SERVER_SOFTWARE", "XDAQ/3.0");
			in.putenv("SERVER_NAME", a.getHost());
			in.putenv("GATEWAY_INTERFACE", "CGI/1.1");
			in.putenv("SERVER_PROTOCOL", "HTTP/1.1");
			in.putenv("SERVER_PORT", a.getPort());
			in.putenv("REQUEST_METHOD", method); // POST or GET			
			in.putenv("PATH_TRANSLATED", url);
			//----
			size_t start_script = url.find ("/urn:xdaq-application:");
			
			size_t end_script = std::string::npos;
			std::string script = "";
			std::string path_info = "";
			std::string query_string = "";

			//if (start_script != std::string::npos)
			
			// if the urn is found in the beginning of the URL path
			if (start_script == 0)
			{
				// find end of urn, i.e. a '/'
				end_script = url.find ("/", start_script+1);
				if (end_script != std::string::npos)
				{
					// without the leading '/'
					script = url.substr (start_script+1, end_script - (start_script+1));
				}
				else
				{
					script = url.substr(start_script+1);
				}
			

				if (end_script != std::string::npos)
				{
					size_t q_pos = url.find ("?", end_script+1);
					if (q_pos == std::string::npos)
					{
						path_info = url.substr (end_script+1);
					}
					else
					{
						query_string = url.substr(q_pos+1);
						path_info = url.substr (end_script+1, q_pos - end_script - 1);
					}
				}
			}
			
			in.putenv("SCRIPT_NAME",script); // actually the full path to the executing program
			in.putenv("PATH_INFO", path_info); // www.cern.ch/pippo.cgi/path -> path
			in.putenv("QUERY_STRING",query_string); // everthing after question mark
			in.putenv("REMOTE_HOST", host); //it comes from the socket connection
			in.putenv("REMOTE_ADDR", ip);  // IP from client (it comes from the socket connection
			
			// extract authorization type
			std::string atype = headers["authorization"];
			if (atype != "")
			{
				size_t sp = atype.find(' ');
				in.putenv("AUTH_TYPE",atype.substr(0, sp)); // authentication
				
				// std::cout << "Authentication type : [" << atype.substr(0,sp) << "]" << std::endl;
				
				std::string info = atype.substr(sp+1);
				
				unsigned int decodedLength;
				XMLCh* decoded = Base64::decode(xoap::XStr(info), &decodedLength);
				
				// std::cout << "authentication info: [" << xoap::XMLCh2String(decoded) << "]" << std::endl;
				
				in.putenv("REMOTE_USER", xoap::XMLCh2String(decoded)); // authenticated user
				
				delete decoded;
			}
			else
			{			
				in.putenv("AUTH_TYPE",""); // authentication
				in.putenv("REMOTE_USER",""); // authenticated user
			}
			
			in.putenv("REMOTE_IDENT",""); // according RFC931
			in.putenv("CONTENT_TYPE", headers["content-type"]); //
			in.putenv("CONTENT_LENGTH", headers["content-length"]);
			in.putenv("HTTP_ACCEPT", headers["accept"]);
			in.putenv("HTTP_USER_AGENT",headers["user-agent"]);
			in.putenv("REDIRECT_REQUEST", "");
			in.putenv("REDIRECT_URL", "");
			in.putenv("REDIRECT_STATUS", "");
			in.putenv("HTTP_REFERER", headers["referer"]);
			in.putenv("HTTP_COOKIE", headers["cookie"]);
			
			//----
			
			if (policy_ != 0 )
			{
				if ( policy_->isAccessLimited(method) )
				{
					// reset input stream
					in.cin().seekg(0);
					if ( ! this->verifyAccess(&in, &out, policy_) )
					{
						// the client has not authorized access
						// Send back HTTP header
						std::ostringstream header;
						header << out.getHTTPResponseHeader();	
						this->reply((char*)header.str().c_str(), header.tellp() );
						this->reply((char*) out.str().c_str(), out.tellp());  
						this->close(); // close connection	
						return;
					} 
				
				}
				
				if ( policy_->isAuthLimited(method) )
				{
					in.cin().seekg(0);
					if ( ! this->authenticateUser(&in, &out, policy_) )
					{
						// the user is not authenticated
						// Send back HTTP header
						std::ostringstream header;
						header << out.getHTTPResponseHeader();	
						this->reply((char*)header.str().c_str(), header.tellp() );
						this->reply((char*) out.str().c_str(), out.tellp());  
						this->close(); // close connection	
						return;
					} 
				}
				
				
			}
			
			
			//----
			
			/*else
			{*/
			
			if(url == "/")
			{					
				// By defauklt the server redirect to HyperDAQ. The user can require a diferent redirect by setting
				// the environment variable XDAQ_HTTP_REDIRECT.
				std::string redirect;

				char* rurl = std::getenv("XDAQ_REDIRECT");

				if (rurl == (char*) 0)
				{
					redirect = a.getProtocol();
					redirect += "://";
					redirect += a.getHost();
					redirect += ":";
					redirect += a.getPort();
					redirect += "/urn:xdaq-application:service=hyperdaq";
				} 
				else
				{
					redirect = rurl; // as specified in environment
				}


				out.getHTTPResponseHeader().getStatusCode(301);
				out.getHTTPResponseHeader().getReasonPhrase(xgi::Utils::getResponsePhrase (301));
				out.getHTTPResponseHeader().addHeader ("Location", redirect);
				std::ostringstream s;
				s << out.getHTTPResponseHeader();
				this->reply((char*)s.str().c_str(), s.tellp() );
				this->close(); // close connection
				return; 							
			}
			
			if ( script == "" )
			{
				std::string filename;
				filename = httpRootDir_ + url;

				//std::cout << " default path:" << filename << std::endl;

				// if an alias is defined,  and found after the "/", use it
				for (std::map<std::string,std::string>::iterator i = aliases_.begin(); i != aliases_.end(); i++ )
				{
					//std::cout << "matching alias for:" << (*i).first << " on path :" << (*i).second << " url:" << url << std::endl;
					//std::cout << " find position: " << url.find((*i).first) << std::endl;
					if ( ((*i).first != "" ) && (url.find((*i).first) == 0)  )
					{
						filename = (*i).second  +  url.substr(((*i).first).size());	
						//std::cout << " override  path:" << filename << std::endl;
						break;
					}
				}
				
				// If there is a '?' character, chop everything including this character
				size_t qposition = filename.find( '?', 0 );
				if (qposition != std::string::npos)
				{
					filename.erase (qposition, std::string::npos);
				}

				LOG4CPLUS_TRACE(logger_, "Serving file: [" << filename << "]");
			
				// Try to read the file and serve it...
				std::string extension = url.substr(url.find(".")+1);
				std::ios_base::openmode mode = std::ios::in;
				std::string contentType = "";
				if (extension == "gif" || extension == "jpg")
				{
					contentType = "image/"+extension;
					mode |= std::ios::binary;
				}
				else if (extension == "js")
				{
					contentType = "text/javascript";
				}
				else if (
						(extension == "so") || 
						(extension == "o") || 
						(extension == "dylib") ||
						(extension == "class") ||
						(extension == "jar")
					)
				{
					contentType = "binary/"+extension;
					mode |= std::ios::binary;
				}
				else if (extension == "swf")
				{
					contentType = "application/x-shockwave-flash";
					mode |= std::ios::binary;
				}
				else if (extension == "txt")
				{
					contentType = "text/plain";
				}
				else
				{
					contentType = "text/"+extension;
				}
				
				// std::cout << "Serving file: " << filename << ", extension: " << extension << std::endl;
				// std::cout << "Content type: " << contentType << std::endl;
				std::ifstream f(filename.c_str(), mode);
				
				if (f.is_open())
				{	
					struct stat stat_buf;
					stat (filename.c_str(), &stat_buf);
							
					out.getHTTPResponseHeader().addHeader("Content-Type", contentType);
					std::stringstream fileSize;
					fileSize << stat_buf.st_size;
					out.getHTTPResponseHeader().addHeader ("Content-Length", fileSize.str().c_str());
					out.getHTTPResponseHeader().addHeader ("Expires", "0");

					char c;
										
					while (f.get(c))
					{
						out.put(c);
					}
					
					f.close();
					
					std::ostringstream s;
					s << out.getHTTPResponseHeader();
					this->reply((char*)s.str().c_str(), s.tellp() );
					this->reply((char*)out.str().c_str(), out.tellp() );
					// keep connection open if requested by client
					 std::map<std::string,std::string, std::less<std::string> >::iterator hi = headers.find("connection");
                			if ( (hi == headers.end()) ||  ((*hi).second.find("keep-alive") == std::string::npos) )
                			{
				 		//std::cout << "user did not use keep alive"  << std::endl;
                        			this->close();
                			}
				}
				else
				{
					// Return code 404
					//
					(void) out.getHTTPResponseHeader().getStatusCode(404);
					(void) out.getHTTPResponseHeader().getReasonPhrase(xgi::Utils::getResponsePhrase(404));
					(void) out.getHTTPResponseHeader().addHeader("Content-Type", "text/html");
					std::string msg = "Requested file not found: ";
					msg += url;
					out << xgi::Utils::getFailurePage("HTTP Request Error", msg);
					std::ostringstream s;
					s << out.getHTTPResponseHeader();
					this->reply((char*)s.str().c_str(), s.tellp() );
					this->reply((char*)out.str().c_str(), out.tellp() );
					this->close(); // close connection				
				}
				return; 				
			}
			


			try
			{	
				// Force page expiration, may be overridden by user callback
				out.getHTTPResponseHeader().addHeader ("Expires", "0");

				in.cin().seekg(0);
				cgiListener_->processIncomingMessage (&in, &out);
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
		}
	}

/*
 // Add XDAQ powered
        std::string pattern = "<html";
         std::cout << "Find pattern" << std::endl;
	const std::vector<std::string>& heads = out.getHTTPResponseHeader().getHeaders();
	for (std::vector<std::string>::const_iterator hi = heads.begin(); hi != heads.end(); hi++ )
	{
		if ( (toolbox::stl::cifind((*hi),"text/html") != std::string::npos ) 
			&& (toolbox::stl::cifind((*hi),"content-type") != std::string::npos ) 
			&&  ( toolbox::stl::cifind(out.str(), pattern) != std::string::npos ))
        	{
		#define QUOTE(str) #str
		#define EXPAND_AND_QUOTE(str) QUOTE(str)
		#define RELEASE EXPAND_AND_QUOTE(PACKAGE_RELEASE)

		std::string release = RELEASE;
		toolbox::TimeVal copyrightDate = toolbox::TimeVal::gettimeofday();
		std::string home = this->getOwnerApplication()->getApplicationDescriptor()->getContextDescriptor()->getURL();
                out << "<div style=\"height: 35px\"/> <div style=\"background: transparent; font: normal 13px Verdana,Arial,'Bitstream Vera Sans',Helvetica,sans-serif;bottom: 0;position: fixed;width: 100%;clear: both;color: #bbb;font-size: 10px;border-top: 1px solid;height: 35px;padding: .25em 0;\"  lang=\"en\" xml:lang=\"en\">";
                out << "<hr style=\"display: none\"/>";
                out << "<a style=\"border: 0; float: left;\" href=\"" << home << "\"  target=\"_blank\">";
                out << "<img STYLE=\"position:relative; TOP:-15px; LEFT:0px; border: none;\" src=\"/pt/http/images/XDAQLogoPoweredNew.png\" alt=\"XDAQ Powered\"/></a>";
                out << "<p style=\"background: transparent; margin: 0; float: left; margin-left: 1em; padding: 0 1em;border-left: 1px solid #d7d7d7;border-right: 1px solid #d7d7d7;\">";
                out << "Powered by <a style=\"color: #bbb;\" href=\"http://xdaq.web.cern.ch/\"  target=\"_blank\"><strong>XDAQ release n. " << release << "</strong></a> - Copyright &copy; " <<  copyrightDate.toString("%Y",toolbox::TimeVal::gmt) << " CERN<br/>";
                out << "Designed by "; 
		out <<  "<a href=\"mailto:Matthew.Bowen@cern.ch\">M. Bowen</a>";
		out <<  " <a href=\"mailto:Luciano.Orsini@cern.ch\">L. B. Orsini</a>";
		out <<  " <a href=\"mailto:Andrea.Petrucci@cern.ch\">A. Petrucci</a></p>";
                out << "<p style=\"background: transparent; margin: 0;float: right; text-align: right; border: .25em 10; padding-right: 15px;\">Visit the CMS XDAQ source project at<br /><a style=\"color: #bbb;\" href=\"http://xdaq.web.cern.ch/\"  target=\"_blank\">http://xdaq.web.cern.ch/</a></p></div>";
		break;

        	}
	}
        //
*/

	// if there's something in the out stream, put the "Content-Length" header
	size_t contentLength = 0;
	if (out.tellp() != std::ostringstream::pos_type(std::ostringstream::off_type(-1)))
	{
		contentLength = out.tellp();
		out.getHTTPResponseHeader().addHeader ("Content-Length", toolbox::toString("%d", contentLength));
	}
	
	// Send back HTTP header
	std::ostringstream header;
	header << out.getHTTPResponseHeader();	
	this->reply((char*)header.str().c_str(), header.tellp() );
	
	if (contentLength > 0)
	{
		this->reply((char*) out.str().c_str(), contentLength);  
		
		std::map<std::string,std::string, std::less<std::string> >::iterator c = headers.find("connection");
		if ( (c == headers.end()) ||  ((*c).second.find("keep-alive") == std::string::npos) )
		{
			this->close();
		}
		
		return; // keep connection open
	} 
	else
	{
		this->close();
		return; // close connection
	}	
}

void pt::http::ReceiverLoop::reply(char* buffer, size_t length) throw (pt::http::exception::Exception) 
{
	if ( ! this->isConnected() ) 
	{
		// check if it is still active, re-connect to the incoming channel if required
		if ( this->isActive() )
		{
			std::cout << "Reply connect" << std::endl;
			this->connect();
		} 
		else 
		{
			XCEPT_RAISE(pt::http::exception::Exception, "Lost incoming client connection, cannot reply");
		}	
	} 	
	
	this->send(buffer, length);  
}

// Helper function for performing case insensitive string comparison
//
int nocase_cmp(const std::string & s1, const std::string& s2) 
{
	std::string::const_iterator it1=s1.begin();
 	std::string::const_iterator it2=s2.begin();
 	//stop when either string's end has been reached
 	while ( (it1!=s1.end()) && (it2!=s2.end()) )
 	{ 
		if(::toupper(*it1) != ::toupper(*it2)) //letters differ?
		// return -1 to indicate smaller than, 1 otherwise
		return (::toupper(*it1)  < ::toupper(*it2)) ? -1 : 1; 
		//proceed to the next character in each string
		++it1;
		++it2;
	}
	size_t size1=s1.size(), size2=s2.size();// cache lengths
	
	//return -1,0 or 1 according to strings' lengths
	if (size1==size2) return 0;
	return (size1<size2) ? -1 : 1;
}

bool pt::http::ReceiverLoop::authenticateUser(xgi::Input * in, xgi::Output * out, pt::SecurityPolicy * policy ) throw (xgi::exception::Exception)
{
	try 
	{
		cgicc::Cgicc cgi(in);

		const cgicc::CgiEnvironment& env = cgi.getEnvironment();
		std::string serversw = env.getServerSoftware();
		std::string clientsw = env.getUserAgent();
		std::string authtype = env.getAuthType();

		std::string remoteuser = "";
		std::string authdetails = "";

		const std::vector<cgicc::HTTPCookie> & cookieList = env.getCookieList();
		
		// first check if cookie is valid
		for ( std::vector<cgicc::HTTPCookie>::const_iterator iter = cookieList.begin() ; iter != cookieList.end() ; iter++)
		{
			if( (*iter).getName() == "xdaq-auth")
			{
				authdetails = (*iter).getValue();
				// std::cout << "Received cookie value: " << authdetails << std::endl;
				unsigned int decodedLength;
				XMLCh* decoded = Base64::decode(xoap::XStr(authdetails), &decodedLength);
				remoteuser = xoap::XMLCh2String(decoded);
			}
		}

                if(remoteuser.empty() || !dynamic_cast<pt::HTAccessSecurityPolicy*>(policy)->checkAuth(remoteuser))
		{
			// cookie invalid, look if auth details have been specified
			remoteuser = env.getRemoteUser();
			unsigned int encodedLength;
			XMLByte* encoded = Base64::encode((const XMLByte*)remoteuser.c_str(), remoteuser.length(), &encodedLength, NULL);
			authdetails = xoap::XMLCh2String((const XMLCh*)encoded);
		}

		in->putenv("REMOTE_USER", remoteuser);

		if(remoteuser.empty() || !dynamic_cast<pt::HTAccessSecurityPolicy*>(policy)->checkAuth(remoteuser)) 
		{
			//std::cout << "----> requesting authentication" << std::endl;
			out->getHTTPResponseHeader().getStatusCode(401);
			out->getHTTPResponseHeader().getReasonPhrase("Unauthorized");
			out->getHTTPResponseHeader().addHeader("WWW-Authenticate", "Basic realm=\"cgicc\"");

			// do not add html data: browsers should not display this anyway
			//  they should request user/password from the user and re-emit
			//  the same request, only with the authentification info added
			//  to the request 
			*out << cgicc::HTMLDoctype( cgicc::HTMLDoctype::eStrict) << std::endl;
			*out << cgicc::html().set("lang", "EN").set("dir", "LTR") << std::endl;
			*out << cgicc::head() << std::endl;

			*out << cgicc::title("401 Authorization Required")  << std::endl;
			*out << cgicc::head() << std::endl;
			*out << cgicc::body() << std::endl;

			*out << cgicc::h1("401 Authorization Required") << std::endl;
			*out << cgicc::p() << "This server could not verify that you are "
			<< "authorized to access the document requested. Either you "
			<< "supplied the wrong credentials (e.g., bad password), or "
			<< "your browser doesn't understand how to supply the "
			<< "credentials required." << cgicc::p();
			*out << cgicc::hr() << std::endl;
			*out << cgicc::address() << "GNU cgicc \"server\" version " << cgi.getVersion()
			<<  cgicc::address() << std::endl;
			
			//std::cout << "----> requesting authentication end" << std::endl;
			return false;

		}
		else // check credentials
		{
			std::string hostname = toolbox::net::getDNSHostName(toolbox::net::getHostName());

			std::string domainname = "";
			std::string::size_type pos = hostname.find(".");
			if(pos != std::string::npos)
			{
				domainname = hostname.substr(pos);
			}

			out->getHTTPResponseHeader().setCookie(cgicc::HTTPCookie("xdaq-auth", authdetails, "", domainname, 1800, "/", true));
			return true;
		}
	}
	catch(const std::exception& e) 
	{
		std::cout << "authenticateUser, problem in cgi:" << e.what() << std::endl;
		// handle error condition
		return false;

	}
	return false;
}

bool pt::http::ReceiverLoop::verifyAccess(xgi::Input * in, xgi::Output * out, pt::SecurityPolicy * policy  ) throw (xgi::exception::Exception)
{
	try {
	
		
		cgicc::Cgicc cgi(in);


		const cgicc::CgiEnvironment& env = cgi.getEnvironment();
		std::string host = env.getRemoteHost();
		std::string ip = env.getRemoteAddr();
		//std::cout << "----> check access:" << host << std::endl;
		bool authorized =  dynamic_cast<pt::HTAccessSecurityPolicy*>(policy)->checkAccess(host,ip);
		if ( ! authorized )
		{
				out->getHTTPResponseHeader().getStatusCode(403);
				out->getHTTPResponseHeader().getReasonPhrase("Forbidden");

				*out << cgicc::HTMLDoctype( cgicc::HTMLDoctype::eStrict) << std::endl;
				*out << cgicc::html().set("lang", "EN").set("dir", "LTR") << std::endl;
				*out << cgicc::head() << std::endl;

				*out << cgicc::title("403 Forbidden")  << std::endl;
				*out << cgicc::head() << std::endl;
				*out << cgicc::body() << std::endl;

				*out << cgicc::h1("403 Forbidden") << std::endl;
				*out << cgicc::p() << "Access denied "
				<< "invalid host (" << host << "). Contact your XDAQ administrator." << cgicc::p();
				*out << cgicc::hr() << std::endl;
				*out << cgicc::address() << "GNU cgicc \"server\" version " << cgi.getVersion()
				<<  cgicc::address() << std::endl;
				return false;

		}
		else
		{
			return true;
		}
	}
	catch(const std::exception& e) 
	{
		std::cout << "verifyAccess, problem in cgi:" << e.what() << std::endl;
		// handle error condition
		return false;

	}
	return false;		
}

bool pt::http::ReceiverLoop::isBrowserSupported(const std::string& name)
{
	if (name.find("Firefox/1")) return true;
	
	// otherwise false
	return false;
}
