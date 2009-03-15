// $Id: EmuPeripheralCrateBase.h,v 1.2 2009/03/15 12:32:14 liu Exp $

#ifndef _EmuPeripheralCrateBase_h_
#define _EmuPeripheralCrateBase_h_

#include "xgi/Utils.h"
#include "xgi/Method.h"

#include "xdaq/Application.h"
#include "xdaq/ApplicationGroup.h"
#include "xdaq/ApplicationContext.h"
#include "xdaq/ApplicationStub.h"
#include "xdaq/exception/Exception.h"
#include "xdaq/WebApplication.h"

#include "xdaq/NamespaceURI.h"

#include "xoap/MessageReference.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/Method.h"
#include "toolbox/fsm/FiniteStateMachine.h"
#include <string>
#include "xdata/String.h"

namespace emu {
  namespace pc {
  
class EmuPeripheralCrateBase: public xdaq::WebApplication
{
  
public:
  
  EmuPeripheralCrateBase(xdaq::ApplicationStub * s);
  //
protected:
  void changeState(toolbox::fsm::FiniteStateMachine &fsm)
                        throw (toolbox::fsm::exception::Exception);
  void transitionFailed(toolbox::Event::Reference event)
                        throw (toolbox::fsm::exception::Exception);
  void fireEvent(std::string event) throw (toolbox::fsm::exception::Exception);

  xoap::MessageReference createReply(xoap::MessageReference message)
                        throw (xoap::exception::Exception);

  //
  xoap::MessageReference PCcreateCommandSOAP(std::string command);
  int PCsendCommand(std::string command, std::string klass, int instance = -1) 
                        throw (xoap::exception::Exception, xdaq::exception::Exception);
  //
  toolbox::fsm::FiniteStateMachine fsm_;
  xdata::String state_;

private:

};

  } // namespace emu::pc
} // namespace emu
  
#endif
