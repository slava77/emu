#ifndef __TYPEDFACT_H__
#define __TYPEDFACT_H__

#include "emu/base/Fact.h"


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
  const char* const ApplicationStatusFact::typeName_ = "applicationStatusFact";
  const char* const ApplicationStatusFact::parameterNames_[] = { "state" };


  template <class T>
  class TypedFact : public Fact {
  public:
    TypedFact<T>() : Fact( T::getTypeName() ){}

    TypedFact<T>& setRun        ( const string& run                      ){ run_         = run;              return *this; }
    TypedFact<T>& setTime       ( const string& time                     ){ time_        = time;             return *this; }
    TypedFact<T>& setDescription( const string& description              ){ description_ = description;      return *this; }
    TypedFact<T>& setComponentId( const string& componentId              ){ component_.addId( componentId ); return *this; }
    TypedFact<T>& setComponent  ( const emu::base::Component&          c ){ component_   = c;                return *this; }
    TypedFact<T>& setUnit       ( const emu::base::Fact::Unit_t        u ){ unit_        = units_[u];        return *this; }
    TypedFact<T>& setSeverity   ( const emu::base::Fact::Severity_t    s ){ severity_    = severities_[s];   return *this; }

    template <typename V>
    TypedFact<T>& setParameter( const typename T::ParameterName_t name, const V& value ){
      stringstream ss;
      ss << value;
      parameters_[ T::getParameterName(name) ] = ss.str();
      return *this;
    }
  };

}}

#endif // ifndef __TYPEDFACT_H__
