/*****************************************************************************\
* $Id: Fact.h,v 1.1 2010/05/31 14:57:19 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_FACT__
#define __EMU_FED_FACT__

#include "emu/base/TypedFact.h"

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

#endif
