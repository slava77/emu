#ifndef __EMU_DB_TSTORECOMMAND_H__
#define __EMU_DB_TSTORECOMMAND_H__

#include "xdaq/Application.h"
#include "xoap/MessageReference.h"
#include "xdata/String.h"
#include "xdata/Table.h"
#include "xdata/exdr/AutoSizeOutputStreamBuffer.h"

#include "emu/soap/QualifiedName.h"
#include "emu/soap/Attachment.h"
#include "emu/exception/Exception.h"

namespace emu {	namespace db {

/** @class TStoreCommand
 * a utility class that wraps the emu::soap::Messenger functionality to easily send commands to TStore via SOAP.
 *
 * There are two types of parameters:
 *   command parameters        which are parameters specific to TStore commands
 *   view-specific parameters  which are passed to TStore SQL views
 *
 * This code used the original TStoreRequest class as a base.
 */
class TStoreCommand
{
public:
  /** Constructor.
   * @application         the pointer to the Application which sends the message
   * @param command_name  the name of the command to send TStore.
   * @param view_id       the TStore SQL view class id, e.g., "urn:tstore-view-SQL:EMUsystem"
   **/
  TStoreCommand(xdaq::Application *application, const std::string &command_name, const std::string &view_id = "");

  /** Add a command parameter to the request with a name and a value.
   * @param parameter_name   the parameter name to add.
   * @param parameter_value  the value of the added parameter in string form.
   */
  void addCommandParameter(const std::string &parameter_name, const std::string &parameter_value);

  /** Add an SQL view-specific parameter to the request with a name and a value.
   * @param parameter_name   the parameter name to add.
   * @param parameter_value  the value of the added parameter in string form.
   */
  void addViewSpecificParameter(const std::string &parameter_name, const std::string &parameter_value) throw (emu::exception::SOAPException);

  /** Make a soap attachment out of data. This is necessary for using the TStore "insert" operation.
   * Note that while a soap message may have several attachments, we need just one for the "insert" operation.
   * @param data     the Table to write
   */
  void setAttachment(xdata::Table &data) throw (emu::exception::SOAPException);

  /** Send the command to TStore and receives the result
   * @param instance   the TStore application instance number
   */
  xoap::MessageReference run(const int instance = 0) throw (emu::exception::SOAPException);

private:

  /// The SQL view class (obtained from view ID)
  std::string viewClass_;

  /// Holder for the qualified command name
  emu::soap::QualifiedName qualifiedCommandName_;

  /// The Application which sends the message
  xdaq::Application *application_;

  /// The command parameters, stored in a name:value map
  std::map< const std::string, xdata::String > commandParameters_;

  /// The view-specific parameters, stored in a name:value map
  std::map< const std::string, xdata::String > viewSpecificParameters_;

  /// we might have just a single attachment, but let's keep it in a vector for convenience reasons
  std::vector<emu::soap::Attachment> attachment_;

  /// exdr buffer to keep the attachment data
  xdata::exdr::AutoSizeOutputStreamBuffer exdrBuffer_;
};

}}

#endif
