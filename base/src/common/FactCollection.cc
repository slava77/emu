#include "emu/base/FactCollection.h"
#include <iostream>

using namespace std;

const char* emu::base::FactCollection::sources_[] = { "DCSFactService",
						      "LocalDQMFactService",
						      "LocalDAQFactService",
						      "XmasFactService", 
						      "FEDFactService" };

emu::base::FactCollection&
emu::base::FactCollection::setRequestId( const int r ){
  stringstream ss;
  ss << r;
  requestId_ = ss.str();  
  return *this;
}

ostream& 
emu::base::operator<<( ostream& os, emu::base::FactCollection& fc ){

  os << "Source          " << fc.getSource   ()    << endl
     << "RequestId       " << fc.getRequestId()    << endl
     << "Number of facts " << fc.getFacts().size() << endl;
  int factCount = 0;
  vector<emu::base::Fact>::const_iterator f;
  for ( f = fc.getFacts().begin(); f != fc.getFacts().end(); f++ ){ os << " Fact " << ++factCount << endl << *f; }

  return os;
}

ostream& 
emu::base::operator<<( ostream& os, const emu::base::FactCollection& fc ){

  os << "Source          " << fc.getSource   ()    << endl
     << "RequestId       " << fc.getRequestId()    << endl
     << "Number of facts " << fc.getFacts().size() << endl;
  int factCount = 0;
  vector<emu::base::Fact>::const_iterator f;
  for ( f = fc.getFacts().begin(); f != fc.getFacts().end(); f++ ){ os << " Fact " << ++factCount << endl << *f; }

  return os;
}
