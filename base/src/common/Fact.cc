#include "emu/base/Fact.h"

#include <iostream>
#include <iomanip>
#include <time.h>

using namespace std;

const char* const emu::base::Fact::units_[]         = { "AMPERE", "VOLT", "METER" };
const char* const emu::base::Fact::severities_[]    = { "NONE",
							"MINOR",
							"TOLERABLE",
							"SEVERE",
							"CRITICAL",
							"DEBUG",
							"INFO",
							"WARN",
							"ERROR",
							"FATAL" };

emu::base::Fact::Fact()
  : time_( defaultDateTime() ){}

emu::base::Fact::Fact( const string& name )
  : time_( defaultDateTime() ),
    name_( name              ){}

string
emu::base::Fact::getOneComponentId() const {
  if ( component_.getIds().empty() ) return "";
  else                               return *component_.getIds().begin();
}


string
emu::base::Fact::defaultDateTime(){
  time_t t;
  struct tm *tm;

  time ( &t );
  tm = gmtime ( &t );

  stringstream ss;
  ss << setfill('0') << setw(4) << tm->tm_year+1900 << "-"
     << setfill('0') << setw(2) << tm->tm_mon+1     << "-"
     << setfill('0') << setw(2) << tm->tm_mday      << "T"
     << setfill('0') << setw(2) << tm->tm_hour      << ":"
     << setfill('0') << setw(2) << tm->tm_min       << ":"
     << setfill('0') << setw(2) << tm->tm_sec       << "Z";

  return ss.str();
}

ostream& emu::base::operator<<( ostream& os, emu::base::Fact& f ){
  os << "   Name           " << f.getName          () << endl
     << "   Time           " << f.getTime          () << endl
     << "   Component      " << f.getComponent     () << endl
     << "   Run            " << f.getRun           () << endl
     << "   Severity       " << f.getSeverity      () << endl
     << "   Description    " << f.getDescription   () << endl;

  emu::base::Fact::Parameters_t parameters = f.getParameters();
  emu::base::Fact::Parameters_t::iterator p;

  unsigned int maxNameLength = 0;
  for ( p = parameters.begin(); p != parameters.end(); ++p ){
    if ( p->first.size() > maxNameLength ) maxNameLength = p->first.size();
  }

  for ( p = parameters.begin(); p != parameters.end(); ++p ){
    os << "   " << p->first 
       << string(3+maxNameLength-p->first.size(),' ')
       << "[";
    for (vector<string>::const_iterator s = p->second.begin(); s != p->second.end(); ++s){
      os << *s << (s + 1 != p->second.end() ? "," : "");
    }
    os << "]" << endl;
  }

  return os;
}

ostream& emu::base::operator<<( ostream& os, const emu::base::Fact& f ){

  os << "   Name           " << f.getName          () << endl
     << "   Time           " << f.getTime          () << endl
     << "   Component      " << f.getComponent     () << endl
     << "   Run            " << f.getRun           () << endl
     << "   Severity       " << f.getSeverity      () << endl
     << "   Description    " << f.getDescription   () << endl;

  emu::base::Fact::Parameters_t::const_iterator p;

  unsigned int maxNameLength = 0;
  for ( p = f.getParameters().begin(); p != f.getParameters().end(); ++p ){
    if ( p->first.size() > maxNameLength ) maxNameLength = p->first.size();
  }

  for ( p = f.getParameters().begin(); p != f.getParameters().end(); ++p ){
    os << "   " << p->first 
       << string(3+maxNameLength-p->first.size(),' ')
       << "[";
    for (vector<string>::const_iterator s = p->second.begin(); s != p->second.end(); ++s){
      os << *s << (s + 1 != p->second.end() ? "," : "");
    }
    os << "]" << endl;
  }
  return os;
}
