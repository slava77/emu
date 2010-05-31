/*****************************************************************************\
* $Id: FiberDBAgent.h,v 1.7 2010/05/31 14:05:18 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_FIBERDBAGENT_H__
#define __EMU_FED_FIBERDBAGENT_H__

#include "emu/fed/DBAgent.h"

#include <vector>
#include "xdata/UnsignedShort.h"

#include "emu/fed/Exception.h"

namespace emu {
	namespace fed {
	
		class Fiber;

		/** @class FiberDBAgent A utility class that will download a Controller configuration from the database and properly configure DDU fibers based on that information.
		**/
		class FiberDBAgent: public DBAgent {
		
		public:

			/** Default constructor **/
			FiberDBAgent(xdaq::WebApplication *application, const int &instance = -1);
			
			/** Build a bunch of Fiber objects corresponding to a configuration key and an RUI **/
			std::vector<emu::fed::Fiber *> getFibers(xdata::UnsignedInteger64 &key, xdata::UnsignedShort &rui)
			throw (emu::fed::exception::DBException);
			
			/** Upload a set of fibers **/
			void upload(xdata::UnsignedInteger64 &key, xdata::UnsignedShort &rui, const std::vector<Fiber *> &fiberVector)
			throw (emu::fed::exception::DBException);

		private:
		
			/** Build the crates from the table returned **/
			std::vector<emu::fed::Fiber *> buildFibers(xdata::Table &table)
			throw (emu::fed::exception::DBException);

		};

	}
}

#endif






