#ifndef __emu_utils_System_h__
#define __emu_utils_System_h__

#include <string>
#include <vector>


namespace emu { namespace utils {

/**
 * read text file to string
 * @param fileName Name of file to read.
 * @return Content of the file.
 */
std::string readFile(const std::string &fileName);

/**
 * Create a file and write \c content into it.
 * @param fileName Name of file to create.
 * @param content Content of file to write to \c fileName
 */
void writeFile(const std::string &fileName, const std::string &content);

/**
 * Execute the argument in a shell.
 * @param shellCommand
 * @return A vector of lines of the reply.
 */
std::vector< std::string > execShellCommand(const std::string &shellCommand);

/**
 * Have the shell perform expansions (parameter, file name, etc.) on the argument.
 * @param toExpand The string to expand.
 * @return The (parameter, file name, etc.)-expanded string.
 *
 */
std::string performExpansions(const std::string &toExpand);

/**
 * Returns string containing date and time with the following default format:
 *   YYYY-MM-DD HH:MM:SS
 *
 * @param for_file_name  if true, the format would be
 *   YYYY-MM-DD_HH-MM-SS
 */
std::string getDateTime(bool for_file_name = false);

/**
 * Holder for SCSI device information
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
 * @return SCSI_t type structure for a first SCSI device in /proc/scsi/scsi that matches given \c vendor and \c model
 */
SCSI_t getSCSI(const std::string &vendor, const std::string &model);


/**
 * @return device ID string (like /dev/sg0 ) for a device specified by \c scsi
 */
std::string findSCSIDevice( SCSI_t scsi );

}}

#endif
