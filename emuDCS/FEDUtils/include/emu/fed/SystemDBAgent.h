/*****************************************************************************\
* $Id: SystemDBAgent.h,v 1.5 2009/11/13 09:03:11 paste Exp $
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
			SystemDBAgent(xdaq::WebApplication *application);
			
			/** Get the system ID and name corresponding to a configuration key **/
			std::string getSystem(xdata::UnsignedInteger64 &key)
			throw (emu::fed::exception::DBException);

		private:
		
			/** Build a system from the table returned **/
			std::string buildSystem(xdata::Table &table)
			throw (emu::fed::exception::DBException);

		};

	}
}

#endif






