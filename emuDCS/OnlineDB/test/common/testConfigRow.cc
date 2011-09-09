/** \file testConfigRow.cc
 * $Id: testConfigRow.cc,v 1.1 2011/09/09 16:06:47 khotilov Exp $
 *
 * tests for the ConfigRow class
 *
 */

#include "emu/db/ConfigRow.h"
#include "emu/db/ConfigTable.h"
#include "emu/db/PCConfigHierarchy.h"

#include "xdata/TableIterator.h"

#include <iostream>
#include <iterator>
#include <map>
#include <string>

using namespace emu::db;
using namespace std;


void setupDummyTables(std::map<std::string, ConfigTable> &tabs, PCConfigHierarchy &h)
{
  tabs = h.definitionsTree();
  const size_t ntypes = h.types().size();
  for (size_t t = 0; t < ntypes; t++)
  {
    string type = h.types()[t];
    cout<<t<<" \t"<<type<<endl;
    //cout<<tabs[type]<<endl;

    xdata::Table::iterator irow = tabs[type].append();

    xdata::UnsignedInteger64 conf_id = 2000003;
    irow->setField(h.idFieldNameOfHead(), conf_id);

    xdata::UnsignedInteger64 id, parent_id;
    if(type=="Configuration") {id = 2000003; parent_id = 0; }
    if(type=="PeripheralCrate") {id = 200000301000000; parent_id = 2000003; }
    if(type=="VCC") {id = 200000301130000; parent_id = 200000301000000; }
    if(type=="MPC") {id = 200000301120000; parent_id = 200000301000000; }
    if(type=="CCB") {id = 200000301110000; parent_id = 200000301000000; }
    if(type=="CSC") {id = 200000301020000; parent_id = 200000301000000; }
    if(type=="DAQMB") {id = 200000301021000; parent_id = 200000301020000; }
    if(type=="CFEB") {id = 200000301021002; parent_id = 200000301021000; }
    if(type=="TMB") {id = 200000301022000; parent_id = 200000301020000; }
    if(type=="ALCT") {id = 200000301022100; parent_id = 200000301022000; }
    if(type=="AnodeChannel") {id = 200000301022114; parent_id = 200000301022100; }

    irow->setField(h.idFieldName(type), id);
    if (type!="Configuration") irow->setField(h.idFieldNameOfParent(type), parent_id);
    //tabs[t].writeTo(cout); cout<<endl;
  }

  string type = "AnodeChannel";
  xdata::Table::iterator irow = tabs[type].append();
  xdata::UnsignedInteger64 conf_id = 2000003, id = 200000301022113, parent_id = 200000301022100;
  irow->setField(h.idFieldNameOfHead(), conf_id);
  irow->setField(h.idFieldName(type), id);
  irow->setField(h.idFieldNameOfParent(type), parent_id);
}


int main(int argc, char **argv)
{
  // initialize the configuration tables hierarchy (including table definitions):
  PCConfigHierarchy h("/home/cscdev/TriDAS/emu/emuDCS/OnlineDB/xml/EMUsystem.view");

  // setup the dummy tables
  std::map<std::string, ConfigTable> tables;
  setupDummyTables(tables, h);

  const size_t ntypes = h.types().size();

  // Test ConfigRows
  ConfigRow *cr[ntypes+1];
  for (size_t t = 0; t < ntypes; t++)
  {
    string type = h.types()[t];
    cr[t] = new ConfigRow(type, &(tables[type]), 0);

    //tables[t].writeTo(cout); cout<<endl;
    //cout<<*cr[t]<<endl<<endl;
  }
  string type = "AnodeChannel";
  cr[ntypes] = new ConfigRow(type, &(tables[type]), 1);

  cout<<h<<endl;
  //cout<<*(cr[0])<<endl<<endl;
  //cout<<*cr[1]<<endl<<endl;

  cr[0]->addChild(cr[1]);
  cr[1]->addChild(cr[2]);
  cr[1]->addChild(cr[3]);
  cr[1]->addChild(cr[4]);
  cr[1]->addChild(cr[5]);
  cr[5]->addChild(cr[6]);
  cr[6]->addChild(cr[7]);
  cr[5]->addChild(cr[8]);
  cr[8]->addChild(cr[9]);
  cr[9]->addChild(cr[10]);
  cr[9]->addChild(cr[11]);


  for (size_t t = 0; t <= ntypes; t++) cout<<*cr[t]<<endl<<endl;

  //clean up
  for (size_t t = 0; t <= ntypes; t++) delete cr[t];

  return 0;
}
