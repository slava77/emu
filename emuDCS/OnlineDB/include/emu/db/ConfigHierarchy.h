#ifndef __EMU_DB_CONFIGHIERARCHY_H__
#define __EMU_DB_CONFIGHIERARCHY_H__

#include "emu/db/TableDefinitions.h"

#include <vector>
#include <map>
#include <string>

#include "xdata/UnsignedInteger64.h"

namespace emu { namespace db
{

class ConfigTable;

/** @class ConfigHierarchy
 * Abstract base class for keeping the hierarchical relationships and information for a configuration tree.
 * Specific implementations should be made for PC and FED.
 **/
class ConfigHierarchy
{
public:

  /// encoded configuration tables tree truncation levels (the top table is at level 0, its children are at level 1, etc.)
  enum TruncateLevel {TRUNCATE_0 = 0, TRUNCATE_1, TRUNCATE_2, TRUNCATE_3, TRUNCATE_4, TRUNCATE_5, NOT_TRUNCATE};

  /// trivial constructor
  ConfigHierarchy(): def_(0), truncate_level_(NOT_TRUNCATE), not_for_db_(0) {}

  /// trivial destructor
  virtual ~ConfigHierarchy() {}

  //virtual int operator==(const ConfigHierarchy & h);

  // ----- accessors: -----
  
  /// system's TStore view ID
  std::string tstoreViewID() const {return def_->tstoreViewID();}

  /// the system tables' prefix
  virtual std::string prefix() const {return def_->prefix();}

  /// all types (table name without prefix) for the system
  virtual std::vector<std::string> types() const = 0;
  
  /// type of the head of configuration tree
  virtual std::string typeOfHead() const = 0;

  /// type of the head of configuration tree in XML file (it's usually different for some reason)
  virtual std::string typeOfHeadXML() const = 0;
  
  /// types of children of a certain type
  virtual std::vector<std::string> typesOfChildern(const std::string &type) const = 0;
  
  /// type of a parent of a certain type
  virtual std::string typeOfParent(const std::string &type) const = 0;
  
  /// type of table that keeps firmware flash info (it's separate from the rest of config tree hierarchy)
  virtual std::string typeOfFlashTable() const = 0;

  /// the top level/head configuration ID field name (is expected to be the same in all tables)
  virtual std::string idFieldNameOfHead() const = 0;

  /// the ID field name for a certain type
  virtual std::string idFieldName(const std::string &type) const = 0;

  /// the ID field name of parent for a certain type
  virtual std::string idFieldNameOfParent(const std::string &type) const = 0;

  /// the DB write time field name (this is a field in the head table)
  virtual std::string timeFieldName() const = 0;

  /// name of the description field name (this is a field in the head table)
  virtual std::string descriptionFieldName() const = 0;

  /// accessor for the TableDefinitions object
  virtual TableDefinitions * definitions() {return def_;}

  /** returns an assembled tree of empty but properly defined ConfigTables
   * that already have all the parent-children relationship hierarchically set up
   */
  virtual std::map<std::string, ConfigTable> definitionsTree() = 0;

  /// constructs configuration row's ID that depends on table type by using parent's ID and some provided number
  virtual xdata::UnsignedInteger64 id(const std::string &type, xdata::UnsignedInteger64 &parent_id, xdata::UnsignedInteger64 &number) = 0;

  /// checks if a filed is used only in DB
  virtual bool isFieldDBOnly(const std::string &type, const std::string &field_name) const = 0;

  /// update the config ID part of id_to_update with new_config_id
  /// e.g., for PC tables, config ID is stored in first 7 digits of an ID
  virtual xdata::UnsignedInteger64 updateConfigId(xdata::UnsignedInteger64 &id_to_update, xdata::UnsignedInteger64 &new_config_id) const = 0;

  /// the subsystem of a non-empty config table (returns empty string for an empty table)
  virtual std::string determineSubSystem(ConfigTable & table) const = 0;

  /// default configuration id for some specific subsystem. Is useful, e.g., when reading xml which has no information about subsystem.
  virtual xdata::UnsignedInteger64 defaultIdForSubsystem(const std::string &subsystem) const = 0;


  /// the ID field of a firmware flash times table
  virtual std::string idFieldNameOfFlashTable() const = 0;

  /// the time field of a firmware flash times table
  virtual std::string timeFieldNameOfFlashTable() const = 0;


  /// indicates if the hierarchy cannot be used with DB, but only with XML
  bool notForDB() const {return not_for_db_; }

  /// forbid the use of hierarchy with DB (still can use with XML)
  void setNotForDB(bool flag = true) { not_for_db_ = flag;}


  // ----- printing -----
  
  friend std::ostream& operator<<(std::ostream& os, const ConfigHierarchy& h);

  /// recursive print
  virtual void print(std::ostream & os, const std::string &type, int padding, int verbosity = 0) const;

protected:

  /// table definitions utility
  TableDefinitions *def_;

  /// indicates after which level of configuration tree to truncate the hierarchy
  TruncateLevel truncate_level_;

  /// flag indicating that hierarchy cannot be used with DB
  bool not_for_db_;
};


std::ostream& operator<<(std::ostream& os, const ConfigHierarchy& h);

}}

#endif
