/*****************************************************************************\
* $Id: SystemDBAgent.h,v 1.1 2009/05/16 18:55:20 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_SYSTEMDBAGENT_H__
#define __EMU_FED_SYSTEMDBAGENT_H__

#include "emu/fed/DBAgent.h"

#include <vector>
#include <string>

#include "emu/fed/Exception.h"

namespace emu {
	namespace fed {

		/** @class SystemDBAgent A utility class that will download a system configuration from the database and properly configure a vector of Crate objects based on that information.
		**/
		class SystemDBAgent: public DBAgent {
		
		public:

			/** Default constructor.
			**/
			SystemDBAgent(xdaq::WebApplication *application);

		private:

		};

	}
}

#endif






