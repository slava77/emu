/*****************************************************************************\
* $Id: CrateDBAgent.h,v 1.5 2009/11/09 11:46:32 paste Exp $
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
			CrateDBAgent(xdaq::WebApplication *application);
			
			/** Build a bunch of crate objects corresponding to a system ID **/
			std::vector<emu::fed::Crate *> getCrates(xdata::UnsignedInteger64 &id)
			throw (emu::fed::exception::DBException);
			
			/** Build a bunch of crate objects corresponding to a configuration key and a crate number **/
			std::vector<emu::fed::Crate *> getCrates(xdata::UnsignedInteger64 &key, xdata::UnsignedShort &number)
			throw (emu::fed::exception::DBException);

		private:
		
			/** Build the crates from the table returned **/
			std::vector<emu::fed::Crate *> buildCrates(xdata::Table &table,xdata::UnsignedInteger64 &key)
			throw (emu::fed::exception::DBException);

		};

	}
}

#endif






