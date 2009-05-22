/*****************************************************************************\
* $Id: DBAgent.h,v 1.2 2009/05/22 11:25:25 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_DBAGENT_H__
#define __EMU_FED_DBAGENT_H__

#include <string>
#include <map>

#include "xdaq/WebApplication.h"
#include "xoap/MessageReference.h"
#include "emu/fed/Exception.h"
#include "xdata/Table.h"

namespace emu {
	namespace fed {

		/** @class DBAgent A utility class that allows for easy communication with a TStore database **/
		class DBAgent
		{
		public:

			/** Default constructor.
			**/
			DBAgent(xdaq::WebApplication *application)
			throw (emu::fed::exception::DBException);
			
			/** Connect to the database and store the connectionID for later use.
			*	@param username the username to use for connecting to the database.
			*	@param password the password to use for connecting to the database.
			**/
			void connect(const std::string &username, const std::string &password)
			throw (emu::fed::exception::DBException);
			
			/** Disconnect from the database **/
			void disconnect()
			throw (emu::fed::exception::DBException);
			
			/** Query the database and return a response.
			*	@param queryViewName The name of the query operation from the TStore configuration
			*	@param queryParameters A collection of string:string pairs of parameter names and values for the query
			*
			*	@returns a Table representing the data requested from the database.
			**/
			xdata::Table query(const std::string &queryViewName, const std::map<std::string, std::string> &queryParameters)
			throw (emu::fed::exception::DBException);
			
			/** Insert into the database new rows.
			*	@param insertViewName The name of the insert operation from the TStore configuration
			*	@param newRows A collection of new rows to insert in Table format (can insert multiple rows simultaneously)
			**/
			void insert(const std::string &insertViewName, xdata::Table &newRows)
			throw (emu::fed::exception::DBException);

		protected:
			
			/** Send a SOAP message to the given application **/
			xoap::MessageReference sendSOAPMessage(xoap::MessageReference message, std::string klass, int instance = -1)
			throw (emu::fed::exception::SOAPException);

			/** Send a SOAP message to the given application **/
			xoap::MessageReference sendSOAPMessage(xoap::MessageReference message, xdaq::ApplicationDescriptor *app)
			throw (emu::fed::exception::SOAPException);
			
			/// The application context from which to send the SOAP messages
			xdaq::WebApplication *application_;

			/// The ID recieved from the database after connecting for future requests
			std::string connectionID_;

		private:
		
		};

	}
}

#endif

