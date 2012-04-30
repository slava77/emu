// $Id: ConfigIDInfo.cc,v 1.1 2012/04/30 23:46:30 khotilov Exp $

#include "emu/db/ConfigIDInfo.h"
#include "emu/db/ConfigHierarchy.h"
#include "emu/exception/Exception.h"
#include "xdata/Table.h"
#include "toolbox/string.h"

#include <sstream>
#include <algorithm>


namespace emu {	namespace db {


ConfigIDInfo::ConfigIDInfo() {}


ConfigIDInfo::ConfigIDInfo(const std::string &id,
               const std::string &time_write,
               const std::string &time_flash,
               const std::string &description)
: id_(id)
, time_write_(time_write)
, time_flash_(time_flash)
, description_(description)
{}


ConfigIDInfo::ConfigIDInfo(const std::string &id,
               const std::string &time_write,
               const std::string &time_flash,
               const std::string &short_description,
               const std::string &long_description)
: id_(id)
, time_write_(time_write)
, time_flash_(time_flash)
{
  description_ = encodeDescription(short_description, long_description);
}


ConfigIDInfo::ConfigIDInfo(xdata::Table &table, size_t row, ConfigHierarchy* hierarchy)
{
  std::string id_field_name = toolbox::toupper(hierarchy->idFieldNameOfHead());
  std::string time_write_field_name = toolbox::toupper(hierarchy->timeFieldName());
  std::string time_flash_field_name = toolbox::toupper(hierarchy->timeFieldNameOfFlashTable());
  std::string description_field_name = toolbox::toupper(hierarchy->descriptionFieldName());

  std::vector<std::string> columns =  table.getColumns();
  bool has_id = ( std::count(columns.begin(), columns.end(), id_field_name) == 1 );
  bool has_time_write = ( std::count(columns.begin(), columns.end(), time_write_field_name) == 1 );
  bool has_time_flash = ( std::count(columns.begin(), columns.end(), time_flash_field_name) == 1 );
  bool has_description = ( std::count(columns.begin(), columns.end(), description_field_name) == 1 );

  if ( !(has_id && has_time_write && has_description) )
  {
    std::ostringstream error;
    error << __func__ << " constructor was supplied with xdata::Table that is missing required columns: " << std::endl;
    error << "has_id = " << has_id << std::endl;
    error << "has_time_write = " << has_time_write << std::endl;
    error << "has_description = " << has_description << std::endl;
    XCEPT_RAISE(emu::exception::ConfigurationException, error.str() );
  }

  if (table.getRowCount() < row + 1)
  {
    std::ostringstream error;
    error << __func__ << " constructor has out of bounds row number " << row
        << ". Table has " << table.getRowCount() << " rows." << std::endl;
    XCEPT_RAISE(emu::exception::ConfigurationException, error.str() );
  }

  id_ = table.getValueAt(row, id_field_name)->toString();
  time_write_ = table.getValueAt(row, time_write_field_name)->toString();
  if (has_time_flash) time_flash_ = table.getValueAt(row, time_flash_field_name)->toString();
  description_ = table.getValueAt(row, description_field_name)->toString();
}


const std::string ConfigIDInfo::shortDescription(size_t max_length) const
{
  size_t newline = description_.find('\n');
  if (newline <= max_length) max_length = newline;
  return description_.substr(0, max_length);
}


const std::string ConfigIDInfo::longDescription() const
{
  size_t newline = description_.find('\n');
  if (newline == std::string::npos) return "";
  return description_.substr(newline + 1);
}


std::string ConfigIDInfo::encodeDescription(const std::string &short_description,
                                            const std::string &long_description)
{
  if (long_description.empty()) return short_description;
  return short_description + '\n' + long_description;
}


std::ostream& operator<<(std::ostream& os, const emu::db::ConfigIDInfo& i)
{
  os << "ID: " << i.id() << std::endl;
  os << "  written: " << i.timeOfWrite() << std::endl;
  if (! i.timeOfFlash().empty())
  {
    os << "  flashed: " << i.timeOfFlash() << std::endl;
  }
  os << "  desc. short: " << i.shortDescription() << std::endl;
  os << "  desc. long : " << i.longDescription() << std::endl;
  return os;
}


}} // namespaces
