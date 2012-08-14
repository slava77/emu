/*****************************************************************************\
* $Id: DduFmmErrorFact.h,v 1.1 2012/08/14 11:38:10 cvuosalo Exp $
\*****************************************************************************/
#ifndef __EMU_FED_DDUFMMERRORFACT_H__
#define __EMU_FED_DDUFMMERRORFACT_H__

#include "emu/fed/Fact.h"

namespace emu {
	namespace fed {

		/// FMM IRQ information
		class DDUFMMErrorFact {
		public:
			enum ParameterName_t {hardResetRequested,
				resyncRequested,
				combinedStatus,
				combinedStatusStr,
				ddufpgaDebugTrap,
				infpga0DebugTrap,
				infpga1DebugTrap,
				fmmErrorThreshold,
				numChambersInErrorForEndcap,
				fmmsReleased,
				chambersInError,
				fibersInError,
				nParameters};
			static const char* getTypeName() { return typeName_; }
			static const std::string getParameterName(const ParameterName_t p) { return parameterNames_[p]; }
		protected:
			static const char* const typeName_;
			static const char* const parameterNames_[nParameters];
		};

	}
}

#endif
