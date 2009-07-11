/*****************************************************************************\
* $Id: DBConfigurator.h,v 1.3 2009/07/11 19:38:32 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_DBCONFIGURATOR_H__
#define __EMU_FED_DBCONFIGURATOR_H__

#include "emu/fed/Configurator.h"

#include <string>
#include "xdaq/WebApplication.h"
#include "xdata/UnsignedInteger64.h"

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
			DBConfigurator(xdaq::WebApplication *application, const std::string &username, const std::string &password, xdata::UnsignedInteger64 &key);

			/** Configure the crates and return them.
			*
			*	@param key The database key to use when configuring.
			**/
			std::vector<emu::fed::Crate *> setupCrates()
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
		
		};

	}
}

#endif

