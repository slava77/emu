// $Id: ConfigRow.cc,v 1.1 2011/09/09 16:04:43 khotilov Exp $

#include "emu/db/ConfigRow.h"
#include "emu/exception/Exception.h"

#include "toolbox/string.h"
#include "xdata/TableIterator.h"


namespace emu
{
namespace db
{

ConfigRow::ConfigRow(const std::string &type, ConfigTable * table, size_t row)
: type_(type)
, table_(table)
, row_(row)
, label_(type)
, parent_(0)
, children_()
{
  // set default label the same as type
  if (label_.length()==0) label_ = type;
}


ConfigRow::ConfigRow(const ConfigRow &r)
{
  type_ = r.type_;
  table_ = r.table_;
  row_ = r.row_;
  label_ = r.label_;
  parent_ = r.parent_;
  children_ = r.children_;
}


xdata::Table ConfigRow::row()
{
  return table_->rowAsTable(row_);
}


xdata::UnsignedInteger64 ConfigRow::id() const
{
  std::string field_name = hierarchy()->idFieldName(type_);
  return table_->getCastValueAt<xdata::UnsignedInteger64>(row_, field_name);
}


xdata::UnsignedInteger64 ConfigRow::parentId() const
{
  std::string field_name = hierarchy()->idFieldNameOfParent(type_);
  return table_->getCastValueAt<xdata::UnsignedInteger64>(row_, field_name);
}


std::vector<ConfigRow* > ConfigRow::findChildren(const std::string &type)
{
  std::vector<ConfigRow* > result;
  // first, check immediate children
  for (std::vector<ConfigRow* >::iterator kid = children_.begin(); kid != children_.end(); ++kid)
  {
    if ((*kid)->type() == type) result.push_back(*kid);
  }
  return result;
}


ConfigRow* ConfigRow::findChild(const std::string &type, xdata::UnsignedInteger64 &id)
{
  for (std::vector<ConfigRow* >::iterator kid = children_.begin(); kid != children_.end(); kid++)
  {
    if ( (*kid)->type() == type && (*kid)->id() == id ) return *kid;
  }
  return NULL;
}


bool ConfigRow::addChild(ConfigRow* child)
{
  // check if we already have a child with such type and ID
  const std::string ch_type = child->type();
  xdata::UnsignedInteger64 ch_id = child->id();
  ConfigRow* check = findChild(ch_type, ch_id);
  if (check) return 0;

  child->setParent(this);
  children_.push_back(child);
  return true;
}


ConfigRow* ConfigRow::rmChild(const std::string &type, xdata::UnsignedInteger64 &id)
{
  for (std::vector<ConfigRow* >::iterator kid = children_.begin(); kid != children_.end(); kid++)
  {
    if ( (*kid)->type() == type && (*kid)->id() == id )
    {
      ConfigRow *child = *kid;
      child->setParent(0);
      children_.erase(kid);
      return child;
    }
  }
  return NULL;
}


bool ConfigRow::has(const std::string & columnName) const
{
  return table_->has(columnName);
}


std::ostream& operator<<(std::ostream& os, const ConfigRow& r)
{
  os<<"ConfigRow "<<r.label_<<": id: "<<r.id().toString()<<" ("<<r.type()<<");  ";
  if (r.parent_) os<<"parent id: "<<r.parent_->id().toString() <<" ("<<r.parent_->type()<<");  ";
  os<<"# children: "<<r.children_.size()<<";  configuration: ";
  xdata::Table t = r.table_->rowAsTable(r.row_);
  t.writeTo(os);
  return os;
}

}}
