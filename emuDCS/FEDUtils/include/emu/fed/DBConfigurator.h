/*****************************************************************************\
* $Id: DBConfigurator.h,v 1.2 2009/06/13 17:59:45 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_DBCONFIGURATOR_H__
#define __EMU_FED_DBCONFIGURATOR_H__

#include "emu/fed/Configurator.h"

#include <string>
#include "xdaq/WebApplication.h"

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
			DBConfigurator(xdaq::WebApplication *application, const std::string &username, const std::string &password);

			/** Configure the crates and return them **/
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
		
		};

	}
}

#endif

