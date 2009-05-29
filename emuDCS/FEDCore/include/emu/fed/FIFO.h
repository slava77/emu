/*****************************************************************************\
* $Id: FIFO.h,v 1.2 2009/05/29 11:23:18 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_FIFO_H__
#define __EMU_FED_FIFO_H__

#include <string>

namespace emu {
	namespace fed {

		/** @class FIFO A class for easy access to DCC FIFOs. **/
		class FIFO
		{
		public:

			friend class FIFOParser;
			friend class DCC;

			/** Constructor used to set the RUI **/
			FIFO(unsigned int rui = 0, bool used = true);

			/** Return the DDU RUI of the FIFO **/
			inline unsigned int getRUI() { return rui_; }
			
			/** Returns whether or not the owning DCC should be configured to use this FIFO **/
			inline bool isUsed() { return used_; }

		private:
		
			/// The RUI of the DDU that this FIFO governs
			unsigned int rui_;
			
			/// Whether or not this FIFO is in use
			bool used_;

		};

	}
}

#endif
