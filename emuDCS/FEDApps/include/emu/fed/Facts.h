#ifndef __FACTS_H__
#define __FACTS_H__

#include "emu/base/TypedFact.h"

namespace emu {
	namespace fed {
		
		class dduVoltageFact {
			
		public:
			enum ParameterName_t {voltage15, voltage25_1, voltage25_2, voltage33, nParameters};
			static const char*  getTypeName() { return typeName_; }
			static const string getParameterName(const ParameterName_t p) { return parameterNames_[p]; }
		protected:
			static const char* const typeName_;
			static const char* const parameterNames_[nParameters];
		};
		const char* const dduVoltageFact::typeName_ = "dduVoltageFact";
		const char* const dduVoltageFact::parameterNames_[] = {"voltage15", "voltage25_1", "voltage25_2", "voltage33"};
		
	}
}

#endif
