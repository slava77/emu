/*****************************************************************************\
* $Id: DCCDBAgent.h,v 1.1 2009/05/16 18:55:20 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_DCCDBAGENT_H__
#define __EMU_FED_DCCDBAGENT_H__

#include "emu/fed/VMEModuleDBAgent.h"

#include <vector>
#include <string>

#include "emu/fed/Exception.h"

namespace emu {
	namespace fed {

		/** @class DCCDBAgent A utility class that will download a Controller configuration from the database and properly configure a DCC object based on that information.
		**/
		class DCCDBAgent: public VMEModuleDBAgent {
		
		public:

			/** Default constructor.
			**/
			DCCDBAgent(xdaq::WebApplication *application);

		private:

		};

	}
}

#endif






