/*****************************************************************************\
* $Id: CrateDBAgent.h,v 1.1 2009/05/16 18:55:20 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_CRATEDBAGENT_H__
#define __EMU_FED_CRATEDBAGENT_H__

#include "emu/fed/DBAgent.h"

#include <vector>
#include <string>

#include "emu/fed/Exception.h"

namespace emu {
	namespace fed {

		/** @class CrateDBAgent A utility class that will download a crate configuration from the database and properly configure a Crate object based on that information.
		**/
		class CrateDBAgent: public DBAgent {
		
		public:

			/** Default constructor.
			**/
			CrateDBAgent(xdaq::WebApplication *application);

		private:

		};

	}
}

#endif






