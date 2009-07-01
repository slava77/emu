/*****************************************************************************\
* $Id: Supervised.h,v 1.4 2009/07/01 14:54:03 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_SUPERVISED_H__
#define __EMU_FED_SUPERVISED_H__

#include "emu/base/Supervised.h"
#include "emu/fed/Exception.h"

#include <string>
#include <vector>

#include "xoap/DOMParser.h"
#include "xoap/MessageReference.h"
#include "xdata/soap/Serializer.h"
#include "xoap/SOAPName.h"
#include "xdata/xdata.h"
#include "xdaq/NamespaceURI.h"
#include "xdata/soap/NamespaceURI.h"
#include "xoap/domutils.h"
#include "xgi/Input.h"
#include "xoap/Method.h"

#define DEFINE_DEFAULT_SOAP2FSM_ACTION( COMMAND ) \
xoap::MessageReference on ##COMMAND(xoap::MessageReference message) \
{ \
	LOG4CPLUS_DEBUG(getApplicationLogger(), "FSM state change requested:  " #COMMAND); \
	fireEvent(#COMMAND); \
	return createReply(message); \
}

#define BIND_DEFAULT_SOAP2FSM_ACTION( CLASS, COMMAND ) xoap::bind(this, &CLASS::on ##COMMAND, #COMMAND, XDAQ_NS_URI);

namespace emu {
	namespace fed {
		
		/** @class Supervised A class from which all FED XDAQ applications that are managed by the Supervisor (i.e., have a finite state machine) should inherit.
		*	Includes routines for FSM manipulation.
		*
		*	@author Phillip Killewald &lt;paste@mps.ohio-state.edu&gt;
		**/
		class Supervised: public virtual emu::base::Supervised
		{

		public:

			/** Default constructor. **/
			Supervised(xdaq::ApplicationStub *stub);

			/** Method that will cause the FSM to perform a transition bound to a given event if it exists.
			*
			*	@param event is the named transition that the FSM will attempt to perform.
			**/
			void fireEvent(std::string event);

			/** Fire a FSM transition command from a CGI GET parameter.
			*
			*	@param in is the standard XGI input.  Requires a field named "action" with a string corresponding to the requested transition
			*	@param out is the standard XGI output
			*
			*	@author Phillip Killewald &lt;paste@mps.ohio-state.edu&gt;
			**/
			void webFire(xgi::Input *in, xgi::Output *out);
			
			/** Tells the application to ignore SOAP commands 
			*
			*	@note Looks for an input paramter "ignoreSOAP" with an integer value, 1 = ignore, 0 = do not ignore.
			**/
			void webIgnoreSOAP(xgi::Input *in, xgi::Output *out);

		protected:
			
			/// The run number of the current run.  Useful for log files.
			xdata::UnsignedLong runNumber_;
			
			/// Whether or not to ignore SOAP state changes.
			xdata::Boolean ignoreSOAP_;
			
			/// Whether or not the command is coming from SOAP.
			bool fromSOAP_;

		};
	}
}


#endif
