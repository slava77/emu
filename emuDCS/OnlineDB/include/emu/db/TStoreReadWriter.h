#ifndef __EMU_DB_TSTOREREADWRITER_H__
#define __EMU_DB_TSTOREREADWRITER_H__

#include <map>
#include <string>

#include "emu/db/ConfigReadWriter.h"
#include "emu/exception/Exception.h"

#include "xdaq/Application.h"
#include "xdata/Integer.h"

namespace emu {	namespace db {
	
/** @class TStoreReadWriter
 *  An implementation of TStore reader/writer
 */
class TStoreReadWriter : public ConfigReadWriter
{
public:

  /** Constructor
   * @hierarchy            Pointer to the object describing tables hierarchy.
   * @param application    The application doing the configuring. Required because the class needs to perform SOAP communication to TStore.
   * @param db_credentials The database credentials (in the "username/password" format).
   * @instance             The instance of tstore (-1 means find first available)
   **/
  TStoreReadWriter(ConfigHierarchy* hierarchy, xdaq::Application *application, const std::string &db_credentials, int instance = 0);

  /// Default destructor
  ~TStoreReadWriter() {};

  /** Read a vector of configuration ids (maximum of 50 last ids)
   * @param subsystem   e.g., for PCrates, the subsystem = endcap side  and it has to be set
   */
  std::vector<xdata::UnsignedInteger64> readIDs(const std::string &subsystem = "") throw (emu::exception::ConfigurationException);

  /** Read the last configuration ID
   * @param subsystem   e.g., for PCrates, the subsystem = endcap side  and it has to be set
   */
  xdata::UnsignedInteger64 readMaxID(const std::string &subsystem = "") throw (emu::exception::ConfigurationException);

  /** Read the configuration
   * @param id    The ID of configuration to read; it HAS to be non-zero for TStoreReadWriter
   */
  bool read(xdata::UnsignedInteger64 id) throw (emu::exception::ConfigurationException);

  /** Write the configuration
   * \param data  a pointer to the ConfigTables map to write; if NULL, writes the current datamap_
   */
  bool write(std::map<std::string, ConfigTable> *data = 0) throw (emu::exception::ConfigurationException);

  /** Write the configuration
   * \param tree  a pointer to the ConfigTree to write; if NULL, writes writes information from the current datamap_
   */
  bool write(ConfigTree *tree) throw (emu::exception::ConfigurationException);

  // ********  ********

  /** Read the last used (flashed) configuration ID key from a designated DB table
   * @param subsystem   e.g., for PCrates, the subsystem = endcap side  and it has to be set
   */
  xdata::UnsignedInteger64 readLastConfigIdFlashed(const std::string &subsystem = "") throw (emu::exception::ConfigurationException);

  /** Writes the time of configuration flashing (in fact, the time when it is called) into a designated DB table
   * \param ID     the DB key of configuration that was flashed
   */
  bool writeFlashTime(xdata::UnsignedInteger64 id) throw (emu::exception::ConfigurationException);

private:

  /// shortcut type: key is column name, value is column type
  typedef std::map<std::string, std::string, xdata::Table::ci_less> DefinitionType;

  /// The application from where to send SOAP messages for communication
  xdaq::Application *application_;

  /// The database credentials
  std::string db_credentials_;

  /// The TStore instance to use
  int instance_;
};

}}

#endif
