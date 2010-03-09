#include "emu/base/FactRequest.h"

using namespace std;

ostream& 
emu::base::operator<<( ostream& os, emu::base::FactRequest& fri ){
  os << "    ComponentIds " << fri.getComponent() << endl
     << "    Fact type    " << fri.getFactType () << endl;
  return os;
}

ostream& 
emu::base::operator<<( ostream& os, const emu::base::FactRequest& fri ){
  os << "    ComponentIds " << fri.getComponent() << endl
     << "    Fact type    " << fri.getFactType () << endl;
  return os;
}

ostream& 
emu::base::operator<<( ostream& os, emu::base::FactRequestCollection& frc ){

  os << "Request id      " << frc.getRequestId() << endl;
  int requestCount = 0;
  vector<emu::base::FactRequest>::iterator fr;
  vector<emu::base::FactRequest> requests = frc.getRequests();
  for ( fr = requests.begin(); fr != requests.end(); fr++ ){ os << " Request " << ++requestCount << endl << *fr; }

  return os;
}

ostream& 
emu::base::operator<<( ostream& os, const emu::base::FactRequestCollection& frc ){

  os << "Request id      " << frc.getRequestId() << endl;
  int requestCount = 0;
  vector<emu::base::FactRequest>::const_iterator fr;
  for ( fr = frc.getRequests().begin(); fr != frc.getRequests().end(); fr++ ){ os << " Request " << ++requestCount << endl << *fr; }

  return os;
}
