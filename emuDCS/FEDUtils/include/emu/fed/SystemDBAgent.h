/*****************************************************************************\
* $Id: SystemDBAgent.h,v 1.6 2009/11/22 22:45:10 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_SYSTEMDBAGENT_H__
#define __EMU_FED_SYSTEMDBAGENT_H__

#include "emu/fed/DBAgent.h"
#include <utility>
#include <time.h>
#include <vector>
#include <map>
#include <string>

namespace emu {
	namespace fed {

		/** @class SystemDBAgent A utility class that will download a system configuration from the database.
		**/
		class SystemDBAgent: public DBAgent {
		
		public:

			/** Default constructor. **/
			SystemDBAgent(xdaq::WebApplication *application);
			
			/** Get the system ID and name corresponding to a configuration key **/
			std::pair<std::string, time_t> getSystem(xdata::UnsignedInteger64 &key)
			throw (emu::fed::exception::DBException);
			
			/** Get all the system IDs corresponding to the given system name, or all the system names if the parameter is omitted **/
			std::map<std::string, std::vector<xdata::UnsignedInteger64> > getAllKeys(const std::string &system = "")
			throw (emu::fed::exception::DBException);

		private:
		
			/** Build a system from the table returned **/
			std::pair<std::string, time_t> buildSystem(xdata::Table &table)
			throw (emu::fed::exception::DBException);

		};

	}
}

#endif






