// $Id: ConfigTable.cc,v 1.1 2011/09/09 16:04:43 khotilov Exp $

#include "emu/db/ConfigTable.h"

#include "xdata/TableIterator.h"


namespace emu { namespace db {

using xdata::Table;


ConfigTable::ConfigTable() throw (xdata::exception::Exception)
: Table()
, type_()
, hierarchy_()
, parent_(0)
, children_()
{}


ConfigTable::ConfigTable(const std::string &type, ConfigHierarchy * hierarchy) throw (xdata::exception::Exception)
: Table()
, type_(type)
, hierarchy_(hierarchy)
, parent_(0)
, children_()
{}


ConfigTable::ConfigTable(const ConfigTable & t) throw (xdata::exception::Exception)
{
  this->operator=(t);
}


ConfigTable & ConfigTable::operator=(const ConfigTable & t) throw (xdata::exception::Exception)
{
  this->Table::operator=(t);
  type_ = t.type_;
  hierarchy_ = t.hierarchy_;
  parent_ = t.parent_;
  children_ = t.children_;
  return *this;
}


ConfigTable & ConfigTable::operator=(const Table & t) throw (xdata::exception::Exception)
{
  this->Table::operator=(t);
  return *this;
}


ConfigTable* ConfigTable::findChild(const std::string &type)
{
  for (std::vector<ConfigTable* >::iterator kid = children_.begin(); kid != children_.end(); ++kid)
  {
    if ((*kid)->tableType() == type) return *kid;
  }
  return 0;
}


bool ConfigTable::addChild(ConfigTable* child)
{
  // check if we already have a child table with such type
  const std::string kid_type = child->tableType();
  ConfigTable* check = findChild(kid_type);
  if (check) return 0;

  child->setParent(this);
  children_.push_back(child);
  return true;
}


ConfigTable* ConfigTable::rmChild(const std::string &type)
{
  for (std::vector<ConfigTable* >::iterator kid = children_.begin(); kid != children_.end(); kid++)
  {
    if ( (*kid)->tableType() == type )
    {
      ConfigTable *child = *kid;
      child->setParent(0);
      children_.erase(kid);
      return child;
    }
  }
  return NULL;
}


bool ConfigTable::has(const std::string & columnName) const
{
  if ( columnDefinitions_.find(columnName) != columnDefinitions_.end() ) return true;
  return false;
}


xdata::Table ConfigTable::rowAsTable(size_t rowIndex)
{
  Table one_row_table(getTableDefinition());
  Table::iterator irow = one_row_table.append();
  std::vector<std::string> fields = getColumns();
  for (size_t i = 0; i < fields.size(); i++ )
  {
    one_row_table.setValueAt(0, fields[i], *(getValueAt(rowIndex, fields[i])) );
  }
  return one_row_table;
}


std::ostream& operator<<(std::ostream& os, const ConfigTable& t)
{
  os<<"ConfigTable "<<t.tableName()<<"  ";
  if (t.parent_) os<<"  parent "<<t.parent_->tableName()<<"  ";
  os<<"# children: "<<t.children_.size()<<";  table:   ";
  xdata::Table non_const_t = t;
  non_const_t.writeTo(os);
  return os;
}

}}
