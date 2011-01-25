// $Id: Utils.cc,v 1.3 2011/01/25 17:36:47 banicz Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2009, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#include <sstream>
#include "pt/http/Utils.h"
#include "pt/http/exception/Exception.h"
#include "xoap/memSearch.h"
#include "xgi/Utils.h"
#include "toolbox/utils.h"
#include "cgicc/HTMLClasses.h"

// Types of supported content length encodings
// None (0) -> read to end of connection
// CHUNKED (1) -> read chunks, hex encoded sizes
// CONTENT_LENGTH (2) -> read according to indicated number of bytes
//
#define HTTP_HEADER_NONE 0
#define HTTP_HEADER_CHUNKED 1
#define HTTP_HEADER_CONTENT_LENGTH 2

//! Read the HTTP header up to the empty line
char* pt::http::Utils::receiveHeaderFrom (pt::http::Channel * in) 
	throw (pt::http::exception::Exception)
{
	char* buffer = new char[http::ReadHeaderSize];
	size_t available = http::ReadHeaderSize;
	size_t dataOffset = 0;
	ssize_t nBytes = 0;

	// Read lines (up to a CRLF) in 1 bytes quantities up to an empty line => \r\n\r\n
	size_t start = 0;
	size_t cursor = start;
	unsigned int foundCR = 0; // number of '\r'
	unsigned int foundLN = 0; // number of '\n'
	
	do 
	{
		try
		{
			nBytes = in->receive(&buffer[cursor], 1);
		} 
		catch (pt::http::exception::Exception& he)
		{
			std::stringstream msg;
			msg << "Failed to receive HTTP header, " << cursor << " bytes read so far, content '" << buffer << "'";
			delete buffer;
			XCEPT_RETHROW (pt::http::exception::Exception, msg.str(), he);
		}

		if ( nBytes > 0 ) 
		{	
			available -= nBytes;

			if (available == 0) 
			{				
				delete buffer;
				XCEPT_RAISE (pt::http::exception::Exception, "Minimum size for reading HTTP header too small (redimension ReadHeaderSize)");
			}

			if ( buffer[cursor] == '\n'  )  
			{
				foundLN++;
			}
			else if ( buffer[cursor] == '\r' ) 
			{
				foundCR++;

			}
			else 
			{
				foundLN = 0;
				foundCR = 0;
			}
			if ( foundLN == 2 ) 
			{
				cursor += nBytes;
				dataOffset = cursor;
				break;
			}
		}
		cursor += nBytes;
	} while (nBytes > 0);
	
	if ( foundLN != 2 )  
	{		
		delete buffer;
		XCEPT_RAISE(pt::http::exception::Exception, "Connection closed by peer (wrong http header, empty line missing)");
	}	
	buffer[cursor] = '\0';
	return buffer;
}
			
char* pt::http::Utils::receiveBodyFrom (pt::http::Channel * in, size_t* size) 
	throw (pt::http::exception::Exception)
{
	char* buffer = new char[http::ReadSegmentSize];
	size_t available = http::ReadSegmentSize;
	size_t dataOffset = 0;
	ssize_t nBytes = 0;
	size_t cursor = 0;
	size_t toRead;
	
	// reset cursor when start reading the payload, restart from the buffer[0]
	// This means that the HTTP header will be overwritten and is no longer available.
	//
	available += dataOffset;
	
	// if *size != 0, read according to given size
	if  (*size != 0) 
	{
		toRead = *size;

		// Allocate all memory needed, if size is known
		if (toRead > available) 
		{				
				char * newBuffer = new char[toRead + 1];				
				memcpy (newBuffer, buffer, cursor);				
				delete buffer;				
				buffer = newBuffer;
				available = toRead;
		}

		do 
		{
			try
			{
				nBytes = in->receive(&buffer[cursor], toRead);
			} 
			catch (pt::http::exception::Exception& he)
			{
				delete buffer;
				std::stringstream msg;
				msg << "Failed to receive HTTP message body, " << cursor << " bytes received so far";
				XCEPT_RETHROW (pt::http::exception::Exception, msg.str(), he);
			}
			available -= nBytes;

			if ( nBytes == 0 ) 
			{
				delete buffer;
				// END OF CONNECTION
				XCEPT_RAISE(pt::http::exception::Exception, "connection was closed by peer (request not completed)");
			}
			toRead -= nBytes;
			cursor += nBytes;
		} while (toRead > 0);

		buffer[cursor] = '\0';
	}
	else
	{      
		do 
		{
			try
			{
				nBytes = in->receive(&buffer[cursor], available);
			} 
			catch (pt::http::exception::Exception& he)
			{
				delete buffer;
				std::stringstream msg;
				msg << "Failed to receive HTTP message body, " << cursor << " bytes received so far";
				XCEPT_RETHROW (pt::http::exception::Exception, msg.str(), he);
			}	
			cursor += nBytes;
			available -= nBytes;

			if (available == 0) 
			{
				// Allocate more buffer memory if needed
				// Add one mtuSize_ each time.

				size_t toAllocate = ReadSegmentSize + cursor + 1;
				char * newBuffer = new char [toAllocate];
				memcpy (newBuffer, buffer, cursor);
				delete buffer;
				buffer = newBuffer;
				available = ReadSegmentSize;
			}
		} while ( nBytes > 0 );

		in->disconnect();
                buffer[cursor] = '\0';
		*size = cursor;
	}

	return buffer;
}

char * pt::http::Utils::receiveFrom(pt::http::Channel * in, size_t * size, std::string* header)  
	throw (pt::http::exception::Exception)
{
	char* buffer = new char[ReadSegmentSize];
	ssize_t available = ReadSegmentSize;
	ssize_t dataOffset = 0;
	ssize_t nBytes = 0;

	// Read lines (up to a CRLF) in 1 bytes quantities
	// up to an empty line => \n\n
	ssize_t start = 0;
	ssize_t cursor = start;
	unsigned int foundCR = 0; // number of '\n'
	unsigned int foundLN = 0;
	
	do 
	{
		try
		{
			nBytes =  in->receive(&buffer[cursor], 1);
		} 
		catch (pt::http::exception::Exception& he)
		{
			std::stringstream msg;
			msg << "Failed to receive HTTP header, " << cursor << " bytes received so far, content '" << buffer << "'";
			delete buffer;
			XCEPT_RETHROW (pt::http::exception::Exception, msg.str(), he);
		}

		if ( nBytes > 0 ) 
		{	
			available -= nBytes;

			if (available == 0) 
			{				
				delete buffer;
				XCEPT_RAISE (pt::http::exception::Exception, "Minimum size for reading HTTP header too small (redimension maxMtuSize in HTTPReceiveEntry)");
			}

			if ( buffer[cursor] == '\n'  )  
			{
				foundLN++;
			}
			else if ( buffer[cursor] == '\r' ) 
			{
				foundCR++;

			}
			else 
			{
				foundLN = 0;
				foundCR = 0;
			}
			if ( foundLN == 2 ) 
			{
				cursor += nBytes;
				dataOffset = cursor;
				break;
			}
		}
		cursor += nBytes;
	} while (nBytes > 0);
	
	if ( foundLN != 2 )  
	{		
		delete buffer;
		XCEPT_RAISE(pt::http::exception::Exception, "Connection closed by peer (wrong http header, empty line missing)");
	}	


	/* At this point the HTTP header is read
	   Now look how the message length is given, currently three options are
	   supported:
	   1) Content-Length -> the message size is provided as a number of bytes
	   2) Transfer-Encoding: chunked -> Multiple chunks ended with 0
	   3) none -> read until the connection is closed
	*/

	size_t len = 0;
	char keyword[15];
	unsigned int lengthEncoding = HTTP_HEADER_NONE; // by default read to end of connection
	
	// Look for Content-Length or Content-length only up to the position
	// 'cursor'. Otherwise we might find an old Content-Length somewhere
	// else in the memory that would lead to failure.
	//
	strcpy(keyword,"Content-Length");
	char*  offset = (char*) xoap::memSearch(&buffer[start], keyword, cursor , strlen(keyword));
	if ( offset == 0 ) // try also with capital L
	{
		strcpy(keyword,"Content-length");	
		offset = (char*) xoap::memSearch(&buffer[start], keyword, cursor, strlen(keyword) );
	}
	
	if (offset != 0)
	{
		lengthEncoding = HTTP_HEADER_CONTENT_LENGTH;
	}
	else
	{	
		strcpy(keyword,"Transfer-Encoding: chunked");
		char*  offset = (char*) xoap::memSearch(&buffer[start], keyword, cursor , strlen(keyword));
		if ( offset == 0 ) // try also with lower case 'encoding'
		{
			strcpy(keyword,"Transfer-encoding: chunked");	
			offset = (char*) xoap::memSearch(&buffer[start], keyword, cursor, strlen(keyword) );
		}
		
		if (offset != 0)
		{
			lengthEncoding = HTTP_HEADER_CHUNKED;
		}
	}
	
	//std::cout << "Buffer: " << buffer << std::endl << std::endl;
	// cout << "Offset: " << offset << endl << endl;
	
	*header = buffer;

	ssize_t toRead;
	
	// reset cursor when start reading the payload, restart from the buffer[0]
	// This means that the HTTP header will be overwritten and is no longer available.
	//
	cursor = 0; 
	available += dataOffset;
	
	// if  (offset != 0) 
	if (lengthEncoding == HTTP_HEADER_CONTENT_LENGTH)
	{
		// Using Content-length header, get the number of bytes encoded 
		// after the 'offset' variable that points to the header line
		char scanform[80];
		strcpy(scanform,keyword);
		strcat(scanform,": %d");
		if ( sscanf (offset, scanform,&len) > 0 ) 
		{			
			toRead = len;

			// Allocate more memory, if needed
			if (toRead > available) 
			{
				
				char * newBuffer = new char[toRead + cursor + 1];
				
				memcpy (newBuffer, buffer, cursor);
				
				delete buffer;
				
				buffer = newBuffer;
				available = toRead;
			}

			while (toRead > 0) {
				try
				{
					nBytes = in->receive(&buffer[cursor], toRead);
				} 
				catch (pt::http::exception::Exception& he)
				{
					delete buffer;
					std::stringstream msg;
					msg << "Failed to receive HTTP content after " << cursor << " out of " << toRead << " bytes";
					XCEPT_RETHROW (pt::http::exception::Exception, msg.str(), he);
				}
				available -= nBytes;

				if ( nBytes == 0 ) 
				{
					delete buffer;
					XCEPT_RAISE(pt::http::exception::Exception, "connection was closed by peer (request not completed)");

				}
				toRead -= nBytes;
				cursor += nBytes;
			}
			buffer[cursor] = '\0';
		}
	}
	else if (lengthEncoding == HTTP_HEADER_CHUNKED)
	{
		// Using Content-length header, get the number of bytes encoded 
		// after the 'offset' variable that points to the header line
		
		// Read the chunk length, encoded as hex, terminated with ';' or \r\n
		// Example: 1a; ignore-stuff-here
		
		// Read byte wise until '\n'
		try
		{
			ssize_t csize = -1; // chunk size, initialize to last chunk processed
			do
			{		
				char numBuf[255]; // buffer for holding the hex number
				size_t nbx = 0;
				while (nbx < 255)
				{
					in->receive(&numBuf[nbx], 1);
					
					if (numBuf[nbx] == '\n')
					{
						break; // found end of line
					}
					else if (numBuf[nbx] == '\r')
					{
						// eat it
					}
					else
					{
						++nbx; // advance
					}
				}

				if (nbx == 255)
				{
					// error: end of line, but length not properly ended
					delete buffer;
					XCEPT_RAISE (pt::http::exception::Exception, "Failed to decode chunk size");
				}

				numBuf[nbx] = '\0'; // replace last read character with '\0' end of string
				
				csize = http::Utils::atoi(numBuf, 16);

				// std::cout << "Chunk size [" << numBuf << "] as integer: " << csize <<std::endl;

				// Now read the chunk
				// Allocate more memory, if needed: size to read > available
				//
				if ( csize > available) 
				{				
					char * newBuffer = new char[cursor + csize + 1];				
					memcpy (newBuffer, buffer, cursor);				
					delete buffer;				
					buffer = newBuffer;
					available = csize;
				}

				// If the csize is 0, this was the last chunk
				if (csize == 0)
				{
					buffer[cursor] = '\0'; // end the HTTP content buffer properly

					// check if there's more in the buffer, but
					// throw it away: this version ignores optional footers
					char dump;
					do
					{
						nBytes = in->receive(&dump, 1);
					} while ((dump != '\n') && (nBytes != 0));
					
					// indicate that it was the last chunk
					csize = -1;
				}
				else
				{
					do 
					{
						nBytes = in->receive(&buffer[cursor], csize);
						available -= nBytes;

						if ( nBytes == 0 ) 
						{
							delete buffer;
							XCEPT_RAISE(pt::http::exception::Exception, "Connection closed by peer (request not completed)");
						}
						csize -= nBytes;
						cursor += nBytes;
					} while (csize > 0);
					
					// When chunk is read, it is ended with CRLF, need to eat those two characters, too
					char dump;
					do
					{
						in->receive(&dump, 1);
					} while (dump != '\n');
				}
				
			} while (csize != -1); // read until the last chunk (csize == -1) comes	
		} 
		catch (pt::http::exception::Exception& he)
		{
			delete buffer;
			XCEPT_RETHROW (pt::http::exception::Exception, "Failed to receive chunked HTTP data", he);
		}
	}
	else if (lengthEncoding == HTTP_HEADER_NONE)
	// if ( len == 0 ) 
	{ // no length specified then read till close connection
		len = available;

		do {
			try
			{
				nBytes = in->receive(&buffer[cursor], available);
			} 
			catch (pt::http::exception::Exception& he)
			{
				delete buffer;
				std::stringstream msg;
				msg << "Failed to receive HTTP content after " << cursor << " bytes in request without content length indication";
				XCEPT_RETHROW (pt::http::exception::Exception, msg.str(), he);
			}
			cursor += nBytes;
			available -= nBytes;

			if (available == 0) 
			{
				// Allocate more buffer memory if needed
				// Add one mtuSize_ each time.

				size_t toAllocate = ReadSegmentSize + cursor + 1;
				char * newBuffer = new char [toAllocate];
				memcpy (newBuffer, buffer, cursor);
				delete buffer;
				buffer = newBuffer;
				available = ReadSegmentSize;
			}
		} while ( nBytes > 0 );

		in->disconnect();
                buffer[cursor] = '\0';
	}

	*size = cursor;

	std::istringstream iss(*header);
	std::string method;
	int code;
	std::string message;
	iss >> method >> code >> message >> std::ws;

	if(code==200 || code==202) 
	{
		return buffer;
	}
	else
	{
		std::stringstream ss;
		ss << "HTTP error, code " << code << ", message size " << *size; 
		XCEPT_RAISE (pt::http::exception::Exception, ss.str());
	}
}

void pt::http::Utils::sendTo(pt::http::Channel * out, char * path, char * host, char* port, const char * buf, size_t len, char* headers)  
	throw (pt::http::exception::Exception)
{
			
	std::string boundary = extractMIMEBoundary(buf, len);

	if ( boundary == "" ) 
	{
		http::Utils::writeHttpPostHeader(out, path, host, port, len, headers);
	} else 
	{
		http::Utils::writeHttpPostMIMEHeader(out, path, host, port,  boundary, len, headers);

	}
		
	// send payload
	out->send(buf,len);

}


void pt::http::Utils::replyTo(pt::http::Channel * out,char * path, char * host, char* port, const char * buf, size_t len)  throw (pt::http::exception::Exception)
{
			
	std::string boundary = extractMIMEBoundary(buf, len);

	if ( boundary == "" ) 
	{
		http::Utils::writeHttpReplyHeader(out, path, host, port, len);
	} else 
	{
		http::Utils::writeHttpReplyMIMEHeader(out, path, host, port, boundary, len);

	}
	// send payload
	out->send(buf,len);

}

std::string pt::http::Utils::extractMIMEBoundary(const char * buf , size_t size) throw (pt::http::exception::Exception)
{
	char* boundary = 0;
	if ((buf[0] == '-') && (buf[1] == '-'))
        {
		boundary = (char*)xoap::memSearch(buf, "--", size, 2);
		
	}
	if (boundary != 0)
	{
		// move to start of boundary string
		boundary = &boundary[2]; 
		//
		// Find name of boundary string
		// Boundary string can be ended by "\n" or "\r\n"
		//
		char* boundaryStringEnd = (char*)xoap::memSearch(boundary, "\r\n", size, 2);
		if (boundaryStringEnd == 0)
		{
			boundaryStringEnd = (char*)xoap::memSearch(boundary, "\n",size, 1);
		}
		
		if (boundaryStringEnd == 0)
		{
                	XCEPT_RAISE (pt::http::exception::Exception, "Cannot find a boundary string in a MIME SOAP message");
        	}
		
		size_t boundaryStringSize = boundaryStringEnd - boundary;
		std::string boundaryString = std::string(boundary, boundaryStringSize);
		return boundaryString;
	}	
	return "";
	
}

void pt::http::Utils::writeHttpPostMIMEHeader(http::Channel * out, char * path, char * host, char* port,
std::string & boundaryStr, size_t len, char* headers) 
	throw (pt::http::exception::Exception)
{
	std::stringstream header;

	//
        // MIME content
	//
        header << "POST ";
        header << path; 
        header << " HTTP/1.1\r\n";
        header << "Host: ";
        header << host;
        header << ":";
	header << port;
        header << "\r\n";
        header << "Connection: keep-alive\r\n";
        header << "Content-type: multipart/related; type=\"application/soap+xml\"; boundary=\"";
        header << boundaryStr;
        header << "\"\r\n";
        header << "Content-Length: ";
        header << len;
	header << "\r\n";
	
	header << headers;
	
        // header += "\r\nSOAPAction: \"\"\r\n";
        header << "Accept: text/html, image/gif, image/jpeg, *; q=.2, */*; q=.2\r\n";
        header << "Content-Description: XDAQ SOAP with attachments.\r\n\r\n";

	out->send((char*)header.str().c_str(),header.str().size());
                        
}

void pt::http::Utils::writeHttpPostHeader(pt::http::Channel * out,char * path, char * host, char* port, size_t len, char* headers) 
	throw (pt::http::exception::Exception)
{
	//  
	// HTTP without MIME
	//
	std::stringstream header;
	header << "POST ";
	header << path;
	header << " HTTP/1.1\r\nHost: ";
	header << host;
	header << ":";
	header << port;
	header << "\r\nConnection: keep-alive\r\nContent-type: application/soap+xml; charset=utf-8\r\nContent-length: ";
	header << len;
	header << "\r\n";
	
	header << headers;
	header << "Content-Description: XDAQ SOAP.\r\n\r\n";
	out->send((char*)header.str().c_str(),header.str().size());
}

void pt::http::Utils::writeHttpReplyMIMEHeader(pt::http::Channel * out, char * path, char * host, char* port,
std::string & boundaryStr, size_t len) 
	throw (pt::http::exception::Exception)
{
	std::stringstream header;
	
	header << "HTTP/1.1 200 OK\r\n";
	header << "Host: ";
	header << host;
	header << ":";
	header << port;
	header << "\r\n";
	header << "Connection: keep-alive\r\n";
	header << "Content-Type: multipart/related; type=\"application/soap+xml\"; boundary=\"";
	header << boundaryStr;
	header << "\"\r\n";
	header << "Content-Length: ";
	header << len;
	header << "\r\nSOAPAction: \"\"\r\n";
	header << "Accept: text/html, image/gif, image/jpeg, *; q=.2, */*; q=.2\r\n";
	header << "Content-Description: Reply from XDAQ with attachments.";
	header << "\r\n\r\n";
	out->send((char*)header.str().c_str(),header.str().size());                 
}		

void pt::http::Utils::replyBodyTo(pt::http::Channel * out, const std::string& data, size_t len) 
	throw (pt::http::exception::Exception)
{
	out->send("\r\n",2);
	out->send((char*) data.c_str(),len);      
}

void pt::http::Utils::replyHeaderTo(pt::http::Channel * out, unsigned int code, size_t len) 
	throw (pt::http::exception::Exception)
{
	std::stringstream header;

	header << "HTTP/1.1 ";
	header << xgi::Utils::getResponsePhrase(code);
	header << "\r\n";
	header << "Date: ";
	header << toolbox::getDateTime();
	header << "\r\n";
	header << "Server: XDAQ/3.x\r\n";
	header << "Connection: close\r\n";
	
	//Content-Type: application/soap+xml; charset=\"utf-8\"\r\n";
	header << "Content-Length: ";
	header << len;
	header << "\r\n";
	out->send((char*)header.str().c_str(),header.str().size());
}

void pt::http::Utils::writeHttpReplyHeader(pt::http::Channel * out, char * path, char * host, char* port, size_t len) 
	throw (pt::http::exception::Exception)
{
	//  
	// HTTP without MIME
	//
	std::stringstream header;

	header << "HTTP/1.1 200 OK\r\nContent-Type: application/soap+xml; charset=\"utf-8\"\r\nContent-Length: ";
	header << len;
	header << "\r\n\r\n";
	out->send((char*)header.str().c_str(),header.str().size());
}


// Convert any string with radix 10 or 16 to an integer
ssize_t pt::http::Utils::atoi(char *str, unsigned int radix)
{
        ssize_t             c, val, negative;

        if (str == 0) 
	{
                return 0;
        }

        val = 0;
        if (radix == 10 && *str == '-') 
	{
                negative = 1;
                str++;
        } else 
	{
                negative = 0;
        }

        if (radix == 10) 
	{
                while (*str && isdigit(*str)) 
		{
                        val = (val * radix) + *str - '0';
                        str++;
                }
        } else if (radix == 16) 
	{
                if (*str == '0' && tolower(str[1]) == 'x') 
		{
                        str += 2;
                }
                while (*str) 
		{
                        c = tolower(*str);
                        if (isdigit(c)) 
			{
                                val = (val * radix) + c - '0';
                        } else if (c >= 'a' && c <= 'f') 
			{
                                val = (val * radix) + c - 'a' + 10;
                        } else 
			{
                                break;
                        }
                        str++;
                }
        }

        return (negative) ? -val: val;
}

