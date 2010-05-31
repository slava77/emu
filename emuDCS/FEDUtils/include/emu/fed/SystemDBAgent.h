/*****************************************************************************\
* $Id: SystemDBAgent.h,v 1.10 2010/05/31 14:05:18 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_SYSTEMDBAGENT_H__
#define __EMU_FED_SYSTEMDBAGENT_H__

#include "emu/fed/DBAgent.h"
#include <utility>
#include <time.h>
#include <vector>
#include <map>
#include <string>

#include "xdata/String.h"

namespace emu {
	namespace fed {

		/** @class SystemDBAgent A utility class that will download a system configuration from the database.
		**/
		class SystemDBAgent: public DBAgent {
		
		public:

			/** Default constructor. **/
			SystemDBAgent(xdaq::WebApplication *application, const int &instance = -1);
			
			/** Get the system ID and name corresponding to a configuration key **/
			std::pair<std::string, time_t> getSystem(xdata::UnsignedInteger64 &key)
			throw (emu::fed::exception::DBException);
			
			/** Get all the System names/IDs in the DB **/
			std::map<std::string, std::vector<std::pair<xdata::UnsignedInteger64, time_t> > > getAllKeys()
			throw (emu::fed::exception::DBException);
			
			/** Upload the system **/
			void upload(xdata::UnsignedInteger64 &key, xdata::String &name)
			throw (emu::fed::exception::DBException);

		private:
		
			/** Build a system from the table returned **/
			std::pair<std::string, time_t> buildSystem(xdata::Table &table)
			throw (emu::fed::exception::DBException);
			
			/** Sort a vector of pairs by timestamp **/
			struct TimeSort {
				bool operator() (std::pair<xdata::UnsignedInteger64, time_t> lhs, std::pair<xdata::UnsignedInteger64, time_t> rhs) {
					// Standards compliance
					return difftime(lhs.second, rhs.second) > 0;
				}
			} sorter_;

		};

	}
}

#endif






