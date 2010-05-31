/*****************************************************************************\
* $Id: ConfigurationFact.h,v 1.1 2010/05/31 14:57:19 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_CONFIGURATIONFACT_H__
#define __EMU_FED_CONFIGURATIONFACT_H__

#include "emu/fed/Fact.h"

namespace emu {
	namespace fed {

		/// The entire configuration (crate vector) encoded in XML for parsing?
		/// FIXME 
		class ConfigurationFact {
		public:
			enum ParameterName_t {systemName, configuration, id, nParameters};
			static const char* getTypeName() { return typeName_; }
			static const std::string getParameterName(const ParameterName_t p) { return parameterNames_[p]; }
		protected:
			static const char* const typeName_;
			static const char* const parameterNames_[nParameters];
		};

	}
}

#endif
