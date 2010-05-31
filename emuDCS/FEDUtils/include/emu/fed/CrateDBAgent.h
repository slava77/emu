/*****************************************************************************\
* $Id: CrateDBAgent.h,v 1.8 2010/05/31 14:05:18 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_CRATEDBAGENT_H__
#define __EMU_FED_CRATEDBAGENT_H__

#include "emu/fed/DBAgent.h"

#include <vector>
#include <string>

#include "emu/fed/Exception.h"
#include "xdata/UnsignedShort.h"

namespace emu {
	namespace fed {
	
		class Crate;

		/** @class CrateDBAgent A utility class that will download a crate configuration from the database and properly configure a Crate object based on that information.
		**/
		class CrateDBAgent: public DBAgent {
		
		public:

			/** Default constructor **/
			CrateDBAgent(xdaq::WebApplication *application, const int &instance = -1);
			
			/** Build a bunch of crate objects corresponding to a system ID **/
			std::vector<emu::fed::Crate *> getCrates(xdata::UnsignedInteger64 &id)
			throw (emu::fed::exception::DBException);
			
			/** Upload a set of crates **/
			void upload(xdata::UnsignedInteger64 &key, const std::vector<Crate *> &crateVector)
			throw (emu::fed::exception::DBException);

		private:
		
			/** Build the crates from the table returned **/
			std::vector<emu::fed::Crate *> buildCrates(xdata::Table &table,xdata::UnsignedInteger64 &key)
			throw (emu::fed::exception::DBException);

		};

	}
}

#endif






