#ifndef __EMU_FED_FACTS_H__
#define __EMU_FED_FACTS_H__

// Macro for easily generating DDURegister facts
#define DDU_REGISTER_FACT( TYPE ) \
class TYPE \
{ \
public: \
	enum ParameterName_t {description, value, base, debug, nParameters}; \
	static const char* getTypeName() { return typeName_; } \
	static const std::string getParameterName(const ParameterName_t p) { return parameterNames_[p]; } \
protected: \
	static const char* const typeName_; \
	static const char* const parameterNames_[nParameters]; \
}; \
const char* const TYPE::typeName_ = # TYPE; \
const char* const TYPE::parameterNames_[] = {"value", "base", "debug"};

#include "emu/base/TypedFact.h"

namespace emu {
	namespace fed {

		/// The entire configuration (crate vector) encoded in XML for parsing
		class ConfigurationFact {
		public:
			enum ParameterName_t {configuration, nParameters};
			static const char* getTypeName() { return typeName_; }
			static const std::string getParameterName(const ParameterName_t p) { return parameterNames_[p]; }
		protected:
			static const char* const typeName_;
			static const char* const parameterNames_[nParameters];
		};
		const char* const ConfigurationFact::typeName_ = "ConfigurationFact";
		const char* const ConfigurationFact::parameterNames_[] = {"configuration"};

		DDU_REGISTER_FACT(DDUFlashBoardIDFact)

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
