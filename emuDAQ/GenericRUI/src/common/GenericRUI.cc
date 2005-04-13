//-----------------------------------------------------------------------
// $Id: GenericRUI.cc,v 2.0 2005/04/13 10:53:02 geurts Exp $
// $Log: GenericRUI.cc,v $
// Revision 2.0  2005/04/13 10:53:02  geurts
// Makefile
//
//
//-----------------------------------------------------------------------
#include "GenericRUI.h"

#include "i2oEVBMsgs.h"
#include "i2oXFunctionCodes.h"

#include <unistd.h>  // usleep(3)
#include <iomanip>

const int NIL_TARGET_ID = -1;

GenericRUI::GenericRUI() :
  eventNumber_(-1),
  maxEvents_(-1),
  nEvents_(0),
  task_(0),
  taskActivated_(false),
  ruTid_(0),
  dataQualityMonitor_(""),
  dataQualityMonitorInstance_(0),
  dataQualityMonitorTid_(NIL_TARGET_ID),
  prescaleForPushingToDQM_(0),
  requestorTid_(-1),
  sourceId_(0),
  addCDF_(false),
  sendEmptyEvents_(false),
  fakeMode_(false),
  jeremyEVB_(false)
{
    sprintf(taskName_, "RUIXXXX");

    exportParam("eventNumber", eventNumber_);
    exportParam("ruTid",       ruTid_);
    exportParam("Source ID", sourceId_);
    exportParam("Data Quality Monitor", dataQualityMonitor_);
    exportParam("Data Quality Monitor Instance", dataQualityMonitorInstance_);
    exportParam("Prescale for Pushing to DQM", prescaleForPushingToDQM_);
    exportParam("Max Events", maxEvents_);
    exportParam("Add CDF", addCDF_);
    exportParam("Send Empty Events", sendEmptyEvents_);
    exportParam("Fake Mode", fakeMode_);
    exportParam("Jeremy EVB", jeremyEVB_);
}


GenericRUI::~GenericRUI()
{
  if(taskActivated_)
  {
     LOG4CPLUS_FATAL(this->getLogger(),
                    "Cannot destroy RUI as task is still activated");
     exit(-1);
  }

  if(task_ != 0)
  {
     delete task_;
  }
}


int GenericRUI::taskSvc()
{
    while(taskShouldRun_)
    {
        taskShouldRun_ = taskShouldRun_ && (
          eventReader_->readNextEvent() &&   
          (maxEvents_<0 || nEvents_<maxEvents_) );
//cout << "Read next event, event " << eventReader_->eventNumber() << endl;
        // assign tashShouldRun here
	if (taskShouldRun_) {
            ++nEvents_;
	    char * data = eventReader_->data();
	    int dataLength = eventReader_->dataLength();
	    int eventn = eventReader_->eventNumber();

            if(jeremyEVB_) {
              m_lastEventNumber++;
              if (eventn==m_lastEventNumber+1) {
                eventn=m_lastEventNumber; // just to build with HCAL...
              }
              else if (eventn==1) { // probably the beginning of a new run
                m_lastEventNumber=0;
                eventn=0;
              }
              else if (eventn!=m_lastEventNumber) {
                cout << "FAKING UP EVENT NUMBER " << m_lastEventNumber << endl;
                eventn=m_lastEventNumber;
              }
            }
            // see if we need to send empty events before this one
            while(sendEmptyEvents_ && (nEvents_ < eventn)) {
              sendEmptyEvent(nEvents_);
              LOG4CPLUS_INFO(this->getLogger(), toolbox::toString("adding empty event %d from source %d", nEvents_, sourceId_));
              ++nEvents_;
            }
            // see if the event needs a Common Data Format header
            if(addCDF_) {
              addCDFAndSend(data, dataLength, eventn, ruTid_); 
            } else {
	      sendMessage(data, dataLength, eventn, ruTid_);
            }

	    LOG4CPLUS_DEBUG(getLogger(), toolbox::toString("frame sent.to %d",ruTid_));

            // write event locally, if asked to
	    if (eventWriter_ != NULL) {
		eventWriter_->writeEvent(data, dataLength);
	    }

            // see if the event needs to be pushed to the data quality monitor
            // maybe there's a more efficient thing to do than repacking.
            if(prescaleForPushingToDQM_ != 0 && 
               dataQualityMonitorTid_ != NIL_TARGET_ID) {
              // only send if it passes the prescale
              if((eventNumber_ % prescaleForPushingToDQM_) == 0) {
		if(addCDF_){
		  //The dqm would like to know where the data is coming from
		  //if we are reading from a source with no CDF header/trailer
		  //so add one and send.
		  addCDFAndSend(data,dataLength,eventn,dataQualityMonitorTid_);
		}
		else{
                sendMessage(data, dataLength, 
                                 eventn, dataQualityMonitorTid_);
		}
		LOG4CPLUS_DEBUG(getLogger(), toolbox::toString("frame sent.to %d",dataQualityMonitorTid_));
              }	      
            }

            // see if anyone requested a look at this data
            if(requestorTid_ >= 0) {
               sendMessage(data, dataLength,
                               eventn, requestorTid_);
               // reset it
               requestorTid_ = -1;
            }

	    // The event number of CMS will be 24-bits
	    // 2 to the power of 24 = 16777216
	    eventNumber_ = eventn;

//	    usleep(100);
	}
    }
    taskActivated_ = false;

    return 0;
}


void GenericRUI::plugin()
{
    LOG4CPLUS_INFO(this->getLogger(), "Plugged in");

    // Create the RUI task's name
    if(instance_ > 9999)
    {
        LOG4CPLUS_FATAL(this->getLogger(),
                        "RUI instance number is greater than 9999");

        exit(-1);
    }
    sprintf(taskName_, "RUI%04d", instance_);

    // Create the RUI's task object but do not activate it
    task_ = new RUITask(taskName_, this);
}


/**
* Standard SOAP callback routine.
*/
void GenericRUI::Configure() throw (xdaqException)
{
    if(dataQualityMonitor_ != "") {
      dataQualityMonitorTid_ = 
         xdaq::getTid(dataQualityMonitor_, dataQualityMonitorInstance_);
    }
}


/**
* Standard SOAP callback routine.
*/
void GenericRUI::Enable() throw (xdaqException)
{
    if(taskActivated_)
    {
        LOG4CPLUS_FATAL(this->getLogger(),
                        "Cannot enable - RUI task already activated");
        exit(-1);
    }

    if(task_ == 0)
    {
        LOG4CPLUS_FATAL(this->getLogger(),
                        "Cannot enable - RUI task does not exist");
        exit(-1);
    }

    m_lastEventNumber= jeremyEVB_ ? -1 : 0;
    taskShouldRun_ = true;
    taskActivated_ = true;
    task_->activate();
}


/**
 * Standard SOAP callback routine.
 */
void GenericRUI::Halt() throw (xdaqException)
{
    taskShouldRun_ = false;

    while(taskActivated_)
    {
        usleep(1000);
    }
}


void GenericRUI::handleDataRequest(BufRef * ref) {
    char *frame = ref->data();

    I2O_MESSAGE_FRAME               *stdMsgFrame =
            (I2O_MESSAGE_FRAME *)              frame;

    // flag that this target requested data
    requestorTid_ = stdMsgFrame->InitiatorAddress;
    xdaq::frameFree(ref);     
}


// packs into I2O format
void GenericRUI::sendMessage(char *data, int dataLength, int eventNumber, int targetAddress)
{
    // if we're in fake mode, just print data and return
    if(fakeMode_) {
      printData(data, dataLength);
      return;
    }

    size_t frameHeaderSize = sizeof(I2O_RU_DATA_READY_MESSAGE_FRAME);
    BufRef *bufRef = xdaq::frameAlloc(frameHeaderSize + dataLength);

    char *frame = bufRef->data();

    I2O_MESSAGE_FRAME               *stdMsgFrame =
	    (I2O_MESSAGE_FRAME *)              frame;
    I2O_PRIVATE_MESSAGE_FRAME       *pvtMsgFrame =
	    (I2O_PRIVATE_MESSAGE_FRAME *)      frame;
    I2O_RU_DATA_READY_MESSAGE_FRAME *ruMsgFrame =
	    (I2O_RU_DATA_READY_MESSAGE_FRAME *)frame;

    char *dataFrame = frame + frameHeaderSize;

    // clear RU message header area
    memset(ruMsgFrame, 0, sizeof(I2O_RU_DATA_READY_MESSAGE_FRAME));

    stdMsgFrame->MessageSize       = (frameHeaderSize + dataLength) >> 2;
    stdMsgFrame->InitiatorAddress  = tid();
    stdMsgFrame->TargetAddress     = targetAddress;
    stdMsgFrame->Function          = I2O_PRIVATE_MESSAGE;
    stdMsgFrame->VersionOffset     = 0;
    stdMsgFrame->MsgFlags          = 0;  // Point-to-point

    pvtMsgFrame->XFunctionCode     = I2O_RU_DATA_READY;
    pvtMsgFrame->OrganizationID    = XDAQ_ORGANIZATION_ID;
 
    ruMsgFrame->eventNumber        = eventNumber;
    ruMsgFrame->totalBlocks        = 1;
    ruMsgFrame->currentBlock       = 0;

    // fill the payload
    memcpy(dataFrame, data, dataLength);

    xdaq::frameSend(bufRef);
}


void GenericRUI::sendEmptyEvent(int eventNumber) {
  emptyEvent_[1] = 0x58000000u;
  emptyEvent_[1] |=(eventNumber&0xffffff);
  sendMessage(reinterpret_cast<char*>(emptyEvent_), 16, eventNumber, ruTid_);
}


void GenericRUI::initializeEmptyEvent() {
  // common data format
  emptyEvent_[0]=(0)<<20; // bunch id
  emptyEvent_[0] |=(0x8)|((sourceId_)<<8); // source
  emptyEvent_[1] = 0x58000000u;
  //emptyEvent_[1] |=(nevent&0xffffff);
  emptyEvent_[2] = 0;
  // "a" header, plus 64-bit word count (2)
  emptyEvent_[3] =0xa0000002u;
}


void GenericRUI::addCDFAndSend(char *data, int dataLength, int eventNumber, int targetaddress) {
  char * newData = new char[dataLength+16];
  // add a CDF Header
  initializeEmptyEvent();
  emptyEvent_[1] |=(eventNumber&0xffffff);
  memcpy(newData, &emptyEvent_[0], 8);
  // the input data
  memcpy(newData+8, data, dataLength);
  // the CDF trailer
  // counts words in 64-bits
  emptyEvent_[3] = 0xa0000000u | (dataLength+16)/8;
  memcpy(newData+8+dataLength, &emptyEvent_[2], 8);
  sendMessage(newData, dataLength+16, eventNumber, targetaddress);
  delete newData;
} 


void GenericRUI::printData(char * data, int dataLength) {
  // everybody likes to see 4-digit hex words.  They just do.
  unsigned short * shorts = reinterpret_cast<unsigned short *>(data);
  int nshorts = dataLength / sizeof(unsigned short);
  std::cout<<std::hex;
  for(int i = 0; i < nshorts; i+=4) {
    std::cout << shorts[i] << " " << shorts[i+1] << " " 
	      << shorts[i+2] << " " << shorts[i+3] << std::endl;
  }
  std::cout<<std::dec;
}

