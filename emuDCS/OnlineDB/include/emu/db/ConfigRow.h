#ifndef __EMU_DB_CONFIGROW_H__
#define __EMU_DB_CONFIGROW_H__

#include "emu/db/ConfigTable.h"
#include "emu/db/ConfigHierarchy.h"
#include "emu/exception/Exception.h"

#include "xdata/Table.h"
#include "xdata/UnsignedInteger64.h"

#include <vector>
#include <string>


namespace emu { namespace db
{

/** @class ConfigRow
 * Configuration data unit that maps onto a row of configuration data represented as xdata::Table.
 * It provides some access utilities and knows about parent and children relationships.
 * Its primary intended use is as a node in a ConfigTree.
 *
 * As xdata::Table data is stored by columns, and xdata::Table::Row is accessible
 * only through a transient iterator, we need to store a row number and to access data through it.
 **/
class ConfigRow
{
public:

  /** Constructors
  * @param type        table type, usually defined by table name without a prefix (VCC, TMB, ALCT, etc.)
  * @param table       pointer to the corresponding configuration xdata::Table
  * @param row         number of a row in the above table which contains node's configuration information
  */
  ConfigRow(const std::string &type, ConfigTable * table, size_t row);
  
  /// copy constructor
  ConfigRow(const ConfigRow& r);


  // ----- basic accessors and modifiers: -----

  const std::string type() const {return type_;}

  xdata::Table row();
  //void setRow(xdata::Table::Row * row) {row_ = row;}
  
  ConfigHierarchy * hierarchy() {return table_->hierarchy();}
  const ConfigHierarchy * hierarchy() const {return table_->hierarchy();}

  ConfigRow * parent() {return parent_;}

  /// human friendly label
  std::string label() const {return label_;}

  /// set human friendly label
  void setLabel(const std::string &label) {label_ = label;}
  
  /// retrieve all 1st degree children
  std::vector<ConfigRow* > children() {return children_;}
  
  // ----- more involved accessors and modifiers: -----

  /// row's own ID
  xdata::UnsignedInteger64 id() const;

  /// parent's ID
  xdata::UnsignedInteger64 parentId() const;

  /// find all 1st degree children of certain type
  std::vector<ConfigRow* > findChildren(const std::string &type);

  /// find a 1st degree child by its type and ID
  ConfigRow* findChild(const std::string &type, xdata::UnsignedInteger64 &id);

  /// add a child.
  /// NOTE: the method also sets child's parent to this object
  /// \return false if node already has a child with the same type and id
  bool addChild(ConfigRow* child);

  /// set a ponter to a parent
  /// NOTE: it DOES NOT add this object as a child to the parent in order to avoid circular dependency!
  void setParent(ConfigRow* parent) { parent_ = parent;}

  /// remove a child from the list of children
  /// NOTE: the method also sets child's parent to NULL
  /// \return  a pointer to removed child or a zero pointer (if child was not found)
  ConfigRow* rmChild(const std::string &type, xdata::UnsignedInteger64 &id);

  /// check if it has a column named columnName (case non-sensitive)
  bool has(const std::string & columnName) const;

  /// specialized getValueAt that allows to get known type values by specifying types
  /// e.g., getCastValue<xdata::UnsignedInteger64>("emu_config_id")
  template< class T >
  T getCastValue(const std::string & columnName) throw (emu::exception::ConfigurationException);

  // ----- printing -----
  friend std::ostream& operator<<(std::ostream& os, const ConfigRow& node);

protected:

  /// table type (vcc, tmb, alct, etc.)
  std::string type_;
  
  /// pointer to the data table
  ConfigTable* table_;

  /// table row number that contains the configuration entry
  size_t row_;

  /// human friendly label of this device
  std::string label_;

  /// parent of this device in a system tree structure
  ConfigRow* parent_;

  /// children of this device in a system tree structure
  std::vector<ConfigRow* > children_;
};


std::ostream& operator<<(std::ostream& os, const ConfigRow& node);

template< class T >
T ConfigRow::getCastValue(const std::string & columnName) throw (emu::exception::ConfigurationException)
{
  T result;
  try
  {
    result = table_->getCastValueAt<T>(row_, columnName);
  }
  catch (emu::exception::SoftwareException &e)
  {
    XCEPT_RETHROW(emu::exception::SoftwareException, "Error in getCastValueAt(row, "+columnName+")", e);
  }
  return result;
}

}}

#endif
