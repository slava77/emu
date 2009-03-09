/*****************************************************************************\
* $Id: Supervised.h,v 1.1 2009/03/09 16:03:16 paste Exp $
*
* $Log: Supervised.h,v $
* Revision 1.1  2009/03/09 16:03:16  paste
* * Updated "ForPage1" routine in Manager with new routines from emu::base::WebReporter
* * Updated inheritance in wake of changes to emu::base::Supervised
* * Added Supervised class to separate XDAQ web-based applications and those with a finite state machine
*
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
		class Supervised: public virtual emu::base::Supervised {

		public:

			/** Default constructor. **/
			Supervised(xdaq::ApplicationStub *stub);

			/* Method used as a general call-back from FSM state changes.
			*
			*	@param fsm is the finite state machine that just transitioned stated
			**/
			//void stateChanged(toolbox::fsm::FiniteStateMachine &fsm);

			/* Method used as a general call-back when the FSM transitions to the Failed state.
			*
			*	@param event is the event (exception) that caused the failure
			**/
			//void transitionFailed(toolbox::Event::Reference event);

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

		protected:
			
			/// The run number of the current run.  Useful for log files.
			xdata::UnsignedLong runNumber_;
			
			/// Whether or not the current state has been transitioned to via SOAP or via a web request.
			bool soapLocal_;
			
			/// Whether or not the application has been configured via SOAP or via the web interface.
			bool soapConfigured_;
			

		};
	}
}


#endif
