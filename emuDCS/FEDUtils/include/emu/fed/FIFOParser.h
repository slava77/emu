/*****************************************************************************\
* $Id: FIFOParser.h,v 1.3 2009/06/13 17:59:45 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_FIFOPARSER_H__
#define __EMU_FED_FIFOPARSER_H__

#include "emu/fed/Parser.h"
#include "emu/fed/Exception.h"

namespace emu {
	namespace fed {

		class FIFO;

		/** @class FIFOParser A parser that builds FIFO objects to be loaded into the DCC.
		*	@sa FIFO
		**/
		class FIFOParser: public Parser
		{
		
		public:

			/** Default constructor.
			*
			*	@param pNode the XML DOM element node to parse.
			**/
			explicit FIFOParser(xercesc::DOMElement *pNode)
			throw (emu::fed::exception::ParseException);
			
			/** Default destructor **/
			~FIFOParser();

			/** @returns a pointer to the parsed FIFO object. **/
			inline FIFO *getFIFO() { return fifo_; }
		
		private:

			/// A FIFO object built from the parsed attributes of the DOM node.
			FIFO *fifo_;
		};

	}
}

#endif
