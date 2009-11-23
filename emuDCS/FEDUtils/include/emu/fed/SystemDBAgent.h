/*****************************************************************************\
* $Id: SystemDBAgent.h,v 1.7 2009/11/23 07:43:59 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_SYSTEMDBAGENT_H__
#define __EMU_FED_SYSTEMDBAGENT_H__

#include "emu/fed/DBAgent.h"
#include <utility>
#include <time.h>
#include <vector>
#include <map>
#include <string>

#include <map>
#include <time.h>

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
			
			/** Get all the System names/IDs in the DB **/
			std::map<std::string, std::vector<std::pair<xdata::UnsignedInteger64, time_t> > > getAllKeys()
			throw (emu::fed::exception::DBException);

		private:
		
			/** Build a system from the table returned **/
			std::pair<std::string, time_t> buildSystem(xdata::Table &table)
			throw (emu::fed::exception::DBException);

		};

	}
}

#endif






