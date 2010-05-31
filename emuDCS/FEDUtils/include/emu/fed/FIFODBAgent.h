/*****************************************************************************\
* $Id: FIFODBAgent.h,v 1.8 2010/05/31 14:05:18 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_FIFODBAGENT_H__
#define __EMU_FED_FIFODBAGENT_H__

#include "emu/fed/DBAgent.h"

#include <vector>

#include "emu/fed/Exception.h"
#include "xdata/UnsignedInteger.h"

namespace emu {
	namespace fed {
	
		class FIFO;

		/** @class FIFODBAgent A utility class that will download a FIFO configuration from the database and properly configure DCC fifos based on that information.
		**/
		class FIFODBAgent: public DBAgent {
		
		public:

			/** Default constructor **/
			FIFODBAgent(xdaq::WebApplication *application, const int &instance = -1);

			/** Build a bunch of FIFO objects corresponding to a configuration key and a DCC FMM ID **/
			std::vector<emu::fed::FIFO *> getFIFOs(xdata::UnsignedInteger64 &key, xdata::UnsignedInteger &fmm_id)
			throw (emu::fed::exception::DBException);
			
			/** Upload a set of FIFOs **/
			void upload(xdata::UnsignedInteger64 &key, xdata::UnsignedInteger &fmmid, const std::vector<FIFO *> &fifoVector)
			throw (emu::fed::exception::DBException);

		private:
		
			/** Build the crates from the table returned **/
			std::vector<emu::fed::FIFO *> buildFIFOs(xdata::Table &table)
			throw (emu::fed::exception::DBException);

		};

	}
}

#endif






