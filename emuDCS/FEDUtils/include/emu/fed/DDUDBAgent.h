/*****************************************************************************\
* $Id: DDUDBAgent.h,v 1.7 2010/05/31 14:05:18 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_DDUDBAGENT_H__
#define __EMU_FED_DDUDBAGENT_H__

#include "emu/fed/DBAgent.h"

#include <vector>
#include <string>

#include "emu/fed/Exception.h"
#include "xdata/UnsignedShort.h"

namespace emu {
	namespace fed {
	
		class DDU;

		/** @class DDUDBAgent A utility class that will download a Controller configuration from the database and properly configure a DDU object based on that information.
		**/
		class DDUDBAgent: public DBAgent {
		
		public:

			/** Default constructor **/
			DDUDBAgent(xdaq::WebApplication *application, const int &instance = -1);
			
			/** Build a bunch of DDU objects corresponding to a configuration key and a crate number **/
			std::vector<emu::fed::DDU *> getDDUs(xdata::UnsignedInteger64 &key, xdata::UnsignedShort &crateNumber, const bool &fake = false)
			throw (emu::fed::exception::DBException);
			
			/** Upload a set of DDUs **/
			void upload(xdata::UnsignedInteger64 &key, xdata::UnsignedShort &crateNumber, const std::vector<DDU *> &dduVector)
			throw (emu::fed::exception::DBException);

		private:
		
			/** Build the crates from the table returned **/
			std::vector<emu::fed::DDU *> buildDDUs(xdata::Table &table, const bool &fake = false)
			throw (emu::fed::exception::DBException);

		};

	}
}

#endif






