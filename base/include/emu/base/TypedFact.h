#ifndef __TYPEDFACT_H__
#define __TYPEDFACT_H__

#include "emu/base/Fact.h"

#include "emu/utils/String.h"


namespace emu { namespace base {

  template <class T>
  class TypedFact : public Fact {
  public:
    TypedFact<T>() : Fact( T::getTypeName() ){
      parameters_.resize( T::nParameters );
      for ( size_t i = 0; i < T::nParameters; ++i ) parameters_[i].first = T::getParameterName( (typename T::ParameterName_t) i );
    }

    TypedFact<T>& setRun        ( const string& run                      ){ run_         = run;              return *this; }
    TypedFact<T>& setTime       ( const string& time                     ){ time_        = time;             return *this; }
    TypedFact<T>& setDescription( const string& description              ){ description_ = description;      return *this; }
    TypedFact<T>& setComponentId( const string& componentId              ){ component_.addId( componentId ); return *this; }
    TypedFact<T>& setComponent  ( const emu::base::Component&          c ){ component_   = c;                return *this; }
    TypedFact<T>& setUnit       ( const emu::base::Fact::Unit_t        u ){ unit_        = units_[u];        return *this; }
    TypedFact<T>& setSeverity   ( const emu::base::Fact::Severity_t    s ){ severity_    = severities_[s];   return *this; }

    /// Set scalar parameter
    ///
    /// @param name Name of parameter.
    /// @param value Value of parameter.
    ///
    /// @return Reference to this object.
    ///
    template <typename V>
    TypedFact<T>& setParameter( const typename T::ParameterName_t name, const V& value ){
      // Scalar parameter is also stored as vector:
      vector<string> vs( 1, emu::utils::stringFrom<V>( value ) );
      parameters_[ name ].second.swap( vs );
      return *this;
    }

    /// Set array parameter
    ///
    /// @param name Name of parameter.
    /// @param values STL vector of parameter values.
    ///
    /// @return  Reference to this object.
    ///
    template <typename V>
    TypedFact<T>& setParameter( const typename T::ParameterName_t name, const vector<V>& values ){
      vector<string> vs( stringify( values ) );
      parameters_[ name ].second.swap( vs );
      return *this;
    }

  protected:

    template <typename V>
    vector<string> stringify( const vector<V>& values ){
      vector<string> strings;
      typename vector<V>::const_iterator iv;
      for ( iv = values.begin(); iv != values.end(); ++iv ){
	strings.push_back( emu::utils::stringFrom<V>( *iv ) );
      }
      return strings;
    }

  };

}}

#endif // ifndef __TYPEDFACT_H__
