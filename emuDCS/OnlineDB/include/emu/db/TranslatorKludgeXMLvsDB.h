#ifndef __EMU_DB_TRANSLATORKLUDGEXMLVSDB_H__
#define __EMU_DB_TRANSLATORKLUDGEXMLVSDB_H__

#include <map>
#include <string>

#include "emu/exception/Exception.h"

namespace emu {	namespace db {

/** @class TranslatorKludgeXMLvsDB
 * A translator-type kludge to etablish proper XML <-> DB correspondence by compensate for the following
 * inconsitencies between XML and DB:
 *    *some XML attribute names are different to DB field names
 *    * some XML attributes have values of different type then DB columns (e.g., hex format in XML, while int in db)
 * input: xml attribute name,  output: db field name
 */
class TranslatorKludgeXMLvsDB
{
public:

  TranslatorKludgeXMLvsDB();

  /// adjust attribute name and value when importing it from XML into xdata::Table (or "DB") space
  void XML2DB(std::string &attr_name, std::string &attr_value) throw (emu::exception::ConfigurationException);

  /// adjust field name and value when exporting it from xdata::Table (or "DB") space into XML attribute
  void DB2XML(std::string &field_name, std::string &field_value) throw (emu::exception::ConfigurationException);

private:

  std::map<std::string, std::string> xml2db_;
  std::map<std::string, std::string> db2xml_;
};

}}

#endif
