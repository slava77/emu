#ifndef __EMU_DB_PCCONFIGHIERARCHY_H__
#define __EMU_DB_PCCONFIGHIERARCHY_H__

#include "emu/db/ConfigHierarchy.h"

#include <vector>
#include <map>
#include <string>

namespace emu { namespace db
{

/** @class PCConfigHierarchy
 * Implementation of ConfigHierarchy deta structure for the Peripheral Crates system.
 **/
class PCConfigHierarchy: public ConfigHierarchy
{
public:

  /** Constructors
  * @param tstore_conf     location of the TStore view configuration file)
  * @param truncate_level  indicates after what level of configuration tree to truncate the hierarchy.
  *                        E.g., level TRUNCATE_0 would include only Configuration table,
  *                        level TRUNCATE_1 would include Configuration and PeripheralCrate tables, etc.
  */
  PCConfigHierarchy(const std::string &tstore_conf, TruncateLevel truncate_level = NOT_TRUNCATE);

  /// Constructor that does not read in table definitions
  PCConfigHierarchy();

  ~PCConfigHierarchy();

  // ----- accessors: -----
  
  /// the tables' prefix (DB table name = prefix + type)
  std::string prefix() const;

  /// all types (table name without prefix) for the system
  std::vector<std::string> types() const {return types_;}
  
  /// the type of the head of configuration tree
  std::string typeOfHead() const {return head_;}
  
  /// the type of the head of configuration tree in XML file (it's usually different for some reason)
  std::string typeOfHeadXML() const {return head_xml_;}

  /// types of children of a certain type
  std::vector<std::string> typesOfChildern(const std::string &type) const;
  
  /// the type of a parent of a certain type
  std::string typeOfParent(const std::string &type) const;
  
  /// the type of table that keeps firmware flash info (it's separate from the rest of config tree hierarchy)
  std::string typeOfFlashTable() const {return typeOfFlashTable_;}

  /// the top level/head configuration ID field name (is expected to be the same in all tables)
  std::string idFieldNameOfHead() const {return idFieldNameHead_;}

  /// the ID field name for a certain type
  std::string idFieldName(const std::string &type) const;

  /// the ID field name of parent for a certain type
  std::string idFieldNameOfParent(const std::string &type) const;

  /// the DB write time field name (this is a field in the head table)
  std::string timeFieldName() const {return timeFieldName_;}

  /// name of the description field name (this is a field in the head table)
  std::string descriptionFieldName() const { return descriptionFieldName_; }

  /** returns an assembled tree of empty but properly defined ConfigTables
   * that already have all the parent-children relationship hierarchically set up
   */
  std::map<std::string, ConfigTable> definitionsTree();

  /// constructs configuration row's ID that depends on table type by using parent's ID and some provided number
  xdata::UnsignedInteger64 id(const std::string &type, xdata::UnsignedInteger64 &parent_id, xdata::UnsignedInteger64 &number);

  /// checks if a filed is used only in DB
  bool isFieldDBOnly(const std::string &type, const std::string &field_name) const;

  /// update config ID part of id_to_update with new_config_id
  xdata::UnsignedInteger64 updateConfigId(xdata::UnsignedInteger64 &id_to_update, xdata::UnsignedInteger64 &new_config_id) const;

  /// the subsystem (endcap side) of a non-empty config table (returns empty string from an empty table)
  std::string determineSubSystem(ConfigTable & table) const;

  /// default configuration id for subsystem: 1000000 for "plus", 2000000 for "minus" endcap side.
  /// Is useful, e.g., when reading older xml's which have no information about the endcap side.
  xdata::UnsignedInteger64 defaultIdForSubsystem(const std::string &subsystem) const;


  /// the ID field of a firmware flash times table
  std::string idFieldNameOfFlashTable() const {return idFieldNameOfFlashTable_;}

  /// the time field of a firmware flash times table
  std::string timeFieldNameOfFlashTable() const {return timeFieldNameOfFlashTable_;}


protected:

  // hierarchy data
  // TODO: change maps to faster unordered_maps after full migration to gcc4
  static std::string prefix_;
  static std::string head_;
  static std::string head_xml_;
  static std::vector<std::string> types_;
  static std::map<std::string, std::vector<std::string> > typeChildren_;
  static std::map<std::string, std::string > typeParent_;
  static std::string idFieldNameHead_;
  static std::map<std::string, std::string > idFieldNames_;
  static std::string timeFieldName_;
  static std::string descriptionFieldName_;

  static std::string typeOfFlashTable_;
  static std::string idFieldNameOfFlashTable_;
  static std::string timeFieldNameOfFlashTable_;

private:

  /// hierarchy data initialization
  /// TODO: possibly, might add an option for a separate hierarchy configuration xml, instead of hardcoding it
  void init();
  void addChildTypeName(const std::string &parent, const std::string &child);

  /// a helper for the definitionsTree method
  void definitionsTreeRecursive(const std::string &type, std::map<std::string, ConfigTable> &tree);

  /// if the hierarchy has DDU type of boards in it (for test stand)
  bool has_DDU_;
};

}}

#endif
