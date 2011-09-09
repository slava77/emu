// $Id: TableDefinitions.cc,v 1.1 2011/09/09 16:04:45 khotilov Exp $

#include "emu/db/TableDefinitions.h"
#include "emu/utils/IO.h"
#include "emu/utils/System.h"
#include "emu/utils/DOM.h"

#include "toolbox/string.h"

using namespace emu::utils;

namespace emu { namespace db {


TableDefinitions::TableDefinitions(const std::string &xml_location, const std::string &prefix)
: prefix_(prefix)
{
  std::string configurationXML;
  try
  {
    configurationXML = readFile(xml_location);
  }
  catch (const std::exception & e)
  {
    XCEPT_RAISE(emu::exception::XMLException, e.what());
  }

  parseTStoreViewID(configurationXML);

  parseDefinitions(configurationXML);
}


std::vector<std::string> TableDefinitions::types() const
{
  std::vector<std::string> result;
  for (DefMap::const_iterator it = definitions_.begin(); it != definitions_.end(); it++)
  {
    result.push_back(it->first);
  }
  return result;
}

xdata::Table * TableDefinitions::tableDefinition(const std::string & type)
{
  if (definitions_.count(type) == 0) return 0;
  return &(definitions_[type]);
}


void TableDefinitions::parseTStoreViewID(const std::string &xml)
throw (emu::exception::XMLException)
{
  tstore_view_id_ = "";
  try
  {
    tstore_view_id_ = emu::utils::getSelectedNodeValue(xml, "//tstore:configuration/tstore:view/@id");
  }
  catch (xcept::Exception &e)
  {
    XCEPT_RAISE( xcept::Exception, "Cannot parse tstore_view_id: " + std::string(e.what()) );
  }
}


void TableDefinitions::parseDefinitions(const std::string &xml)
throw (emu::exception::XMLException)
{
  // first, parse types (table names):
  VectorPairStrings tab_names;
  try
  {
    tab_names = emu::utils::getSelectedNodesValues(xml, "//tstore:configuration/tstore:view/tstore:table/@name");
  }
  catch (xcept::Exception &e)
  {
    XCEPT_RAISE( xcept::Exception, "Cannot parse tables: " + std::string(e.what()) );
  }

  std::vector< std::string >  table_names;
  for (VectorPairStrings::iterator name = tab_names.begin(); name != tab_names.end(); name++)
  {
    if ( toolbox::toupper( name->second.substr(0, prefix_.size()) ) == toolbox::toupper(prefix_) )
    {
      table_names.push_back(name->second);
    }
  }

  // parse types' definitions:
  xdata::Table trivial_table;

  for (std::vector< std::string >::iterator t = table_names.begin(); t != table_names.end(); t++)
  {
    VectorPairStrings col_names, type_names;
    try
    {
      col_names = getSelectedNodesValues(xml,
        "//tstore:configuration/tstore:view/tstore:table[@name=\"" + *t + "\"]/tstore:column/@name");
      type_names = getSelectedNodesValues(xml,
        "//tstore:configuration/tstore:view/tstore:table[@name=\"" + *t + "\"]/tstore:column/@type");
    }
    catch(xcept::Exception &e)
    {
      XCEPT_RAISE( xcept::Exception, "Cannot parse definitions: " + std::string(e.what()) );
    }
    assert( col_names.size() == type_names.size() );

    if (col_names.size() == 0) continue;

    std::string type = t->substr(prefix_.size()); // table name without prefix
    definitions_[type] = trivial_table;

    // fill table definitions
    //std::cout<<"*** TABLE:  "<< table_name <<std::endl;
    VectorPairStrings::const_iterator col_name = col_names.begin(), col_type = type_names.begin();
    for (; col_name != col_names.end(); col_name++, col_type++)
    {
      //std::cout<<"    "<< col_name->second<<" : "<<col_type->second<<std::endl;
      definitions_[type].addColumn(col_name->second, col_type->second);
      //if (col_name->second != toolbox::toupper(col_name->second)) std::cout<<"NOT ALL UPPER: "<<col_name->second<<std::endl;
    }
  }
}


}}
