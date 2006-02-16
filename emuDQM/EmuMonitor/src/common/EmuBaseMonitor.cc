#include <unistd.h>
#include "EmuBaseMonitor.h"
// #include "EmuDAQ/GenericRUI/include/RUIDataRequest.h"
#include "EventFilter/CSCRawToDigi/interface/CSCDDUHeader.h"
#include "EventFilter/CSCRawToDigi/interface/CSCDDUTrailer.h"
#include "xdaq.h"
// #include "i2oEVBMsgs.h"

// XDAQ_INSTANTIATOR_IMPL(EmuBaseMonitor)

EmuBaseMonitor::EmuBaseMonitor(xdaq::ApplicationStub* stub)
        throw (xdaq::exception::Exception)
	: xdaq::WebApplication(stub)
{
  enabled_ = false;
  dataSourceTargetId_ = -1;
  previousEventNumber_ = -1;
  getApplicationInfoSpace()->fireItemAvailable("SourceTid", &dataSourceTargetId_);

  //  i2o::bind (this, &EmuBaseMonitor::I2O_ruDataReadyMsg, I2O_DQMDataRequestMsgCode, XDAQ_ORGANIZATION_ID);
        errorHandler_ = toolbox::exception::bind (this, &EmuBaseMonitor::onError, "onError");
}


EmuBaseMonitor::~EmuBaseMonitor() {
}

bool EmuBaseMonitor::onError ( xcept::Exception& ex, void * context )
{
        std::cout << "onError: " << ex.what() << std::endl;
        return false;
}

void EmuBaseMonitor::Configure() throw (toolbox::fsm::exception::Exception)
{
    LOG4CPLUS_DEBUG (getApplicationLogger(), "SourceTid: "<< dataSourceTargetId_);
}

void EmuBaseMonitor::Enable() throw (toolbox::fsm::exception::Exception)
{
     enabled_ = true;
    // request some data?
    if(dataSourceTargetId_ > xdata::Integer(-1)) {
       I2O_sendDataRequest();
    }
}

void EmuBaseMonitor::Halt() throw (toolbox::fsm::exception::Exception)
{
    enabled_ = false;
}


void EmuBaseMonitor::I2O_sendDataRequest() {
    /*
    size_t frameSize = sizeof(RUIDataRequest);
    BufRef *bufRef = xdaq::frameAlloc(frameSize);

    char *frame = bufRef->data();

    I2O_MESSAGE_FRAME * stdMsgFrame = reinterpret_cast<I2O_MESSAGE_FRAME *>(frame);
    RUIDataRequest * request = reinterpret_cast<RUIDataRequest*>(frame);

    // clear RU message header area
    memset(frame, 0, frameSize);

    stdMsgFrame->MessageSize       = (frameSize) >> 2;
    stdMsgFrame->InitiatorAddress  = tid();
    stdMsgFrame->TargetAddress     = dataSourceTargetId_;
    stdMsgFrame->Function          = I2O_PRIVATE_MESSAGE;
    request->PvtMessageFrame.XFunctionCode = RUIDataRequestMsgCode;
    request->PvtMessageFrame.OrganizationID = XDAQ_ORGANIZATION_ID;
    xdaq::frameSend(bufRef);
    */
}


void EmuBaseMonitor::I2O_ruDataReadyMsg(toolbox::mem::Reference * ref)  throw (i2o::exception::Exception) 
{
   LOG4CPLUS_DEBUG (getApplicationLogger(), "Receiving data in EmuBaseMonitor");
  // assume there's only one frame in this fragment
  /*
  size_t frameHeaderSize = sizeof(I2O_RU_DATA_READY_MESSAGE_FRAME);
  char *frame = ref->data();

  I2O_MESSAGE_FRAME               *request =
          (I2O_MESSAGE_FRAME *)              frame;

  char *data = frame + frameHeaderSize;
  // the message size seems to have been passed in 32-bit words
  int dataSize = (request->MessageSize << 2) - frameHeaderSize;
  */
/*
  for(int i=0; i<dataSize/sizeof(unsigned short);i++)
    {
      cout<<hex<<reinterpret_cast<unsigned short*>(data)[i]<<endl;
    }
*/

/*
  unsigned long errorFlag = 0;
  // don't bother processing if it's obviously bad, or
  // has the same event number as the previous event
//KK
  if(checkEvent(data, dataSize)) {
    processEvent(data, dataSize, errorFlag);
  }
//KKend

  xdaq::frameFree(ref);

  // see if we should ask for more data
  if(dataSourceTargetId_ != -1 && enabled_) {
    cout << "Requesting next event from TID:" << dataSourceTargetId_ << endl;
    sendDataRequest();
  }
*/
}


bool EmuBaseMonitor::checkEvent(const char * data, int dataSize) {
  // check that it's not the same as previous event, and header and
  // trailer are OK
  const CSCDDUHeader * dduHeader   = reinterpret_cast<const CSCDDUHeader*>(data);
  int lvl1num = dduHeader->lvl1num();
  if(lvl1num == previousEventNumber_) return false;

  int trailerPosition = dataSize/2 - CSCDDUTrailer::sizeInWords();
  const unsigned short * shorts = reinterpret_cast<const unsigned short*>(data);
  const CSCDDUTrailer * dduTrailer = reinterpret_cast<const CSCDDUTrailer*>(&(shorts[trailerPosition]));

  LOG4CPLUS_DEBUG (getApplicationLogger(),"DQM Header & trailer Checks " << dduHeader->check() << " " << dduTrailer->check());

  return(dduHeader->check() && dduTrailer->check());
}


void EmuBaseMonitor::processEvent(const char * data, int dataSize, unsigned long errorFlag) {
  usleep(1000000);
}

