/*****************************************************************************\
* $Id: FIFODBAgent.h,v 1.1 2009/05/16 18:55:20 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_FIFODBAGENT_H__
#define __EMU_FED_FIFODBAGENT_H__

#include "emu/fed/DBAgent.h"

#include <vector>
#include <string>

#include "emu/fed/Exception.h"

namespace emu {
	namespace fed {

		/** @class FIFODBAgent A utility class that will download a Controller configuration from the database and properly configure a DCC fifos based on that information.
		**/
		class FIFODBAgent: public DBAgent {
		
		public:

			/** Default constructor.
			**/
			FIFODBAgent(xdaq::WebApplication *application);

		private:

		};

	}
}

#endif






