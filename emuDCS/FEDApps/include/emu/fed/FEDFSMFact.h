/*****************************************************************************\
* $Id: FEDFSMFact.h,v 1.1 2010/08/30 17:24:29 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_FEDFSMFACT_H__
#define __EMU_FED_FEDFSMFACT_H__

#include "emu/fed/Fact.h"

namespace emu {
	namespace fed {

		class FEDFSMFact {
		public:
			enum ParameterName_t {from, to, configType, configValue, result, nParameters};
			static const char* getTypeName() { return typeName_; }
			static const std::string getParameterName(const ParameterName_t p) { return parameterNames_[p]; }
		protected:
			static const char* const typeName_;
			static const char* const parameterNames_[nParameters];
		};

	}
}

#endif
