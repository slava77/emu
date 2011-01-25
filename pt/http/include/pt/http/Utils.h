// $Id: Utils.h,v 1.2 2011/01/25 17:36:47 banicz Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2009, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _pt_http_Utils_h_
#define _pt_http_Utils_h_

#include <string>

#include "pt/http/Channel.h"
#include "pt/http/exception/Exception.h"

namespace pt 
{
namespace http
{
	const size_t ReadSegmentSize = 8192;
	const size_t ReadHeaderSize = 2048;
	
	class Utils 
	{
		public:
		
			//! Read the HTTP header up to the empty line
			static char* receiveHeaderFrom (http::Channel * in)
				throw (pt::http::exception::Exception);
			
			/*! Read everything after the HTTP header. 
			     * If the value contained by the \param size pointer is 0, assume
			     * Content-Length: nnnn and return the actual ready size into the parameter
			     */
			static char* receiveBodyFrom   (http::Channel * in, size_t* size)
				throw (pt::http::exception::Exception);
			
			/*! receive  from established HTTP connection and returns data into a char *, and its length in * size,
			 *  return the http header in a string.
			 */
			static char * receiveFrom(http::Channel * in, size_t * size, std::string* header) 
			throw (pt::http::exception::Exception);
			
			//! send data to an established HTTP connection
			//
			static void sendTo(http::Channel * out, char * path, char * host, char* port, const char * buf, size_t len, char* headers) 
				throw (pt::http::exception::Exception);
			
			//! reply data to an established HTTP connection
			//
			static void replyTo(http::Channel * out,char *  path, char * host, char* port, const char * buf, size_t len) 
				throw (pt::http::exception::Exception);
			
			static void replyBodyTo(http::Channel * out, const std::string& data, size_t len) throw (pt::http::exception::Exception);

			static void replyHeaderTo(http::Channel * out, unsigned int code, size_t len) throw (pt::http::exception::Exception);
			
			/*! Convert numbers found in HTTP headers from string to integer, allowed radices are 10 and 16
			    This function is typically used to interpret the chunk sizes			
			*/
			static ssize_t atoi(char *str, unsigned int radix);
			
		protected:
				
			static std::string extractMIMEBoundary(const char * buf , size_t size) throw (pt::http::exception::Exception);
				
			static void writeHttpPostMIMEHeader(http::Channel * out ,char * path, char * host, char* port, std::string & boundaryStr, size_t len, char* headers) 
				throw (pt::http::exception::Exception);
			
			static void writeHttpPostHeader(http::Channel * out,char * path, char * host, char* port, size_t len, char* headers) 
				throw (pt::http::exception::Exception);
			
			static void writeHttpReplyMIMEHeader(http::Channel * out ,char * path, char * host, char* port, std::string & boundaryStr, size_t len) 
				throw (pt::http::exception::Exception);
			
			static void writeHttpReplyHeader(http::Channel * out,char * path, char * host, char* port, size_t len) 
				throw (pt::http::exception::Exception);
		};

}
}
#endif
