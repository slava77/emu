/*****************************************************************************\
* $Id: FiberParser.h,v 1.3 2009/06/13 17:59:45 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_FIBERPARSER_H__
#define __EMU_FED_FIBERPARSER_H__

#include "emu/fed/Parser.h"
#include "emu/fed/Exception.h"

namespace emu {
	namespace fed {

		class Fiber;

		/** @class FiberParser A parser that builds Fiber objects to be loaded into the DDU.
		*	@sa Fiber
		**/
		class FiberParser: public Parser
		{
		
		public:

			/** Default constructor.
			*
			*	@param pNode the XML DOM element node to parse.
			**/
			explicit FiberParser(xercesc::DOMElement *pNode)
			throw (emu::fed::exception::ParseException);
			
			/** Default destructor **/
			~FiberParser();

			/** @returns a pointer to the parsed Fiber object. **/
			inline Fiber *getFiber() { return fiber_; }
		
		private:

			/// A Fiber object built from the parsed attributes of the DOM node.
			Fiber *fiber_;

		};

	}
}

#endif
