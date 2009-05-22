/*****************************************************************************\
* $Id: DBAgent.h,v 1.3 2009/05/22 19:25:50 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_DBAGENT_H__
#define __EMU_FED_DBAGENT_H__

#include <string>
#include <map>

#include "xdaq/WebApplication.h"
#include "xoap/MessageReference.h"
#include "emu/fed/Exception.h"
#include "xdata/Table.h"
#include "xdata/UnsignedInteger64.h"

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
			
			/** Get all rows from the table this agent accesses. **/
			xdata::Table getAll()
			throw (emu::fed::exception::DBException);
			
			/** Get a particular row from the table given that row's ID. **/
			xdata::Table getByID(xdata::UnsignedInteger64 id)
			throw (emu::fed::exception::DBException);
			
			/** Get all matching rows from the table given a configuration key. **/
			xdata::Table getByKey(xdata::UnsignedInteger64 key)
			throw (emu::fed::exception::DBException);

		protected:
		
			/** Required to make maps with xdata::UnsignedInteger64 keys. 
			*
			*	@note Seriously?  Who is responsibile for defining special comparison operators without defining const versions of the same?  Good thing I'm a c++ ninja, or this would end me.
			**/
			struct comp {
				bool operator() (const xdata::UnsignedInteger64 &lhs, const xdata::UnsignedInteger64 &rhs) const { return *(const_cast<xdata::UnsignedInteger64 *>(&lhs)) < *(const_cast<xdata::UnsignedInteger64 *>(&rhs)); }
			};
			
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
			
			/// The name of the table this agent accesses
			std::string table_;

		private:
		
		};

	}
}

#endif

