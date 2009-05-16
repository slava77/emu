/*****************************************************************************\
* $Id: VMEControllerDBAgent.h,v 1.1 2009/05/16 18:55:20 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_VMECONTROLLERDBAGENT_H__
#define __EMU_FED_VMECONTROLLERDBAGENT_H__

#include "emu/fed/DBAgent.h"

#include <vector>
#include <string>

#include "emu/fed/Exception.h"

namespace emu {
	namespace fed {

		/** @class VMEControllerDBAgent A utility class that will download a Controller configuration from the database and properly configure a VMEController object based on that information.
		**/
		class VMEControllerDBAgent: public DBAgent {
		
		public:

			/** Default constructor.
			**/
			VMEControllerDBAgent(xdaq::WebApplication *application);

		private:

		};

	}
}

#endif






