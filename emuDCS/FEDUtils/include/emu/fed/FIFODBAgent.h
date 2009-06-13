/*****************************************************************************\
* $Id: FIFODBAgent.h,v 1.3 2009/06/13 17:59:45 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_FIFODBAGENT_H__
#define __EMU_FED_FIFODBAGENT_H__

#include "emu/fed/DBAgent.h"

#include <vector>

#include "emu/fed/Exception.h"
#include "xdata/UnsignedShort.h"

namespace emu {
	namespace fed {
	
		class FIFO;

		/** @class FIFODBAgent A utility class that will download a FIFO configuration from the database and properly configure DCC fifos based on that information.
		**/
		class FIFODBAgent: public DBAgent {
		
		public:

			/** Default constructor **/
			FIFODBAgent(xdaq::WebApplication *application)
			throw (emu::fed::exception::DBException);
			
			/** Build a bunch of FIFOs corresponding to a DCC ID **/
			std::vector<emu::fed::FIFO *> getFIFOs(xdata::UnsignedInteger64 &id)
			throw (emu::fed::exception::DBException);
			
			/** Build a bunch of FIFO objects corresponding to a configuration key and a DCC FMM ID **/
			std::vector<emu::fed::FIFO *> getFIFOs(xdata::UnsignedInteger64 &key, xdata::UnsignedShort &fmm_id)
			throw (emu::fed::exception::DBException);
			
			/** Build a bunch of FIFO objects corresponding to a configuration key and a DCC FMM ID and a FIFO number **/
			std::vector<emu::fed::FIFO *> getFIFOs(xdata::UnsignedInteger64 &key, xdata::UnsignedShort &fmm_id, xdata::UnsignedShort &number)
			throw (emu::fed::exception::DBException);

		private:
		
			/** Build the crates from the table returned **/
			std::vector<emu::fed::FIFO *> buildFIFOs(xdata::Table &table)
			throw (emu::fed::exception::DBException);

		};

	}
}

#endif






