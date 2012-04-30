/** \file testXMLReadWriter.cc
 * $Id: testXMLReadWriter.cc,v 1.1 2012/04/30 23:49:59 khotilov Exp $
 *
 * tests for the XMLReadWriter class
 *
 */

#include "emu/db/XMLReadWriter.h"
#include "emu/db/ConfigTree.h"
#include "emu/db/PCConfigHierarchy.h"

#include <iostream>

using namespace emu::db;
using namespace std;

int main(int argc, char **argv)
{
  // initialize the configuration tables hierarchy (including table definitions):
  PCConfigHierarchy h("/home/cscdev/TriDAS/emu/emuDCS/OnlineDB/xml/EMUsystem.view");

  // just read and then write into another file
  XMLReadWriter xml(&h, "/home/cscdev/config/pc/2000114.xml");
  xml.read(2000114);
  xml.setFileName("test1.xml");
  xml.write();
  
  // create a ConfigTree, and write it out into one more file using one more XMLReadWriter
  ConfigTree tree_from_xml(xml.configTables());
  XMLReadWriter xml_write(&h, "test2.xml");
  xml_write.write(&tree_from_xml);

  return 0;
}
