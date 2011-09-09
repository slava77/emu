#ifndef __EMU_DB_TABLEDEFINITIONS_H__
#define __EMU_DB_TABLEDEFINITIONS_H__

#include "xdata/Table.h"

#include "emu/exception/Exception.h"

#include <vector>
#include <map>
#include <string>

namespace emu { namespace db {


/** @class TableDefinitions
 * A utility class that reads in table definitions from TStore view configuration XML
 * and provides them in form of empty xdata::Tables
 *
 **/
class TableDefinitions
{
public:
  /** Constructor
   * Reads in XML configuration of TStore view and parses is for table definitions
   *
   * @param xml_location  location of the TStore view configuration XML file
   * @param prefix  the required prefix for table names (table name = prefix + type)
   */
  TableDefinitions(const std::string &xml_location, const std::string &prefix);

  /// destructor
  ~TableDefinitions() {};

  //virtual int operator==(const TableDefinitions & d);

  // ----- accessors -----

  /// whether we got any definitions at all
  bool valid() const { return definitions_.size() > 0; }

  /// table names' prefix
  std::string prefix() const {return prefix_;}

  /// system's TStore view ID
  std::string tstoreViewID() const {return tstore_view_id_;}

  /**
   * \returns vector of all table "types" that were read in from the configuration file.
   * table types are always upper case
   */
  std::vector<std::string> types() const;
  
  /**
   * \returns pointer to the definition of a table for a given table type
   * The "definition" is an empty table with proper column types.
   * Column names preserve their letter case from the xml.
   */
  xdata::Table * tableDefinition(const std::string &type);

private:

  typedef std::vector< std::pair< std::string, std::string > > VectorPairStrings;
  typedef std::map<std::string, xdata::Table > DefMap;

  void parseTStoreViewID(const std::string &xml) throw (emu::exception::XMLException);
  void parseDefinitions(const std::string &xml) throw (emu::exception::XMLException);

  /// table name prefix
  std::string prefix_;

  /// the key is table type, the value is definition
  /// table types are always upper case
  DefMap definitions_;

  /// system's TStore view ID
  std::string tstore_view_id_;
};

}}

#endif
