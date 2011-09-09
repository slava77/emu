/** \file testConfigTree.cc
 * $Id: testConfigTree.cc,v 1.1 2011/09/09 16:06:47 khotilov Exp $
 *
 * tests for the ConfigTree class
 *
 */

#include "emu/db/ConfigTree.h"
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

  // Test ConfigTree
  ConfigTree tree(tables);
  
  cout<<h<<endl;

  cout<<endl<<"traversing tree with const_iterator:"<<endl;
  for (ConfigTree::const_iterator it = tree.begin(); it != tree.end(); it++) { cout<<**it<<endl; }

  cout<<endl<<"traversing tree with iterator:"<<endl;
  for (ConfigTree::iterator it = tree.begin(); it != tree.end(); it++) { cout<<**it<<endl; }

  cout<<endl<<"find AFEBs:"<<endl;
  std::vector<ConfigRow*> afebs = tree.find("AnodeChannel");
  for (std::vector<ConfigRow*>::iterator it = afebs.begin(); it != afebs.end(); it++) { cout<<**it<<endl; }

  cout<<endl<<"find some non-existing type nodes:"<<endl;
  std::vector<ConfigRow*> zzzs = tree.find("ZZZZ");
  for (std::vector<ConfigRow*>::iterator it = zzzs.begin(); it != zzzs.end(); it++) { cout<<**it<<endl; }

  cout<<endl<<"find a particular TMB:"<<endl;
  ConfigRow * tmb = tree.find("TMB", 200000301022000);
  if (tmb) cout<<*tmb<<endl;
  else cout<<"not found"<<endl;

  cout<<endl<<"find a non-existing TMB:"<<endl;
  tmb = tree.find("TMB", 200000322022000);
  if (tmb) cout<<*tmb<<endl;
  else cout<<"not found"<<endl;

  cout<<endl<<"get a table of nonexitsting type ZZZZ (expect trivial table):"<<endl;
  xdata::Table tbl = tree.table("ZZZZ");
  tbl.writeTo(cout); cout<<endl;

  cout<<endl<<"get a table of AnodeChannels:"<<endl;
  tbl = tree.table("AnodeChannel");
  tbl.writeTo(cout); cout<<endl;

  return 0;
}
