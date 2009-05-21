/*****************************************************************************\
* $Id: FIFO.h,v 1.1 2009/05/21 15:33:43 paste Exp $
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

			/** Constructor used to set the RUI **/
			FIFO(unsigned int rui = 0);

			/** Return the DDU RUI of the given FIFO **/
			inline unsigned int getRUI() { return rui_; }

		private:
		
			/// The RUI of the DDU that this FIFO governs
			unsigned int rui_;

		};

	}
}

#endif
