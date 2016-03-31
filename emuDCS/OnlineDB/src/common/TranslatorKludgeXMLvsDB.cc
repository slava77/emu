// $Id: TranslatorKludgeXMLvsDB.cc,v 1.1 2011/09/09 16:04:45 khotilov Exp $

#include "emu/db/TranslatorKludgeXMLvsDB.h"

#include "toolbox/string.h"

namespace emu {	namespace db {
	

TranslatorKludgeXMLvsDB::TranslatorKludgeXMLvsDB()
{
  // some XML attributes (key) might be different then DB fields (value)
  xml2db_["problem_location_mask"] = "problem_mask";
  db2xml_["problem_mask"] = "problem_location_mask";
}


void TranslatorKludgeXMLvsDB::XML2DB(std::string &attr_name, std::string &attr_value)
throw (emu::exception::ConfigurationException)
{
  std::map<std::string, std::string>::iterator itr = xml2db_.find(attr_name);
  if (itr == xml2db_.end()) return;
  if (attr_name == "problem_location_mask")
  {
    unsigned int val;
    int err = sscanf(attr_value.c_str(), "%x", &val);
    if (err==0)
    {
      std::ostringstream error;
      error << "ERROR kludging: conversion to hex of " << attr_name << "="<<attr_value<<" failed!!! errcode=" << err << std::endl;
      XCEPT_RAISE(emu::exception::ConfigurationException, error.str());
    }
    attr_value = toolbox::toString("%d", val);
  }
  attr_name = (*itr).second;
}


void TranslatorKludgeXMLvsDB::DB2XML(std::string &field_name, std::string &field_value)
throw (emu::exception::ConfigurationException)
{
  std::map<std::string, std::string>::iterator itr = db2xml_.find(field_name);
  if (itr == db2xml_.end()) return;
  if (field_name == "problem_mask")
  {
    unsigned int val;
    int err = sscanf(field_value.c_str(), "%d", &val);
    if (err==0)
    {
      std::ostringstream error;
      error << "ERROR kludging: conversion to int of " << field_name << "="<<field_value<<" failed!!! errcode=" << err << std::endl;
      XCEPT_RAISE(emu::exception::ConfigurationException, error.str());
    }
    field_value = toolbox::toString("%02x", val);
  }
  field_name = (*itr).second;
}

}}
