#ifndef __APPLICATIONSTATUSFACT_H__
#define __APPLICATIONSTATUSFACT_H__

#include <string>

using namespace std;

namespace emu { namespace base {

  class ApplicationStatusFact {
  public:
    enum ParameterName_t { state, nParameters };
    static const char*  getTypeName() { return typeName_; }
    static const string getParameterName( const ParameterName_t p ) { return parameterNames_[p]; }
  protected:
    static const char* const typeName_; 
    static const char* const parameterNames_[nParameters];
  };

}}

#endif // ifndef __APPLICATIONSTATUSFACT_H__
