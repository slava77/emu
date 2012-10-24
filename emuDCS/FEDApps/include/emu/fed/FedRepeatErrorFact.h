/*****************************************************************************\
* $Id: FedRepeatErrorFact.h,v 1.1 2012/10/24 15:14:43 cvuosalo Exp $
\*****************************************************************************/
#ifndef __EMU_FED_FEDREPEATERRORFACT_H__
#define __EMU_FED_FEDREPEATERRORFACT_H__

#include "emu/fed/Fact.h"

namespace emu {
	namespace fed {

		/// List of chambers repeatedly in error
		class FedRepeatErrorFact {
		public:
			enum ParameterName_t {chambersInError, numChambersInError,
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
