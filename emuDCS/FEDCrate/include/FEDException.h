/*****************************************************************************\
* $Id: FEDException.h,v 1.5 2009/01/29 15:31:22 paste Exp $
*
* $Log: FEDException.h,v $
* Revision 1.5  2009/01/29 15:31:22  paste
* Massive update to properly throw and catch exceptions, improve documentation, deploy new namespaces, and prepare for Sentinel messaging.
*
* Revision 1.4  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#ifndef __FEDEXCEPTION_H__
#define __FEDEXCEPTION_H__

#include "xcept/Exception.h"
#include <string>

// Macro for easily generating exceptions that are carbon-copies of their parents
#define XCEPT_CC( TO, FROM ) \
class TO: public FROM\
{ \
public: \
	TO(const std::string &name, const std::string &message, const std::string &module, int line, const std::string &function): \
		FROM(name, message, module, line, function) \
	{}; \
\
	TO(const std::string &name, const std::string &message, const std::string &module, int line, const std::string &function, xcept::Exception& previous): \
		FROM(name, message, module, line, function, previous) \
	{}; \
};

namespace emu {
	namespace fed {
	
		/** An exception class that FED objects throw and catch.
		*
		*	@note This is a bogus class that simply gives a name to an extension of
		*	xdaq's xcept::Exception class.  Used in Sentinal interface.
		*
		*	@author Phillip Killewald
		**/
		XCEPT_CC(Exception, xcept::Exception);

///////////////////////////////////////////////////////////////////////////////
// HARDWARE EXCEPTIONS
///////////////////////////////////////////////////////////////////////////////

		/** An exception describing FED hardware problems
		*
		*	@author Phillip Killewald
		**/
		XCEPT_CC(HardwareException, emu::fed::Exception);

		XCEPT_CC(CAENException, HardwareException);

///////////////////////////////////////////////////////////////////////////////
// SOFTWARE EXCEPTIONS
///////////////////////////////////////////////////////////////////////////////

		/** An exception describing FED software problems
		*
		*	@author Phillip Killewald
		**/
		XCEPT_CC(SoftwareException, emu::fed::Exception);

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

#endif
