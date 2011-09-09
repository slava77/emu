/** \file testTableDefinitions.cc
 * $Id: testTableDefinitions.cc,v 1.1 2011/09/09 16:06:48 khotilov Exp $
 *
 * tests for the TableDefinitions class
 *
 */

#include "emu/db/TableDefinitions.h"

#include "xdata/Table.h"

#include <iostream>
#include <iterator>

//using namespace emu::db;
using namespace std;

int main(int argc, char **argv)
{
  emu::db::TableDefinitions defs("/home/cscdev/TriDAS/emu/emuDCS/OnlineDB/xml/EMUsystem.view", "EMU_");

  cout<<"testing PC..."<<endl<<endl;
  cout<<"TStore view: "<<defs.tstoreViewID()<<endl<<endl;
  vector<string> types = defs.types();
  cout<<types.size()<<" types:  ";
  copy(types.begin(), types.end(), ostream_iterator<string>(cout," "));
  cout<<endl<<endl;
  for (vector<string>::iterator it = types.begin(); it != types.end(); it++)
  {
    xdata::Table * table_def = defs.tableDefinition(*it);
    cout<<"table of type  "<<*it<<"  ("<<table_def->getColumns().size()<<" columns)"<<endl;
    table_def->writeTo(cout);cout<<endl<<endl;
  }

  emu::db::TableDefinitions defs_fed("/home/cscdev/TriDAS/emu/emuDCS/FEDUtils/xml/TStoreConfiguration.view", "EMU_FED_");

  cout<<endl<<"testing FED..."<<endl<<endl;
  cout<<"TStore view: "<<defs_fed.tstoreViewID()<<endl<<endl;
  types = defs_fed.types();
  cout<<types.size()<<" types:  ";
  copy(types.begin(), types.end(), ostream_iterator<string>(cout," "));
  cout<<endl<<endl;
  for (vector<string>::iterator it = types.begin(); it != types.end(); it++)
  {
    xdata::Table * table_def = defs_fed.tableDefinition(*it);
    cout<<"table of type  "<<*it<<"  ("<<table_def->getColumns().size()<<" columns)"<<endl;
    table_def->writeTo(cout);cout<<endl<<endl;
  }

  return 0;
}
