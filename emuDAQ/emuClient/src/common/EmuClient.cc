// $Id: EmuClient.cc,v 3.4 2006/09/11 12:16:54 banicz Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#include <iomanip>

#include "EmuClient.h"
#include "i2oEmuClientMsg.h"

// #include "i2oStreamIOMsg.h"
#include "Task.h"

#include "toolbox/rlist.h"
#include "toolbox/fsm/FiniteStateMachine.h"
#include "toolbox/mem/Reference.h"
#include "toolbox/mem/MemoryPoolFactory.h"
#include "toolbox/mem/Pool.h"
#include "toolbox/mem/Reference.h"
#include "toolbox/mem/Pool.h"
#include "toolbox/mem/CommittedHeapAllocator.h"

#include "i2o/Method.h"
#include "i2o/utils/AddressMap.h"

#include "xdaq/Application.h"
#include "xdaq/ApplicationContext.h"
#include "xdaq/ApplicationGroup.h"
#include "xoap/MessageFactory.h"
#include "xdaq/NamespaceURI.h"
#include "xcept/Exception.h"
#include "xcept/tools.h"

#include "emuReadout/include/MuEndDDUHeader.h"

EmuClient::EmuClient(xdaq::ApplicationStub* c)
  throw(xdaq::exception::Exception)
  :xdaq::Application(c)
  ,Task("EmuClient") 
  ,applicationBSem_(BSem::FULL)
{
  getApplicationInfoSpace()->fireItemAvailable("committedPoolSize",&committedPoolSize_);
  getApplicationInfoSpace()->fireItemAvailable("serversClassName", &serversClassName_);
  getApplicationInfoSpace()->fireItemAvailable("serversClassInstance", &serversClassInstance_);
  getApplicationInfoSpace()->fireItemAvailable("nEventCredits",    &nEventCredits_);
  getApplicationInfoSpace()->fireItemAvailable("prescalingFactor", &prescalingFactor_);

  // Add infospace listeners for exporting data values
  getApplicationInfoSpace()->addItemChangedListener ("committedPoolSize",    this);
  getApplicationInfoSpace()->addItemChangedListener ("serversClassName",     this);
  getApplicationInfoSpace()->addItemChangedListener ("serversClassInstance", this);
  getApplicationInfoSpace()->addItemChangedListener ("nEventCredits",        this);
  getApplicationInfoSpace()->addItemChangedListener ("prescalingFactor",     this);

  // bind i2o callback for server messages
  i2o::bind(this, &EmuClient::emuDataMsg, I2O_EMUCLIENT_CODE, XDAQ_ORGANIZATION_ID );
  maxFrameSize_ = sizeof(I2O_EMUCLIENT_CREDIT_MESSAGE_FRAME);

  // These are used to make sure the service loop starts only after all parameters have been set.
  hasSet_committedPoolSize_     = false;
  hasSet_serversClassName_      = false;
  hasSet_serversClassInstance_  = false;
  hasSet_nEventCredits_         = false;
  hasSet_prescalingFactor_      = false;
  serviceLoopStarted_           = false;

}

int EmuClient::svc()
{
  LOG4CPLUS_DEBUG (getApplicationLogger(), "starting send loop...");

  while(true)
    {

      sleep(1);

      if (!pool_->isHighThresholdExceeded())
	{
	  // Stop if there is an error in sending
	  if (this->sendMessage(0) == 1) 
	    { 
	      LOG4CPLUS_FATAL (getApplicationLogger(), toolbox::toString("Error in frameSend. Stopping client."));
 	      return 1;
	    }
	} else 
	  {
	    LOG4CPLUS_DEBUG (getApplicationLogger(), "high threshold is exceeded");
	    while (pool_->isLowThresholdExceeded())
	      {
		LOG4CPLUS_INFO (getApplicationLogger(), "yield till low threshold reached");
		this->yield(1);
	      }
	  }
    }


  return 0;
}

//
// used to create the memory pool upon parameter initialization and start the service loop
//	
void EmuClient::actionPerformed (xdata::Event& e) 
{
  LOG4CPLUS_INFO(getApplicationLogger(), "An event occured.");

  // update measurements monitors		
  if (e.type() == "ItemChangedEvent")
    {
      std::string item = dynamic_cast<xdata::ItemChangedEvent&>(e).itemName();
      LOG4CPLUS_INFO(getApplicationLogger(), item << " changed.");
      if ( item == "committedPoolSize")
	{
	  LOG4CPLUS_INFO(getApplicationLogger(), 
			 toolbox::toString("EmuClient's Tid: %d",
					   i2o::utils::getAddressMap()->getTid(this->getApplicationDescriptor())) );
	  try 
	    {
	      LOG4CPLUS_INFO (getApplicationLogger(), "Committed pool size is " << (unsigned long) committedPoolSize_);
	      toolbox::mem::CommittedHeapAllocator* a = new toolbox::mem::CommittedHeapAllocator(committedPoolSize_);
	      toolbox::net::URN urn ("toolbox-mem-pool", "EmuClient_EMU_MsgPool");
	      pool_ = toolbox::mem::getMemoryPoolFactory()->createPool(urn, a);
// 	      LOG4CPLUS_INFO (getApplicationLogger(), "Set high watermark to 90% and low watermark to 70%");
	      LOG4CPLUS_INFO (getApplicationLogger(), "Set event credit message buffer's high watermark to 70%");
	      pool_->setHighThreshold ( (unsigned long) (committedPoolSize_ * 0.7));
// 	      pool_->setLowThreshold ((unsigned long) (committedPoolSize_ * 0.7));
	      hasSet_committedPoolSize_ = true;
	    }
	  catch(toolbox::mem::exception::Exception & e)
	    {
	      LOG4CPLUS_FATAL (getApplicationLogger(), toolbox::toString("Could not set up memory pool, %s", e.what()));
	      return;
	    }

	}
      if ( item == "serversClassName")
	{
	  try 
	    {
	      destination_ = getApplicationContext()->getApplicationGroup()->getApplicationDescriptors(serversClassName_.toString().c_str());
	      hasSet_serversClassName_ = true;
	    }
	  catch (xdaq::exception::Exception& e)
	    {
	      LOG4CPLUS_ERROR (getApplicationLogger(), 
			       "No " << serversClassName_.toString() << 
			       "found. EmuClient cannot be configured." << 
			       xcept::stdformat_exception_history(e));
	    }	
	}
      if ( item == "serversClassInstance")
	{
	  LOG4CPLUS_INFO(getApplicationLogger(), "Server " << serversClassName_.toString() << " instance " << serversClassInstance_ );
	  hasSet_serversClassInstance_ = true;
	}

      if ( item == "nEventCredits")    hasSet_nEventCredits_    = true;
      if ( item == "prescalingFactor") hasSet_prescalingFactor_ = true;

      // start service loop when all configurable parameters have been set
      if ( hasSet_committedPoolSize_     &&
	   hasSet_serversClassName_      &&
	   hasSet_serversClassInstance_  &&
	   hasSet_nEventCredits_         &&
	   hasSet_prescalingFactor_      &&
	   !serviceLoopStarted_             ) {
	LOG4CPLUS_INFO(getApplicationLogger(),"Starting work loop.");
	if ( this->activate() == 0 ) serviceLoopStarted_ = true;
      }
    }
}
	
	
	
// Send an I2O token message to the specified instance of the server
int EmuClient::sendMessage(unsigned long last) 
{	
  for (unsigned int i = 0; i < destination_.size(); i++)
    {

      if ( destination_[i]->getInstance() != serversClassInstance_.value_ ) continue;

      toolbox::mem::Reference * ref = 0;
      try 
	{
	  ref = toolbox::mem::getMemoryPoolFactory()->getFrame(pool_, maxFrameSize_);

	  PI2O_EMUCLIENT_CREDIT_MESSAGE_FRAME frame = (PI2O_EMUCLIENT_CREDIT_MESSAGE_FRAME) ref->getDataLocation();   


	  frame->PvtMessageFrame.StdMessageFrame.MsgFlags         = 0;
	  frame->PvtMessageFrame.StdMessageFrame.VersionOffset    = 0;
	  frame->PvtMessageFrame.StdMessageFrame.TargetAddress    = i2o::utils::getAddressMap()->getTid(destination_[i]);
	  frame->PvtMessageFrame.StdMessageFrame.InitiatorAddress = i2o::utils::getAddressMap()->getTid(this->getApplicationDescriptor());
	  frame->PvtMessageFrame.StdMessageFrame.MessageSize      = (sizeof(I2O_EMUCLIENT_CREDIT_MESSAGE_FRAME)) >> 2;

	  frame->PvtMessageFrame.StdMessageFrame.Function = I2O_PRIVATE_MESSAGE;
	  frame->PvtMessageFrame.XFunctionCode            = I2O_EMUCLIENT_CODE;
	  frame->PvtMessageFrame.OrganizationID           = XDAQ_ORGANIZATION_ID;

	  frame->nEventCredits    = nEventCredits_;
	  frame->prescalingFactor = prescalingFactor_;

	  ref->setDataSize(frame->PvtMessageFrame.StdMessageFrame.MessageSize << 2);
	  LOG4CPLUS_DEBUG (getApplicationLogger(),
			   "Sending credit to tid: " << frame->PvtMessageFrame.StdMessageFrame.TargetAddress
			   );
	  getApplicationContext()->postFrame(ref, this->getApplicationDescriptor(), destination_[i]);
	} 
      catch (toolbox::mem::exception::Exception & me)
	{
	  LOG4CPLUS_FATAL (getApplicationLogger(), xcept::stdformat_exception_history(me));
	  return 1; // error
	}
      catch (xdaq::exception::Exception & e)
	{
	  // Retry 3 times
	  bool retryOK = false;
	  for (int k = 0; k < 3; k++)
	    {
	      try
		{
		  getApplicationContext()->postFrame(ref,  this->getApplicationDescriptor(), destination_[i]);
		  retryOK = true;
		  break; // if send was successfull, continue to send other messages
		}
	      catch (xdaq::exception::Exception & re)
		{
		  LOG4CPLUS_WARN (getApplicationLogger(), xcept::stdformat_exception_history(re));
		}
	    }

	  if (!retryOK)
	    {
	      LOG4CPLUS_FATAL (getApplicationLogger(), "Frame send failed after 3 times.");
	      LOG4CPLUS_FATAL (getApplicationLogger(), xcept::stdformat_exception_history(e));
	      ref->release();
	      return 1; // error				
	    }
	}
    }
  return 0; // o.k.
}

void EmuClient::emuDataMsg(toolbox::mem::Reference *bufRef){
  // EMu-specific stuff
  I2O_EMU_DATA_MESSAGE_FRAME *msg =
    (I2O_EMU_DATA_MESSAGE_FRAME*)bufRef->getDataLocation();

  char *startOfPayload = (char*) bufRef->getDataLocation() 
    + sizeof(I2O_EMU_DATA_MESSAGE_FRAME);
  MuEndDDUHeader *dduHeader = (MuEndDDUHeader *) startOfPayload;
  int eventNumber = dduHeader->lvl1num();

  LOG4CPLUS_INFO(getApplicationLogger(), 
		 "Received " << bufRef->getDataSize()-sizeof(I2O_EMU_DATA_MESSAGE_FRAME) << 
		 " bytes of event " << eventNumber << 
		 " of run " << msg->runNumber << 
		 " from " << serversClassName_.toString() <<
		 " still holding " << msg->nEventCreditsHeld << 
		 " event credits. Error flag 0x" << setw(2) 
		 << std::hex << setfill('0') << msg->errorFlag );

  // Send it to the end of the queue
//   dataMessages_.push_back( bufRef );

//   // Process the oldest message, i.e., the one at the front of the queue
//   toolbox::mem::Reference *oldestMessage = dataMessages_.front();
//   printMessageReceived( oldestMessage );

  // Free the memory taken by the oldest message...
//   oldestMessage->release();
  // ...and dismiss it from the queue
//   dataMessages_.erase( dataMessages_.begin() );

  bufRef->release();

}

void EmuClient::printMessageReceived( toolbox::mem::Reference *bufRef )
{
  if ( !bufRef )
    {
      LOG4CPLUS_WARN(getApplicationLogger(), "EmuClient::printMessageReceived: no buffer?!" );
      return;
    }

  stringstream ss;

  ss << "EmuFU::printMessageReceived:" << endl;
  I2O_EMU_DATA_MESSAGE_FRAME* msgData = 
    (I2O_EMU_DATA_MESSAGE_FRAME*) bufRef->getDataLocation();
  ss  << "   Data size [byte]      : " << bufRef->getDataSize() << endl;
  ss  << "   Run                   : " << msgData->runNumber << endl;
  ss  << "   Credits held by server: " << msgData->nEventCreditsHeld << endl;

  char         *startOfPayload = (char*) bufRef->getDataLocation() 
    + sizeof(I2O_EMU_DATA_MESSAGE_FRAME);
  unsigned long  sizeOfPayload =         bufRef->getDataSize()
    - sizeof(I2O_EMU_DATA_MESSAGE_FRAME);
  unsigned short *shorts = reinterpret_cast<unsigned short *>(startOfPayload);
  int nshorts = sizeOfPayload / sizeof(unsigned short);
  ss<<std::hex;
  for(int i = 0; i < nshorts; i+=4)
    {
      ss << "      ";
      ss.width(4); ss.fill('0');    
      ss << shorts[i+3] << " ";
      ss.width(4); ss.fill('0');    
      ss << shorts[i+2] << " ";
      ss.width(4); ss.fill('0');    
      ss << shorts[i+1] << " ";
      ss.width(4); ss.fill('0');    
      ss << shorts[i] << std::endl;
    }
  ss<<std::dec;
  ss.width(0);

  LOG4CPLUS_INFO( getApplicationLogger(), ss.str() );
}


XDAQ_INSTANTIATOR_IMPL(EmuClient)
