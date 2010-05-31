/*****************************************************************************\
* $Id: DCCDBAgent.h,v 1.8 2010/05/31 14:05:18 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_DCCDBAGENT_H__
#define __EMU_FED_DCCDBAGENT_H__

#include "emu/fed/DBAgent.h"

#include <vector>
#include <string>

#include "emu/fed/Exception.h"
#include "xdata/UnsignedShort.h"

namespace emu {
	namespace fed {
	
		class DCC;

		/** @class DCCDBAgent A utility class that will download a Controller configuration from the database and properly configure a DCC object based on that information.
		**/
		class DCCDBAgent: public DBAgent {
		
		public:

			/** Default constructor **/
			DCCDBAgent(xdaq::WebApplication *application, const int &instance = -1);
			
			/** Build a bunch of DCCs corresponding to a crate ID **/
			std::vector<emu::fed::DCC *> getDCCs(xdata::UnsignedInteger64 &key, xdata::UnsignedShort &crateNumber, const bool &fake = false)
			throw (emu::fed::exception::DBException);
			
			/** Upload a set of DCCs **/
			void upload(xdata::UnsignedInteger64 &key, xdata::UnsignedShort &crateNumber, const std::vector<DCC *> &dccVector)
			throw (emu::fed::exception::DBException);

		private:
		
			/** Build the crates from the table returned **/
			std::vector<emu::fed::DCC *> buildDCCs(xdata::Table &table, const bool &fake = false)
			throw (emu::fed::exception::DBException);

		};

	}
}

#endif






