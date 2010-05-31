/*****************************************************************************\
* $Id: DDUFMMIRQFact.h,v 1.1 2010/05/31 14:57:19 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_DDUFMMIRQFACT_H__
#define __EMU_FED_DDUFMMIRQFACT_H__

#include "emu/fed/Fact.h"

namespace emu {
	namespace fed {

		/// FMM IRQ information
		class DDUFMMIRQFact {
		public:
			enum ParameterName_t {crateNumber, slotNumber, rui, hardError, syncError, resetRequested, fiberStatus, advancedFiberStatus, xorStatus, ddufpgaDebugTrap, infpga0DebugTrap, infpga1DebugTrap, nParameters};
			static const char* getTypeName() { return typeName_; }
			static const std::string getParameterName(const ParameterName_t p) { return parameterNames_[p]; }
		protected:
			static const char* const typeName_;
			static const char* const parameterNames_[nParameters];
		};

	}
}

#endif
