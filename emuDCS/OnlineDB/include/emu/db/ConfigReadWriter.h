#ifndef __EMU_DB_CONFIGREADWRITER_H__
#define __EMU_DB_CONFIGREADWRITER_H__

#include <map>
#include <string>

#include "emu/db/ConfigTable.h"
#include "emu/db/ConfigTree.h"
#include "emu/exception/Exception.h"

#include "xdata/UnsignedInteger64.h"

namespace emu {	namespace db {
	
/** @class ConfigReadWriter
 *  An interface class for reading and writing configuration data
 */
class ConfigReadWriter
{
public:
  /// Default constructor
  ConfigReadWriter() : datamap_(), hierarchy_(0) {};

  /// Default destructor
  virtual ~ConfigReadWriter() {};

  /** Read the configuration
   * @param id    The ID of configuration to read.
   */
  virtual bool read(xdata::UnsignedInteger64 id) throw (emu::exception::ConfigurationException) = 0;

  /** Write the configuration
   * \param data  a pointer to the ConfigTables map to write; if NULL, writes the current datamap_
   */
  virtual bool write(std::map<std::string, ConfigTable> *data = 0) throw (emu::exception::ConfigurationException) = 0;

  /** Write the configuration
   * \param tree  a pointer to the ConfigTree to write; if NULL, writes information from the current datamap_
   */
  virtual bool write(ConfigTree *tree) throw (emu::exception::ConfigurationException) = 0;

  /// a reference to ConfigTables map
  inline std::map<std::string, ConfigTable> & configTables() { return datamap_; }

  /// pointer to the hierarchy object
  const ConfigHierarchy * hierarchy() const {return hierarchy_;}

protected:

  /// configuration tables map accessible by a table type key
  std::map<std::string, ConfigTable> datamap_;

  /// pointer to a system hierarchy object
  ConfigHierarchy* hierarchy_;
};

}}

#endif
