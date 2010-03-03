#include "emu/base/Fact.h"
#include <iostream>
#include <iomanip>
#include <time.h>

using namespace std;

const char* const emu::base::Fact::units_[]         = { "AMPERE", "VOLT", "METER" };
const char* const emu::base::Fact::severities_[]    = { "DEBUG",
							"INFO",
							"MINOR",
							"TOLERABLE",
							"WARN",
							"ERROR",
							"SEVERE",
							"CRITICAL",
							"FATAL" };

emu::base::Fact::Fact()
  : time_( defaultDateTime() ){}

emu::base::Fact::Fact( const string& name )
  : time_( defaultDateTime() ),
    name_( name              ){}

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
     << "   ComponentId    " << f.getComponentId   () << endl
     << "   Run            " << f.getRun           () << endl
     << "   Severity       " << f.getSeverity      () << endl
     << "   Description    " << f.getDescription   () << endl;
  map<string,string> parameters = f.getParameters();
  map<string,string>::iterator p;
  for ( p = parameters.begin(); p != parameters.end(); ++p )
    os << "   " << p->first << string(15-p->first.size(),' ') << p->second << endl;
  return os;
}

ostream& emu::base::operator<<( ostream& os, const emu::base::Fact& f ){
  os << "   Name           " << f.getName          () << endl
     << "   Time           " << f.getTime          () << endl
     << "   ComponentId    " << f.getComponentId   () << endl
     << "   Run            " << f.getRun           () << endl
     << "   Severity       " << f.getSeverity      () << endl
     << "   Description    " << f.getDescription   () << endl;
  map<string,string>::const_iterator p;
  for ( p = f.getParameters().begin(); p != f.getParameters().end(); ++p )
    os << "   " << p->first << string(15-p->first.size(),' ') << p->second << endl;
  return os;
}
