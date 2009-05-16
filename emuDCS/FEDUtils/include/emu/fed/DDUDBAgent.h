/*****************************************************************************\
* $Id: DDUDBAgent.h,v 1.1 2009/05/16 18:55:20 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_DDUDBAGENT_H__
#define __EMU_FED_DDUDBAGENT_H__

#include "emu/fed/VMEModuleDBAgent.h"

#include <vector>
#include <string>

#include "emu/fed/Exception.h"

namespace emu {
	namespace fed {

		/** @class DDUDBAgent A utility class that will download a Controller configuration from the database and properly configure a DDU object based on that information.
		**/
		class DDUDBAgent: public VMEModuleDBAgent {
		
		public:

			/** Default constructor.
			**/
			DDUDBAgent(xdaq::WebApplication *application);

		private:

		};

	}
}

#endif






