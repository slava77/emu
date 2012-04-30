#ifndef __EMU_DB_ConfigIDInfo_H__
#define __EMU_DB_ConfigIDInfo_H__

#include <string>
#include <iostream>

namespace xdata { class Table; }

namespace emu {	namespace db {

class ConfigHierarchy;


/** @class ConfigIDInfo
 *  A container to pass extra information about a configuration ID.
 */
class ConfigIDInfo
{
public:

  /// default c-tor: initializes everything to empty
  ConfigIDInfo();

  /// full c-tor with already encoded description
  ConfigIDInfo(const std::string &id,
               const std::string &time_write,
               const std::string &time_flash,
               const std::string &description);

  /// full c-tor with separate descriptions
  ConfigIDInfo(const std::string &id,
               const std::string &time_write,
               const std::string &time_flash,
               const std::string &short_description,
               const std::string &long_description);

  /**
   * c-tor from a \c row of xdata::Table \c table.
   * The \c table has to have to have columns with names defined by
   *   hierarchy->idFieldNameOfHead()
   *   hierarchy->timeFieldName()
   *   hierarchy->descriptionFieldName()
   * and, optionally, a column named
   *   hierarchy->timeFieldNameOfFlashTable()
   */
  ConfigIDInfo(xdata::Table &table, size_t row, ConfigHierarchy* hierarchy);


  /** ID value itself */
  const std::string id() const { return id_; }

  /**
   * Time when ID was written into configuration DB (from the top level configuration table)
   */
  const std::string timeOfWrite() const { return time_write_; }

  /**
   * Time when this ID configuration was flash-written into the system (from the flash_write table)
   * Could be empty if a procedure that fills ConfigIDInfo doesn't care about flash info.
   */
  const std::string timeOfFlash() const { return time_flash_; }

  /**
   * Short description.
   * Extracted from the description field of the top level configuration table.
   * It's either contents of this field until the 1st '\n' character or everything if there's no '\n'.
   * The \c max_length parameter allows to limit the short description's length.
   */
  const std::string shortDescription(size_t max_length = 100) const;

  /**
   * Long description.
   * Extracted from the description field of the top level configuration table.
   * It's either contents of this field after the 1st '\n' character or empty if there's no '\n'.
   */
  const std::string longDescription() const;

  /**
   * Full encoded description.
   * Corresponds directly to the description field of the top level configuration table.
   */
  const std::string description() const { return description_; }

  /**
   * Encode short and long descriptions into a single string.
   * \return \c short_description if \c long_description is empty
   * \return \c short_description + '\n' + \c long_description otherwise
   */
  static std::string encodeDescription(const std::string &short_description,
                                       const std::string &long_description);

private:

  std::string id_;
  std::string time_write_;
  std::string time_flash_;
  std::string description_;
};


std::ostream& operator<<(std::ostream& os, const emu::db::ConfigIDInfo& i);


}} // namespaces


#endif
