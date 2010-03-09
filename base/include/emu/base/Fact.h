#ifndef __FACT_H__
#define __FACT_H__

#include "emu/base/Component.h"

#include <sstream>
#include <ostream>
#include <map>


namespace emu { namespace base {
  class Fact;
  using namespace std;
  ///
  /// 
  ///
  class Fact {
  public:

    enum ValueType_t   { stringType=0, numericType, booleanType, nValueTypes };
    enum Unit_t        { AMPERE=0, VOLT, METER, nUnits };
    enum Severity_t    { DEBUG=0,
			 INFO,
			 MINOR,
			 TOLERABLE,
			 WARN,
			 ERROR,
			 SEVERE,
			 CRITICAL,
			 FATAL,
			 nSeverities };

    friend ostream& operator<<( ostream& os,       emu::base::Fact& f );
    friend ostream& operator<<( ostream& os, const emu::base::Fact& f );

    Fact();
    Fact( const string& name );

    const string& getRun           () const { return run_;                  }
    const string& getTime          () const { return time_;                 }
    string        getOneComponentId() const;
    const string& getUnit          () const { return unit_;                 }
    const string& getSeverity      () const { return severity_;             }
    const string& getDescription   () const { return description_;          }
    const string& getName          () const { return name_;                 }
    const emu::base::Component& getComponent()  const { return component_;  }
    const map<string,string>&   getParameters() const { return parameters_; }

    void setComponentId( const string& componentId     ){ component_.addId( componentId ); }
    void setComponent  ( const emu::base::Component& c ){ component_ = c;                  }

  protected:
    string defaultDateTime();
   
    static const char* const units_[nUnits];
    static const char* const severities_[nSeverities];
    string run_;
    string time_;
    //string componentId_;
    emu::base::Component component_;
    string unit_;
    string severity_;
    string description_;
    string name_;

    map<string,string> parameters_;

  };
}} // namespace emu::base

#endif  // ifndef __FACT_H__
