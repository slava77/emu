// $Id: EmuClient.h,v 3.2 2007/03/05 11:00:17 banicz Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _EmuClient_h_
#define _EmuClient_h_

#include <vector>
#include "Task.h"
#include "toolbox/mem/Reference.h"
#include "toolbox/mem/Pool.h"
#include "toolbox/fsm/FiniteStateMachine.h"
#include "xdaq/Application.h"
#include "xdata/UnsignedLong.h"
#include "xdata/include/xdata/String.h"
#include "xdata/Event.h"
#include "xdata/ActionListener.h"

class EmuClient: public xdaq::Application, public Task, public xdata::ActionListener
{	
public:
  XDAQ_INSTANTIATOR();

  EmuClient(xdaq::ApplicationStub* c) throw(xdaq::exception::Exception);

protected:
	
  // Send an I2O token message to all servers, If last is 1, the flag 'last' in the
  // message is set to one. Otherwise it is set to 0
  //
  int sendMessage(unsigned long last);

  // service routine for sending
  int svc();

  //
  // used to create the memory pool upon parametrization
  //	
  void actionPerformed (xdata::Event& e);
	
  void emuDataMsg(toolbox::mem::Reference *bufRef);
  void printMessageReceived( toolbox::mem::Reference *bufRef );

protected:

  std::set<xdaq::ApplicationDescriptor*> destination_;	// Vector of all server tids
  xdata::UnsignedLong maxFrameSize_;	 	// The maximum frame size to be allocated by the Client       
  xdata::UnsignedLong committedPoolSize_;	// Total memory for credit messages
  xdata::String       serversClassName_; 	// servers' class name
  xdata::UnsignedLong serversClassInstance_;	// instance of server
  xdata::UnsignedLong nEventCredits_;	 	// send this meny event credits at a time
  xdata::UnsignedLong prescalingFactor_;        // prescaling factor for data to be received

  bool hasSet_committedPoolSize_;
  bool hasSet_serversClassName_; 
  bool hasSet_serversClassInstance_; 
  bool hasSet_nEventCredits_;	 
  bool hasSet_prescalingFactor_;

  bool serviceLoopStarted_;
  
  toolbox::mem::Pool* pool_;			// Memory pool for allocating messages for sending

  deque<toolbox::mem::Reference*> dataMessages_; // the queue of data messages waiting to be processed

  BSem applicationBSem_;

};

#endif
