#ifndef __EMU_DB_CONFIGTABLE_H__
#define __EMU_DB_CONFIGTABLE_H__

#include "xdata/Table.h"
#include "toolbox/string.h"

#include "emu/db/ConfigHierarchy.h"

#include <string>


namespace emu { namespace db {

//using xdata::Table;

/** @class ConfigTable
 * Extension of xdata::Table.
 * It allows to assign type and hierarchy properties, and to easily access rows.
 *
 * Note that the original xdata::Table stores its data by columns and its xdata::Table::Row* is accessible
 * only through a transient iterator.
 **/
class ConfigTable : public xdata::Table
{
public:

  /// trivial constructor
  ConfigTable() throw (xdata::exception::Exception);

  /** constructor that sets some additional properties
  * @param type        table type; usually defined by table name without prefix (VCC, TMB, ALCT, etc.)
  * @param hierarchy   object containing hierarchical relationships and table definitions information for configuration (has to be non-zero pointer!)
  */
  ConfigTable(const std::string &type, ConfigHierarchy * hierarchy) throw (xdata::exception::Exception);

  /// Copy constructor creates a new table from another table with a deep copy
  ConfigTable(const ConfigTable & t) throw (xdata::exception::Exception);

  virtual ~ConfigTable() {};

  ConfigTable & operator=(const ConfigTable & b) throw (xdata::exception::Exception);
  ConfigTable & operator=(const Table & b) throw (xdata::exception::Exception);


  /// hierarchy getter & setter
  ConfigHierarchy * hierarchy() {return hierarchy_;}
  void setHierarchy (ConfigHierarchy * hierarchy) {hierarchy_ = hierarchy;}

  /// table type getter & setter
  const std::string tableType() const {return type_;}
  void setTableType(const std::string &type) {type_ = type;}

  /// table prefix
  const std::string tablePrefix() const {return hierarchy_->prefix();}

  /// table name
  const std::string tableName() const {return hierarchy_->prefix() + type_;}

  /// parent table
  ConfigTable * parent() {return parent_;}

  /// retrieve all 1st degree children tables
  std::vector<ConfigTable* > children() {return children_;}
  
  /// find a child of certain type
  ConfigTable*  findChild(const std::string &type);

  /// set a ponter to a parent
  /// NOTE: it DOES NOT add this object as a child to the parent in order to avoid circular dependency!
  void setParent(ConfigTable* parent) { parent_ = parent;}

  /// add a child.
  /// NOTE: the method also sets child's parent to this object
  /// \return  false if a child of such type already exists
  bool addChild(ConfigTable* child);

  /// remove a child from the list of children
  /// NOTE: the method also sets child's parent to NULL
  /// \return  a pointer to removed child or a zero pointer (if child was not found)
  ConfigTable* rmChild(const std::string &type);

  /// check if it has a column named columnName (case non-sensitive)
  bool has(const std::string & columnName) const;

  // ----- special row/field accessors

  /// specialized getValueAt that allows to get known type values by specifying types
  /// e.g., getCastValueAt<xdata::UnsignedInteger64>(0, "emu_config_id")
  /// column name is not case sensitive
  template< class T >
  T getCastValueAt(size_t rowIndex, const std::string & columnName) throw (emu::exception::SoftwareException);

  /// get row as a Table
  xdata::Table rowAsTable(size_t rowIndex);

  // ----- printing -----
  friend std::ostream& operator<<(std::ostream& os, const ConfigTable& node);

protected:

  /// table type (vcc, tmb, alct, etc.)
  std::string type_;
  
  /// pointer to the configuration object
  ConfigHierarchy * hierarchy_;

  /// parent of this ConfigTable in a configuration tables tree structure
  ConfigTable* parent_;

  /// children of this table in a configuration tables tree structure
  std::vector<ConfigTable* > children_;
};

std::ostream& operator<<(std::ostream& os, const ConfigTable & t);


template< class T >
T ConfigTable::getCastValueAt(size_t rowIndex, const std::string & columnName) throw (emu::exception::SoftwareException)
{
  // first, try to do a non-case sensitive column name match:
  std::string column_name = columnName;
  std::vector<std::string> columns = getColumns();
  for (std::vector<std::string>::iterator column=columns.begin(); column!=columns.end(); ++column)
  {
    if (toolbox::tolower(*column) == toolbox::tolower(columnName))
    {
      column_name = *column;
      break;
    }
  }
  if (column_name.empty())
  {
    XCEPT_RAISE(emu::exception::SoftwareException, "Table doesn't have a column with name "+columnName);
  }

  xdata::Serializable *svalue;
  try
  {
    svalue = getValueAt(rowIndex, column_name);
  }
  catch (xdata::exception::Exception &e)
  {
    XCEPT_RETHROW(emu::exception::SoftwareException, "Can't read column "+column_name, e);
  }
  if (!svalue)
  {
    XCEPT_RAISE(emu::exception::SoftwareException, "Can't convert NULL value to anything");
  }
  T result;
  if (svalue->type() != result.type())
  {
    XCEPT_RAISE(emu::exception::SoftwareException, "Can't convert "+svalue->type()+" value to "+result.type());
  }
  try
  {
    result.setValue(*svalue);
  }
  catch (std::bad_cast &e)
  {
    XCEPT_RAISE(emu::exception::SoftwareException,
                "Can't copy one "+svalue->type()+" to another. Maybe two xdata types return the same value from type()");
  }
  catch (std::exception &e)
  {
    XCEPT_RAISE(emu::exception::SoftwareException, "Can't copy one "+svalue->type()+" to another. "+e.what());
  }
  return result;
}

}}

#endif
