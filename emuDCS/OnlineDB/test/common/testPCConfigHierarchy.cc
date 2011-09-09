/** \file testPCConfigHierarchy.cc
 * $Id: testPCConfigHierarchy.cc,v 1.1 2011/09/09 16:06:47 khotilov Exp $
 *
 * tests for the PCConfigHierarchy class
 *
 */

#include "emu/db/PCConfigHierarchy.h"

#include <iostream>
#include <iterator>

using namespace emu::db;
using namespace std;

int main(int argc, char **argv)
{
  PCConfigHierarchy h("/home/cscdev/TriDAS/emu/emuDCS/OnlineDB/xml/EMUsystem.view");
  //PCConfigHierarchy h;

  cout<<endl<<"PRINTOUT OF DEFINITIONS:"<<endl<<endl;
  std::string type = h.typeOfHead();
  h.print(cout, type, 0, 1);

  cout<<endl<<"SIMPLE PRINTOUT:"<<endl<<endl;
  cout<<h<<endl;

  vector<string> types = h.types();
  cout<<endl<<"VECTOR OF TYPES: ";
  copy(types.begin(), types.end(), ostream_iterator<string>(cout," ")); cout<<endl;
  cout<<"HEAD: "<<h.typeOfHead()<<endl;

  cout<<endl<<"TYPE --> PARENT:"<<endl;
  for (std::vector< std::string >::iterator t = types.begin(); t != types.end(); t++)
  {
    cout<<*t<<"\t -->\t "<<h.typeOfParent(*t)<<endl;
  }
  cout<<endl;

  cout<<"type of flash table: ";
  h.print(cout, h.typeOfFlashTable(), 0, 1);
  cout<<endl;

  return 0;
}
