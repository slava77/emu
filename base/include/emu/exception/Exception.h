#ifndef __EMU_EXCEPTION_H__
#define __EMU_EXCEPTION_H__

#include "xcept/Exception.h"
#include <string>
#include <sstream>
#include <vector>
#include <map>

// Macro for easily generating exceptions that are carbon-copies of their parents
#define XCEPT_CC( TO, FROM ) \
class TO: public FROM\
{ \
public: \
  TO(const std::string &name, const std::string &message, const std::string &module, int line, const std::string &function): \
  FROM(name, message, module, line, function) \
  {} \
\
  TO(const std::string &name, const std::string &message, const std::string &module, int line, const std::string &function, xcept::Exception& previous): \
  FROM(name, message, module, line, function, previous) \
  {} \
}

namespace emu
{
namespace exception
{

/** An exception class that EMU objects throw and catch.
 *
 *	@note This is a bogus class that simply gives a name to an extension of xdaq's xcept::Exception class.
 *
 **/
class Exception : public xcept::Exception
{
public:
  Exception(const std::string &name, const std::string &message, const std::string &module, int line, const std::string &function)
  : xcept::Exception(name, message, module, line, function)
  {}

  Exception(const std::string &name, const std::string &message, const std::string &module, int line, const std::string &function, xcept::Exception& previous)
  : xcept::Exception(name, message, module, line, function, previous)
  {}

  /**
   * Outputs exception in a form that is suitable for javascript parsing
   */
  std::string toJSON()
  {
    std::ostringstream out;
    out << "{\"history\":[";
    std::vector< xcept::ExceptionInformation > history = getHistory();
    for (std::vector< xcept::ExceptionInformation >::iterator iError = history.begin(); iError != history.end(); iError++)
    {
      out << "{";
      std::map< std::string, std::string > messages = iError->getProperties();
      for (std::map< std::string, std::string >::iterator iMessage = messages.begin(); iMessage != messages.end(); iMessage++)
      {
        out << "\"" << iMessage->first << "\"" << ":";
        out << "\"" << iMessage->second << "\"";
        out << ",";
      }
      // lousy last comma
      out.seekp((size_t) out.tellp() - 1);
      out << "}";
    }
    // lousy last comma
    out.seekp((size_t) out.tellp() - 1);
    out << "]}";
    return out.str();
  }

  /**
   * Outputs exception in an HTML form
   */
  std::string toHTML()
  {
    std::ostringstream out;

    out << "<table class=\"Xcept\">";
    out << "<tr><th colspan=\"3\">Exception stack</th></tr>";
    out << "<tr><th style=\"width: 1%;\">#</th><th style=\"text-align: left;\">type</th><th style=\"text-align: left;\">message</th><th style=\"text-align: left;\">raised at</th></tr>";

    std::vector<xcept::ExceptionInformation> & history = getHistory();
    std::vector<xcept::ExceptionInformation>::reverse_iterator i = history.rbegin();
    int iLayer = history.size();
    while ( i != history.rend() )
    {
      out << "<tr>";
      out << "<td style=\"width: 1%;background-color: #dddddd;\">" << iLayer << "</td>";
      out << "<td style=\"text-align: left;\">" << i->getProperty("identifier") << "</td>";
      out << "<td style=\"background-color: #ff5533;\">" << i->getProperty("message") << "</td>";
      out << "<td style=\"text-align: left;\">" << i->getProperty("function") << " (" << i->getProperty("module") << ":" << i->getProperty("line") << ")</td>";
      out << "</tr>";
      ++i;
      --iLayer;
    }
    out << "</table>";

    return out.str();
  }
};

///////////////////////////////////////////////////////////////////////////////
// HARDWARE EXCEPTIONS
///////////////////////////////////////////////////////////////////////////////

/** An exception describing EMU hardware problems **/
XCEPT_CC(HardwareException, emu::exception::Exception);

XCEPT_CC(CAENException, HardwareException);
XCEPT_CC(DDUException, HardwareException);
XCEPT_CC(DCCException, HardwareException);

XCEPT_CC(VCCException, HardwareException);
XCEPT_CC(CSCException, HardwareException);
XCEPT_CC(TMBException, HardwareException);
XCEPT_CC(DMBException, HardwareException);
XCEPT_CC(CCBException, HardwareException);
XCEPT_CC(MPCException, HardwareException);
XCEPT_CC(CFEBException, HardwareException);
XCEPT_CC(ALCTException, HardwareException);
XCEPT_CC(AnodeChannelException, HardwareException);

///////////////////////////////////////////////////////////////////////////////
// SOFTWARE EXCEPTIONS
///////////////////////////////////////////////////////////////////////////////

/** An exception describing EMU software problems **/
XCEPT_CC(SoftwareException, emu::exception::Exception);

XCEPT_CC(OutOfBoundsException, SoftwareException);

XCEPT_CC(DevTypeException, OutOfBoundsException);

XCEPT_CC(ConfigurationException, SoftwareException);

XCEPT_CC(FileException, SoftwareException);

XCEPT_CC(UndefinedException, SoftwareException);

XCEPT_CC(ParseException, SoftwareException);

XCEPT_CC(XMLException, ParseException);

XCEPT_CC(FSMException, SoftwareException);

XCEPT_CC(TTSException, SoftwareException);

XCEPT_CC(SOAPException, SoftwareException);

XCEPT_CC(FMMThreadException, SoftwareException);

XCEPT_CC(CGIException, SoftwareException);

XCEPT_CC(DBException, SoftwareException);

XCEPT_CC(FirmwareException, SoftwareException);

}
}

#endif
