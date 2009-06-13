/*****************************************************************************\
* $Id: DDUDBAgent.h,v 1.3 2009/06/13 17:59:45 paste Exp $
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
			DDUDBAgent(xdaq::WebApplication *application)
			throw (emu::fed::exception::DBException);
			
			/** Build a bunch of DDUs corresponding to a crate ID **/
			std::map<xdata::UnsignedInteger64, emu::fed::DDU *, emu::fed::DBAgent::comp> getDDUs(xdata::UnsignedInteger64 &id)
			throw (emu::fed::exception::DBException);
			
			/** Build a bunch of DDU objects corresponding to a configuration key and an RUI **/
			std::map<xdata::UnsignedInteger64, emu::fed::DDU *, emu::fed::DBAgent::comp> getDDUs(xdata::UnsignedInteger64 &key, xdata::UnsignedShort &rui)
			throw (emu::fed::exception::DBException);

		private:
		
			/** Build the crates from the table returned **/
			std::map<xdata::UnsignedInteger64, emu::fed::DDU *, emu::fed::DBAgent::comp> buildDDUs(xdata::Table &table)
			throw (emu::fed::exception::DBException);

		};

	}
}

#endif






