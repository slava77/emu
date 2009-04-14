/*****************************************************************************\
* $Id: Exception.h,v 1.2 2009/04/14 17:50:50 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_EXCEPTION_H__
#define __EMU_FED_EXCEPTION_H__

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

namespace emu {
	namespace fed {
		namespace exception {

			/** An exception class that FED objects throw and catch.
			*
			*	@note This is a bogus class that simply gives a name to an extension of
			*	xdaq's xcept::Exception class.  Used in Sentinal interface.
			*
			*	@author Phillip Killewald
			**/
			class Exception: public xcept::Exception
			{
			public:
				Exception(const std::string &name, const std::string &message, const std::string &module, int line, const std::string &function):
				xcept::Exception(name, message, module, line, function)
				{}

				Exception(const std::string &name, const std::string &message, const std::string &module, int line, const std::string &function, xcept::Exception& previous):
				xcept::Exception(name, message, module, line, function, previous)
				{}

				/** Outputs exception in a form that is suitable for javascript parsing **/
				std::string toJSON()
				{
					std::ostringstream out;
					out << "{\"history\":[";
					std::vector<xcept::ExceptionInformation> history = getHistory();
					for (std::vector<xcept::ExceptionInformation>::iterator iError = history.begin(); iError != history.end(); iError++) {
						out << "{";
						std::map<std::string, std::string> messages = iError->getProperties();
						for (std::map<std::string, std::string>::iterator iMessage = messages.begin(); iMessage != messages.end(); iMessage++) {

							out << "\"" << iMessage->first << "\"" << ":";
							out << "\"" << iMessage->second << "\"";
							out << ",";
						}
						// lousy last comma
						out.seekp((long int) out.tellp() - 1);
						out << "}";
					}
					// lousy last comma
					out.seekp((long int) out.tellp() - 1);
					out << "]}";
					return out.str();
				}
			};

///////////////////////////////////////////////////////////////////////////////
// HARDWARE EXCEPTIONS
///////////////////////////////////////////////////////////////////////////////

			/** An exception describing FED hardware problems
			*
			*	@author Phillip Killewald
			**/
			XCEPT_CC(HardwareException, emu::fed::exception::Exception);

			XCEPT_CC(CAENException, HardwareException);

			XCEPT_CC(DDUException, HardwareException);

			XCEPT_CC(DCCException, HardwareException);

///////////////////////////////////////////////////////////////////////////////
// SOFTWARE EXCEPTIONS
///////////////////////////////////////////////////////////////////////////////

			/** An exception describing FED software problems
			*
			*	@author Phillip Killewald
			**/
			XCEPT_CC(SoftwareException, emu::fed::exception::Exception);

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
		}
	}
}

#endif
