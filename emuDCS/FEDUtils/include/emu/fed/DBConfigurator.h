/*****************************************************************************\
* $Id: DBConfigurator.h,v 1.7 2012/01/30 17:06:55 cvuosalo Exp $
\*****************************************************************************/
#ifndef __EMU_FED_DBCONFIGURATOR_H__
#define __EMU_FED_DBCONFIGURATOR_H__

#include "emu/fed/Configurator.h"

#include <string>
#include "xdaq/WebApplication.h"
#include "xdata/UnsignedInteger64.h"
#include "xdata/Integer.h"

namespace emu {
	namespace fed {

		/** @class DBConfigurator A utility class that produces a vector of Crates based on information read from the online database. **/
		class DBConfigurator: public Configurator
		{
		public:
			
			/** Constructor.
			*
			*	@param application The web application doing the configuring.  Required because the class needs to perform SOAP communication in order to configure.
			*	@param username The database username.
			*	@param password The database password.
			**/
			DBConfigurator(xdaq::WebApplication *application, const std::string &username, const std::string &password, xdata::UnsignedInteger64 &key, xdata::Integer &instance);
			
			/** Destructor **/
			virtual ~DBConfigurator() {};

			/** Configure the crates and return them. **/
			virtual std::vector<emu::fed::Crate *> setupCrates(const bool &fake = false)
			throw (emu::fed::exception::ConfigurationException);
			
			/** Upload system configuration to database **/
			void uploadToDB(const std::vector<emu::fed::Crate *> &crateVector, const std::string &systemName)
			throw (emu::fed::exception::ConfigurationException);

		protected:

		private:
			
			/// The application from where to send SOAP messages for communication
			xdaq::WebApplication *application_;
			
			/// The database username
			std::string dbUsername_;
			
			/// The database password
			std::string dbPassword_;
			
			/// The database key to use
			xdata::UnsignedInteger64 dbKey_;
			
			/// The TStore instance to use
			xdata::Integer instance_;
		
		};

	}
}

#endif

