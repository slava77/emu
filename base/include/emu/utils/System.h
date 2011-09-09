#ifndef __emu_utils_System_h__
#define __emu_utils_System_h__

#include "log4cplus/logger.h"
#include "xcept/Exception.h"

#include <string>
#include <vector>


namespace emu { namespace utils {

/**
 * read text file to string
 */
std::string readFile(const std::string fileName) throw (xcept::Exception);

/**
 * write string to text file
 */
void writeFile(const std::string fileName, const std::string content) throw (xcept::Exception);

/**
 *
 */
std::vector< std::string > execShellCommand(const std::string shellCommand) throw (xcept::Exception);

/**
 * Gets UTC date and time.
 * @return UTC date and time in YYYYMMDD_hhmmss_UTC format
 */
std::string getDateTime();

/**
 *
 */
struct SCSI_t
{
  int host;
  int channel;
  int id;
  int lun;
  std::string vendor;
  std::string model;
  std::string revision;
};

/**
 *
 */
SCSI_t getSCSI(const std::string vendor, const std::string model);

}}

#endif
