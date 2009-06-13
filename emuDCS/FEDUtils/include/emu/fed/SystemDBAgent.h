/*****************************************************************************\
* $Id: SystemDBAgent.h,v 1.3 2009/06/13 17:59:45 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_SYSTEMDBAGENT_H__
#define __EMU_FED_SYSTEMDBAGENT_H__

#include "emu/fed/DBAgent.h"

namespace emu {
	namespace fed {

		/** @class SystemDBAgent A utility class that will download a system configuration from the database and properly configure a vector of Crate objects based on that information.
		**/
		class SystemDBAgent: public DBAgent {
		
		public:

			/** Default constructor. **/
			SystemDBAgent(xdaq::WebApplication *application)
			throw (emu::fed::exception::DBException);
			
			/** Get the system ID and name corresponding to a hostname and a configuration key **/
			std::pair<xdata::UnsignedInteger64, std::string> getSystem(const std::string &hostname, xdata::UnsignedInteger64 &key)
			throw (emu::fed::exception::DBException);
			
			/** Get the latest system ID and name from the database given a hostname **/
			std::pair<xdata::UnsignedInteger64, std::string> getSystem(const std::string &hostname)
			throw (emu::fed::exception::DBException);

		private:
		
			/** Build a system from the table returned **/
			std::pair<xdata::UnsignedInteger64, std::string> buildSystem(xdata::Table &table)
			throw (emu::fed::exception::DBException);

		};

	}
}

#endif






