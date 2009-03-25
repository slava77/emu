// $Id: EmuPeripheralCrateTimer.h,v 1.2 2009/03/25 11:37:22 liu Exp $

#ifndef _EmuPeripheralCrateTimer_h_
#define _EmuPeripheralCrateTimer_h_

#include "xgi/Utils.h"
#include "xgi/Method.h"

#include "xdaq/Application.h"
#include "xdaq/ApplicationGroup.h"
#include "xdaq/ApplicationContext.h"
#include "xdaq/ApplicationStub.h"
#include "xdaq/exception/Exception.h"


#include "xdaq/NamespaceURI.h"

#include "xoap/MessageReference.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/Method.h"

#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"

#include "xdata/exdr/FixedSizeInputStreamBuffer.h"
#include "xdata/exdr/AutoSizeOutputStreamBuffer.h"
#include "xdata/exdr/Serializer.h"
#include <xdata/String.h>
#include <xdata/Float.h>
#include <xdata/Double.h>
#include <xdata/Integer.h>
#include <xdata/Boolean.h>
#include <xdata/UnsignedLong.h>
#include <xdata/UnsignedShort.h>
#include <xdata/Table.h>

#include "toolbox/Event.h"
#include "toolbox/string.h"
#include "toolbox/net/URL.h"
#include "toolbox/task/TimerTask.h"
#include "toolbox/task/Timer.h"
#include "toolbox/task/TimerFactory.h"
#include "toolbox/TimeInterval.h"

#include "emu/pc/EmuPeripheralCrateBase.h"

namespace emu {
  namespace pc {
  
class EmuPeripheralCrateTimer: public EmuPeripheralCrateBase,
       public toolbox::task::TimerListener
{
  
public:
  
  XDAQ_INSTANTIATOR();
  
  //
  EmuPeripheralCrateTimer(xdaq::ApplicationStub * s);
  //
  void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void MainPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  //
  xoap::MessageReference onConfigure (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onEnable (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onDisable (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onHalt (xoap::MessageReference message) throw (xoap::exception::Exception);
  //
  // define states
  void stateChanged(toolbox::fsm::FiniteStateMachine &fsm) throw (toolbox::fsm::exception::Exception);
  void dummyAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception);
  // for Monitoring
  xoap::MessageReference  MonitorStart(xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference  MonitorStop(xoap::MessageReference message) throw (xoap::exception::Exception);
  void timeExpired (toolbox::task::TimerEvent& e);

private:

  bool Monitor_On_, Monitor_Ready_, In_Monitor_, In_Broadcast_;
  toolbox::task::Timer * timer_;
  xdata::UnsignedShort fastloop, slowloop, extraloop;
  int current_state_;
};

  } // namespace emu::pc
} // namespace emu
  
#endif
