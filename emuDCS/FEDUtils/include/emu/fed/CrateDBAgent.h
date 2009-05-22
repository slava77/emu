/*****************************************************************************\
* $Id: CrateDBAgent.h,v 1.2 2009/05/22 19:25:50 paste Exp $
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
			CrateDBAgent(xdaq::WebApplication *application)
			throw (emu::fed::exception::DBException);
			
			/** Build a bunch of crate objects corresponding to a system ID **/
			std::map<xdata::UnsignedInteger64, emu::fed::Crate *, emu::fed::DBAgent::comp> getCrates(xdata::UnsignedInteger64 id)
			throw (emu::fed::exception::DBException);
			
			/** Build a bunch of crate objects corresponding to a configuration key and a crate number **/
			std::map<xdata::UnsignedInteger64, emu::fed::Crate *, emu::fed::DBAgent::comp> getCrates(xdata::UnsignedInteger64 key, xdata::UnsignedShort number)
			throw (emu::fed::exception::DBException);

		private:
		
			/** Build the crates from the table returned **/
			std::map<xdata::UnsignedInteger64, emu::fed::Crate *, emu::fed::DBAgent::comp> buildCrates(xdata::Table table)
			throw (emu::fed::exception::DBException);

		};

	}
}

#endif






