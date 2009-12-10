/*****************************************************************************\
* $Id: VMEControllerDBAgent.h,v 1.6 2009/12/10 16:30:04 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_VMECONTROLLERDBAGENT_H__
#define __EMU_FED_VMECONTROLLERDBAGENT_H__

#include "emu/fed/DBAgent.h"

#include <vector>
#include <string>

#include "emu/fed/Exception.h"
#include "xdata/UnsignedShort.h"
#include "xdata/TableIterator.h"

namespace emu {
	namespace fed {
		
		class VMEController;

		/** @class VMEControllerDBAgent A utility class that will download a Controller configuration from the database and properly configure a VMEController object based on that information.
		**/
		class VMEControllerDBAgent: public DBAgent {
		
		public:

			/** Default constructor.
			**/
			VMEControllerDBAgent(xdaq::WebApplication *application);
			
			/** Build a controller corresponding to a configuration key and a crate number **/
			emu::fed::VMEController *getController(xdata::UnsignedInteger64 &key, xdata::UnsignedShort &number, const bool &fake = false)
			throw (emu::fed::exception::DBException);
			
			/** Upload a controller **/
			void upload(xdata::UnsignedInteger64 &key, xdata::UnsignedShort &crateNumber, VMEController *controller)
			throw (emu::fed::exception::DBException);

		private:
		
			/** Build the controller from the table returned **/
			emu::fed::VMEController *buildController(xdata::Table &table, const bool &fake = false)
			throw (emu::fed::exception::DBException);
			
		};

	}
}

#endif






