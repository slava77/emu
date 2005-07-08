//-----------------------------------------------------------------------
// $Id: TriggerAdapter.cc,v 2.0 2005/07/08 12:15:41 geurts Exp $
// $Log: TriggerAdapter.cc,v $
// Revision 2.0  2005/07/08 12:15:41  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#include "TriggerAdapter.hh"
#include "i2oEVBMsgs.h"
#include "i2oXFunctionCodes.h"


TriggerAdapter::TriggerAdapter() : mutex(BSem::FULL) {

  i2oBindMethod(this,&TriggerAdapter::ta_credit_message,
                I2O_TA_CREDIT,
                XDAQ_ORGANIZATION_ID, 0);
  nCredits=0;
  evmTid=-1;
  runNumber=1;

  exportParam("evmTid",evmTid);
  exportParam("runNumber",runNumber);
}

void TriggerAdapter::ta_credit_message (BufRef* buf){

  I2O_TA_CREDIT_MESSAGE_FRAME* frame=(I2O_TA_CREDIT_MESSAGE_FRAME*)buf->buf_;

  mutex.take();
  nCredits += frame->nbCredits;

  LOG4CPLUS_DEBUG(this->getLogger(),toolbox::toString("Got %d credits.  Total of %d now.  I will spend them wisely.",frame->nbCredits,nCredits));

  if(state() == Enabled){
    post_credits();
  }
  mutex.give();

  xdaq::frameFree(buf);
}


void TriggerAdapter::Enable() throw(std::exception){
  if (evmTid<0) evmTid=xdaq::getTid("EVM",0);
  nEvent=1;

  mutex.take();
  post_credits();
  mutex.give();
}

void TriggerAdapter::Resume() throw(std::exception){
  mutex.take();
  post_credits();
  mutex.give();
}

void TriggerAdapter::post_credits(){

  for (int i=0; i<nCredits; i++){
    int bufferSize=sizeof(SliceTestTriggerChunk)+sizeof(I2O_EVM_TRIGGER_MESSAGE_FRAME);

    BufRef* ref=xdaq::frameAlloc(bufferSize);
    I2O_EVM_TRIGGER_MESSAGE_FRAME* frame=(I2O_EVM_TRIGGER_MESSAGE_FRAME*)ref->buf_;

    // generic I2O stuff
    memset(frame,0,bufferSize);

    frame->PvtMessageFrame.StdMessageFrame.MsgFlags          = 0;
    frame->PvtMessageFrame.StdMessageFrame.VersionOffset     = 0;
    frame->PvtMessageFrame.StdMessageFrame.Function          = I2O_PRIVATE_MESSAGE;
    frame->PvtMessageFrame.StdMessageFrame.TargetAddress     = evmTid;;
    frame->PvtMessageFrame.StdMessageFrame.InitiatorAddress  = tid();
    frame->PvtMessageFrame.StdMessageFrame.MessageSize       = (bufferSize)>>2;
    frame->PvtMessageFrame.XFunctionCode                     = I2O_EVM_TRIGGER;
    frame->PvtMessageFrame.OrganizationID                    = XDAQ_ORGANIZATION_ID; 

    frame->eventNumber=nEvent;

    SliceTestTriggerChunk* tc=(SliceTestTriggerChunk*)(((char*)ref->buf_)+sizeof(I2O_EVM_TRIGGER_MESSAGE_FRAME));

    tc->triggerNumber=nEvent;
    tc->runNumber=runNumber;
      
    // common data format
    tc->h1a=(0)<<20; // bunch id
    tc->h1a|=(0x8)|((SliceTestTriggerChunk_SourceId)<<8); // source
    tc->h1b=0x58000000u;
    tc->h1b|=(nEvent&0xFFFFFF);
    tc->t1a=0;
    tc->t2a=0xA0000000u;
    tc->t2a|=sizeof(SliceTestTriggerChunk)/8;
    
    xdaq::frameSend(ref);

    nEvent++;

  }
  nCredits=0;

}
