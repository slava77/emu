/*****************************************************************************\
* $Id: VMEModuleDBAgent.h,v 1.1 2009/05/16 18:55:20 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_VMEMODULEDBAGENT_H__
#define __EMU_FED_VMEMODULEDBAGENT_H__

#include "emu/fed/DBAgent.h"

#include <vector>
#include <string>

#include "emu/fed/Exception.h"

namespace emu {
	namespace fed {

		/** @class VMEModuleDBAgent A utility class that will download a Controller configuration from the database and properly configure a VMEModule object based on that information.
		**/
		class VMEModuleDBAgent: public DBAgent {
		
		public:

			/** Default constructor.
			**/
			VMEModuleDBAgent(xdaq::WebApplication *application);

		private:

		};

	}
}

#endif






