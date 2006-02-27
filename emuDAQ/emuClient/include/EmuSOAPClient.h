// $Id: EmuSOAPClient.h,v 3.1 2006/02/27 12:49:27 banicz Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _EmuSOAPClient_h_
#define _EmuSOAPClient_h_

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


#include "xdaq/NamespaceURI.h"

#include "xoap/MessageReference.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/Method.h"


class EmuSOAPClient: public xdaq::Application, public Task, public xdata::ActionListener
{	
public:
  XDAQ_INSTANTIATOR();

  EmuSOAPClient(xdaq::ApplicationStub* c) throw(xdaq::exception::Exception);

protected:
	
  int sendCreditSOAPMessage()
    throw ( xoap::exception::Exception );

  // service routine for sending
  int svc();

  //
  // used to create the memory pool upon parametrization
  //	
  void actionPerformed (xdata::Event& e);
	
  xoap::MessageReference emuDataSOAPMsg(xoap::MessageReference msg)
    throw (xoap::exception::Exception);
  std::string printMessageReceived( xoap::MessageReference msg );
  DOMNode *findNode(DOMNodeList *nodeList,
		    const string nodeLocalName)
    throw ( xoap::exception::Exception );

protected:

  std::string         name_;
  std::vector<xdaq::ApplicationDescriptor*> destination_;	// Vector of all server tids
  xdata::String       serversClassName_; 	// servers' class name
  xdata::UnsignedLong serversClassInstance_;	// instance of server
  xdata::UnsignedLong nEventCredits_;	 	// send this meny event credits at a time
  xdata::UnsignedLong prescalingFactor_;        // prescaling factor for data to be received

  bool hasSet_serversClassName_; 
  bool hasSet_serversClassInstance_; 
  bool hasSet_nEventCredits_;	 
  bool hasSet_prescalingFactor_;
  
  bool serviceLoopStarted_;

  BSem applicationBSem_;

};

#endif
