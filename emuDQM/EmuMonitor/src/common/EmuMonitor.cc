/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#include "EmuMonitor.h"

#include <time.h>


std::string now()
{
  char buf[255];
  time_t now=time(NULL);
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S %Z", localtime(&now));
  std::string time = std::string(buf);
  return time.substr(0,time.find("\n",0));

};

std::string getDateTime(){
  time_t t;
  struct tm *tm;

  time ( &t );
  tm = gmtime ( &t ); // Unversal Coordinated Time

  std::stringstream ss;
  ss << std::setfill('0') << std::setw(2) << tm->tm_year%100
     << std::setfill('0') << std::setw(2) << tm->tm_mon+1
     << std::setfill('0') << std::setw(2) << tm->tm_mday      << "_"
     << std::setfill('0') << std::setw(2) << tm->tm_hour
     << std::setfill('0') << std::setw(2) << tm->tm_min
     << std::setfill('0') << std::setw(2) << 0       << "_UTC";

  return ss.str();
}


XDAQ_INSTANTIATOR_IMPL(EmuMonitor)

  // == EmuMonitor Constructor == //
  EmuMonitor::EmuMonitor(xdaq::ApplicationStub* c)
  throw(xdaq::exception::Exception)
    :xdaq::WebApplication(c)
    ,Task("EmuMonitor")
    ,appBSem_(BSem::FULL)
    ,isReadoutActive(false)
{
  LOG4CPLUS_INFO(this->getApplicationLogger(),"Constructor called");
  
  /*  
      getApplicationDescriptor()->setAttribute("icon",
      "/emu/emuDQM/EmuMonitor/images/dqm64x64.gif");
  */

  appBSem_.take();
  plotter_ = NULL;
  deviceReader_ = NULL;
  pool_ = NULL;
  initProperties();
  setMemoryPool();

  timer_ = new EmuMonitorTimerTask();
  pmeter_ = new toolbox::PerformanceMeter();
  pmeterCSC_ = new toolbox::PerformanceMeter();
  rateMeter = new RateMeter<xdata::UnsignedInteger>();
  rateMeter->addSampler("averageRate", &sessionEvents_);
  rateMeter->addSampler("cscRate", &cscUnpacked_);
  // rateMeter->activate();

  errorHandler_ = toolbox::exception::bind (this, &EmuMonitor::onError, "onError");

  defineFSM();
  defineWebSM();

  stateName_ = wsm_.getStateName(wsm_.getCurrentState());
  stateChangeTime_ = now();
  lastEventTime_ = "---";

  bindI2Ocallbacks();
  bindSOAPcallbacks();
  bindCGIcallbacks();

  appTid_ = this->getApplicationDescriptor()->getInstance();

  appBSem_.give();
}

EmuMonitor::~EmuMonitor() 
{
/*
	destroyDeviceReader();
	if (plotter_) delete plotter_;
	if (timer_) {
		if (timer_->isActive()) timer_->kill();
		delete timer_;
	}
	if (pmeter_) delete pmeter_;
	if (pmeterCSC_) delete pmeterCSC_;
	if (rateMeter) {
		if (rateMeter->isActive()) rateMeter->kill();
		delete rateMeter;
	}
*/
}

// == Application Error Handler == //
bool EmuMonitor::onError ( xcept::Exception& ex, void * context )
{
  LOG4CPLUS_ERROR(this->getApplicationLogger(), "onError: " << ex.what());
  return false;
}


// == Init Application Properties == //
void EmuMonitor::initProperties() 
{

  LOG4CPLUS_DEBUG(this->getApplicationLogger(),"initProperties called");

  sTimeout = 3;
  plotterSaveTimer_ = 120;
  outputROOTFile_ = "";
  outputImagesPath_ = ""; 
  xmlHistosBookingCfgFile_ = ""; 
  xmlCanvasesCfgFile_ = "";
  cscMapFile_ = "";
  dduCheckMask_ = 0xFFFFFFFF;
  binCheckMask_ = 0xFFFFFFFF;

  dataRate_ 		= "0.0";
  dataLatency_		= "0.0";
  dataBw_ 		= "0.0";
  cscRate_		= 0;
  cscUnpacked_		= 0;
  runNumber_		= 0;


  totalEvents_ 		= 0;
  sessionEvents_ 	= 0;
  readoutMode_ 		= "internal";
  nDAQEvents_		= 0;

  inputDataFormat_      = "DDU";
  inputDeviceType_	= "file";
  inputDeviceName_      = "";
  
  serversClassName_ 	= "EmuRUI";
  serverTIDs_.clear();
  daqGroup_		= "default";
  // serverTIDs_.push_back(0);
  collectorsClassName_	= "EmuDisplayServer";
  collectorID_		= 0;
  transport_            = "i2o";
  committedPoolSize_	= 0x500000; // 1Mb
  nEventCredits_	= 50;
  defEventCredits_      = nEventCredits_;
  prescalingFactor_	= 1;
  averageRate_          = 0;


  useAltFileReader_	= false;
  stateName_		= "";
  fSaveROOTFile_	= false;
  keepRunning           = false;

  getApplicationInfoSpace()->fireItemAvailable("totalEvents",&totalEvents_);
  getApplicationInfoSpace()->fireItemAvailable("sessionEvents",&sessionEvents_);
  getApplicationInfoSpace()->fireItemAvailable("dataBw",&dataBw_);
  getApplicationInfoSpace()->fireItemAvailable("dataLatency",&dataLatency_);
  getApplicationInfoSpace()->fireItemAvailable("dataRate",&dataRate_);
  getApplicationInfoSpace()->fireItemAvailable("averageRate",&averageRate_);
  getApplicationInfoSpace()->fireItemAvailable("cscRate",&cscRate_);
  getApplicationInfoSpace()->fireItemAvailable("cscUnpacked",&cscUnpacked_);
  getApplicationInfoSpace()->fireItemAvailable("runNumber",&runNumber_);

  getApplicationInfoSpace()->fireItemAvailable("readoutMode",&readoutMode_);
  getApplicationInfoSpace()->fireItemAvailable("transport",&transport_);
  getApplicationInfoSpace()->fireItemAvailable("collectorsClassName",&collectorsClassName_);
  getApplicationInfoSpace()->fireItemAvailable("collectorID",&collectorID_);

  getApplicationInfoSpace()->fireItemAvailable("committedPoolSize",&committedPoolSize_);
  getApplicationInfoSpace()->fireItemAvailable("serversClassName", &serversClassName_);
  getApplicationInfoSpace()->fireItemAvailable("serverTIDs", &serverTIDs_);
  getApplicationInfoSpace()->fireItemAvailable("nEventCredits",    &nEventCredits_);
  getApplicationInfoSpace()->fireItemAvailable("prescalingFactor", &prescalingFactor_);

  getApplicationInfoSpace()->fireItemAvailable("inputDataFormat", &inputDataFormat_);
  getApplicationInfoSpace()->fireItemAvailable("inputDeviceType", &inputDeviceType_);
  getApplicationInfoSpace()->fireItemAvailable("inputDeviceName", &inputDeviceName_);

  getApplicationInfoSpace()->fireItemAvailable("xmlCfgFile",&xmlHistosBookingCfgFile_);
  getApplicationInfoSpace()->fireItemAvailable("xmlCanvasesCfgFile",&xmlCanvasesCfgFile_);
  getApplicationInfoSpace()->fireItemAvailable("plotterSaveTimer",&plotterSaveTimer_);
  getApplicationInfoSpace()->fireItemAvailable("outputROOTFile",&outputROOTFile_);
  getApplicationInfoSpace()->fireItemAvailable("fSaveROOTFile",&fSaveROOTFile_);
  getApplicationInfoSpace()->fireItemAvailable("outputImagesPath",&outputImagesPath_);
  getApplicationInfoSpace()->fireItemAvailable("dduCheckMask",&dduCheckMask_);
  getApplicationInfoSpace()->fireItemAvailable("binCheckMask",&binCheckMask_);
  getApplicationInfoSpace()->fireItemAvailable("cscMapFile",&cscMapFile_);

  getApplicationInfoSpace()->fireItemAvailable("useAltFileReader",&useAltFileReader_);
  getApplicationInfoSpace()->fireItemAvailable("stateName",&stateName_);
  getApplicationInfoSpace()->fireItemAvailable("stateChangeTime",&stateChangeTime_);
  getApplicationInfoSpace()->fireItemAvailable("lastEventTime",&lastEventTime_);
  getApplicationInfoSpace()->fireItemAvailable("nDAQEvents",&nDAQEvents_);
  getApplicationInfoSpace()->fireItemAvailable("daqGroup",&daqGroup_);
  getApplicationInfoSpace()->addItemChangedListener ("cscMapFile", this);

  getApplicationInfoSpace()->addItemChangedListener ("readoutMode", this);
  getApplicationInfoSpace()->addItemChangedListener ("transport", this);
  getApplicationInfoSpace()->addItemChangedListener ("collectorsClassName", this);
  //  getApplicationInfoSpace()->addItemChangedListener ("committedPoolSize",this);
  getApplicationInfoSpace()->addItemChangedListener ("serversClassName", this);
  getApplicationInfoSpace()->addItemChangedListener ("serverTIDs", this);
  getApplicationInfoSpace()->addItemChangedListener ("nEventCredits",    this);
  getApplicationInfoSpace()->addItemChangedListener ("prescalingFactor", this);
  
  getApplicationInfoSpace()->addItemChangedListener ("inputDataFormat", this);
  getApplicationInfoSpace()->addItemChangedListener ("inputDeviceType", this);
  getApplicationInfoSpace()->addItemChangedListener ("inputDeviceName", this);

  getApplicationInfoSpace()->addItemChangedListener ("xmlCfgFile", this);
  getApplicationInfoSpace()->addItemChangedListener ("xmlCanvasesCfgFile", this);
  getApplicationInfoSpace()->addItemChangedListener ("outputROOTFile", this);
  getApplicationInfoSpace()->addItemChangedListener ("fSaveROOTFile", this);
  getApplicationInfoSpace()->addItemChangedListener ("plotterSaveTimer", this);
  getApplicationInfoSpace()->addItemChangedListener ("dduCheckMask", this);
  getApplicationInfoSpace()->addItemChangedListener ("binCheckMask", this);
  getApplicationInfoSpace()->addItemChangedListener ("daqGroup", this);

  getApplicationInfoSpace()->addItemChangedListener ("useAltFileReader", this);

  getApplicationInfoSpace()->addItemRetrieveListener ("totalEvents", this);
  getApplicationInfoSpace()->addItemRetrieveListener ("sessionEvents", this);
  getApplicationInfoSpace()->addItemRetrieveListener ("dataBw", this);
  getApplicationInfoSpace()->addItemRetrieveListener ("dataLatency", this);
  getApplicationInfoSpace()->addItemRetrieveListener ("dataRate", this);
  getApplicationInfoSpace()->addItemRetrieveListener ("averageRate", this);
  getApplicationInfoSpace()->addItemRetrieveListener ("stateName", this);
  getApplicationInfoSpace()->addItemRetrieveListener ("cscRate", this);
  getApplicationInfoSpace()->addItemRetrieveListener ("nDAQEvents", this);

};


// == Define Application FSM == //
void EmuMonitor::defineFSM() 
{
  fsm_.addState ('H', "Halted");
  fsm_.addState ('R', "Ready");
  fsm_.addState ('E', "Enabled");
  fsm_.addStateTransition ('H','R', "Configure", this, &EmuMonitor::ConfigureAction);
  fsm_.addStateTransition ('H','E', "Enable", this, &EmuMonitor::EnableAction);
  fsm_.addStateTransition ('R','R', "Configure", this, &EmuMonitor::noAction);
  fsm_.addStateTransition ('R','E', "Enable", this, &EmuMonitor::EnableAction);
  fsm_.addStateTransition ('E','E', "Enable", this, &EmuMonitor::noAction);
  fsm_.addStateTransition ('E','R', "Configure", this, &EmuMonitor::ConfigureAction);
  fsm_.addStateTransition ('R','H', "Halt", this, &EmuMonitor::HaltAction);
  fsm_.addStateTransition ('E','H', "Halt", this, &EmuMonitor::HaltAction);
  fsm_.addStateTransition ('H','H', "Halt", this, &EmuMonitor::HaltAction);
  fsm_.setInitialState('H');
  fsm_.reset();
};

// == Define Web state machine == //
void EmuMonitor::defineWebSM() 
{
  wsm_.addState('H', "Halted",    this, &EmuMonitor::stateMachinePage);
  wsm_.addState('R', "Ready",     this, &EmuMonitor::stateMachinePage);
  wsm_.addState('E', "Enabled",   this, &EmuMonitor::stateMachinePage);
  wsm_.addStateTransition('H','R', "Configure", this, &EmuMonitor::Configure, &EmuMonitor::failurePage);
  wsm_.addStateTransition('R','E', "Enable",    this, &EmuMonitor::Enable,    &EmuMonitor::failurePage);
  wsm_.addStateTransition('R','H', "Halt",      this, &EmuMonitor::Halt,      &EmuMonitor::failurePage);
  wsm_.addStateTransition('E','H', "Halt",      this, &EmuMonitor::Halt,      &EmuMonitor::failurePage);
  wsm_.addStateTransition('H','H', "Halt",      this, &EmuMonitor::Halt, &EmuMonitor::failurePage);
  wsm_.setInitialState('H');
};

void EmuMonitor::setMemoryPool()
{
  if (pool_ != NULL) {
    delete pool_;
    pool_ = NULL;
  }
  try
    {
      LOG4CPLUS_INFO (getApplicationLogger(), "Committed pool size is " << (uint32_t) committedPoolSize_);
      toolbox::mem::CommittedHeapAllocator* a = new toolbox::mem::CommittedHeapAllocator(committedPoolSize_);
      toolbox::net::URN urn ("toolbox-mem-pool", "EmuMonitor_EMU_MsgPool");
      pool_ = toolbox::mem::getMemoryPoolFactory()->createPool(urn, a);
      //            LOG4CPLUS_INFO (getApplicationLogger(), "Set high watermark to 90% and low watermark to 70%");
      LOG4CPLUS_INFO (getApplicationLogger(), "Set event credit message buffer's high watermark to 70%");
      pool_->setHighThreshold ( (uint32_t) (committedPoolSize_ * 0.7));
      //            pool_->setLowThreshold ((uint32_t) (committedPoolSize_ * 0.7));
      //  hasSet_committedPoolSize_ = true;
    }
  catch(toolbox::mem::exception::Exception & e)
    {
      LOG4CPLUS_FATAL (getApplicationLogger(), toolbox::toString("Could not set up memory pool, %s (exiting thread)", e.what()));
      return;
    }
	

}

void EmuMonitor::setupPlotter() 
{
  if (plotter_ != NULL) {
    delete plotter_;
    plotter_ = NULL;
  }

  // plotter_ = new EmuPlotter(getApplicationLogger());
  plotter_ = new EmuPlotter(this->getApplicationLogger());
//  plotter_->setLogLevel(WARN_LOG_LEVEL);
  plotter_->setUnpackingLogLevel(OFF_LOG_LEVEL);
  if (xmlHistosBookingCfgFile_ != "") plotter_->setXMLHistosBookingCfgFile(xmlHistosBookingCfgFile_.toString());
  if (xmlCanvasesCfgFile_ != "") plotter_->setXMLCanvasesCfgFile(xmlCanvasesCfgFile_.toString());
  if (cscMapFile_ != "") plotter_->setCSCMapFile(cscMapFile_.toString());

}

// == Bind I2O Callbacks ==//
void EmuMonitor::bindI2Ocallbacks()
{
  i2o::bind(this, &EmuMonitor::emuDataMsg, I2O_EMUMONITOR_CODE, XDAQ_ORGANIZATION_ID );
}


// == Bind SOAP Callbacks == //
void EmuMonitor::bindSOAPcallbacks() 
{
  // == Bind SOAP callbacks for control messages
  xoap::bind (this, &EmuMonitor::fireEvent, "Configure", XDAQ_NS_URI);
  xoap::bind (this, &EmuMonitor::fireEvent, "Enable", XDAQ_NS_URI);
  xoap::bind (this, &EmuMonitor::fireEvent, "Halt", XDAQ_NS_URI);
  xoap::bind(this, &EmuMonitor::onReset,    "Reset", XDAQ_NS_URI);

  xoap::bind (this, &EmuMonitor::requestObjectsList, "requestObjectsList", XDAQ_NS_URI);
  xoap::bind (this, &EmuMonitor::requestObjects, "requestObjects", XDAQ_NS_URI);
  xoap::bind (this, &EmuMonitor::requestCanvasesList, "requestCanvasesList", XDAQ_NS_URI);
  xoap::bind (this, &EmuMonitor::requestCanvas, "requestCanvas", XDAQ_NS_URI);
  xoap::bind (this, &EmuMonitor::requestFoldersList, "requestFoldersList", XDAQ_NS_URI);
}

// == Bind CGI Callbacks ==//
void EmuMonitor::bindCGIcallbacks()
{
  xgi::bind(this, &EmuMonitor::dispatch, "dispatch");
  xgi::bind(this, &EmuMonitor::showStatus, "showStatus");
  xgi::bind(this, &EmuMonitor::showControl, "showControl");
  maxFrameSize_ = sizeof(I2O_EMUMONITOR_CREDIT_MESSAGE_FRAME);
}


// == SOAP Callback trigger state change == //
xoap::MessageReference EmuMonitor::fireEvent (xoap::MessageReference msg) throw (xoap::exception::Exception)
{
  xoap::SOAPPart part = msg->getSOAPPart();
  xoap::SOAPEnvelope env = part.getEnvelope();
  xoap::SOAPBody body = env.getBody();
  DOMNode* node = body.getDOMNode();
  DOMNodeList* bodyList = node->getChildNodes();
  for (unsigned int i = 0; i < bodyList->getLength(); i++)
    {
      DOMNode* command = bodyList->item(i);

      if (command->getNodeType() == DOMNode::ELEMENT_NODE)
	{
	  std::string commandName = xoap::XMLCh2String (command->getLocalName());

	  try
	    {
	      toolbox::Event::Reference e(new toolbox::Event(commandName, this));
	      fsm_.fireEvent(e);
	      // Synchronize Web state machine
	      wsm_.setInitialState(fsm_.getCurrentState());
	      stateChangeTime_ = now();
	    }
	  catch (toolbox::fsm::exception::Exception & e)
	    {
	      XCEPT_RETHROW(xcept::Exception, "invalid command", e);
	    }

	  xoap::MessageReference reply = xoap::createMessage();
	  xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
	  xoap::SOAPName responseName = envelope.createName( commandName +"Response", "xdaq", XDAQ_NS_URI);
	  envelope.getBody().addBodyElement ( responseName );
	  return reply;
	}
    }

  XCEPT_RAISE(xoap::exception::Exception,"command not found");
}

xoap::MessageReference EmuMonitor::onReset(xoap::MessageReference msg)
  throw (xoap::exception::Exception)
{
  std::string commandName = "Reset";
  
  try{
    fsm_.reset();
  }
  catch( toolbox::fsm::exception::Exception e ){
    XCEPT_RETHROW(xoap::exception::Exception,
                  "Failed to reset FSM: ", e);
  }

  toolbox::Event::Reference evtRef(new toolbox::Event("Halt", this));
  fsm_.fireEvent( evtRef );

  xoap::MessageReference reply = xoap::createMessage();
  xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
  xoap::SOAPName responseName = envelope.createName( commandName +"Response", "xdaq", XDAQ_NS_URI);
  envelope.getBody().addBodyElement ( responseName );
  return reply;
}





// == Run Control requests current parameter values == //
void EmuMonitor::actionPerformed (xdata::Event& e)
{
  // update measurements monitors
  if (e.type() == "ItemRetrieveEvent")
    {
      std::string item = dynamic_cast<xdata::ItemRetrieveEvent&>(e).itemName();
      if ( item == "totalEvents")
	{
	  // LOG4CPLUS_INFO(getApplicationLogger(), "Total Events : " << totalEvents_.toString());
	} 
      else if ( item == "sessionEvents")
	{
	  // LOG4CPLUS_INFO(getApplicationLogger(), "Session Events: " << sessionEvents_.toString());
	} 
      else if ( item == "dataBw")
	{
	  dataBw_ = toolbox::toString("%.2f",pmeter_->bandwidth());
	  // LOG4CPLUS_INFO(getApplicationLogger(), "Data Bandwidth: " << dataBw_.toString());
	}
      else if ( item == "dataLatency")
        {
          dataRate_ =  toolbox::toString("%f",pmeter_->latency());
          // LOG4CPLUS_INFO(getApplicationLogger(), "Data Latency: " << dataLatency_.toString());
        }
      else if ( item == "dataRate")
        {
	  
	  dataRate_ = toolbox::toString("%.2f",pmeter_->rate());
          // LOG4CPLUS_INFO(getApplicationLogger(), "Data Rate: " << dataRate_.toString());
        }
      else if ( item == "averageRate")
        {
		averageRate_ = rateMeter->getRate("averageRate");
		// LOG4CPLUS_INFO(getApplicationLogger(), "Average Data Rate: " << averageRate_.toString());
        }
      else if ( item == "cscRate")
        {

		cscRate_ = rateMeter->getRate("cscRate");
  //        cscRate_ = toolbox::toString("%.2f",pmeterCSC_->rate());
          // LOG4CPLUS_INFO(getApplicationLogger(), "Data Rate: " << dataRate_.toString());
        }
      else if ( item == "stateName")
        {
	  stateName_ = wsm_.getStateName(wsm_.getCurrentState());
          // LOG4CPLUS_INFO(getApplicationLogger(), "State: " << stateName_.toString());
        }
      else if ( item == "nDAQEvents")
        {
	  nDAQEvents_ = 0;
	  std::set<xdaq::ApplicationDescriptor*>::iterator pos;
	  for (pos=dataservers_.begin(); pos!=dataservers_.end(); ++pos) {
	    xdata::UnsignedLong count;
	    count.fromString(emu::dqm::getScalarParam(getApplicationContext(), getApplicationDescriptor(), (*pos),"nEventsRead","unsignedLong"));
	    nDAQEvents_ = nDAQEvents_ + count;	
          }
        }


    };
  if (e.type() == "ItemChangedEvent")
    {
      std::string item = dynamic_cast<xdata::ItemChangedEvent&>(e).itemName();
      // std::cout << item << std::endl;

      if ( item == "readoutMode")
        {
          LOG4CPLUS_INFO(getApplicationLogger(), "Readout Mode : " << readoutMode_.toString());
	  disableReadout();
	  configureReadout();
        }
      else if ( item == "committedPoolSize")
        {
          LOG4CPLUS_INFO(getApplicationLogger(),
                         toolbox::toString("EmuMonitor's Tid: %d",
                                           i2o::utils::getAddressMap()->getTid(this->getApplicationDescriptor())) );
	  setMemoryPool();
        }
      else if ( item == "serversClassName")
        {
          LOG4CPLUS_INFO(getApplicationLogger(), "Data Servers Class Name : " << serversClassName_.toString());
	  getDataServers(serversClassName_);
        }
      else if ( item == "serverTIDs" || item == "daqGroup")
        {
          LOG4CPLUS_INFO(getApplicationLogger(), "List of Servers TIDs changed : ");
          getDataServers(serversClassName_);
        }

      else if ( item == "collectorsClassName")
        {
          LOG4CPLUS_INFO(getApplicationLogger(), "Collectors Class Name : " << collectorsClassName_.toString());
          getCollectors(collectorsClassName_);
        }
      else if ( item == "inputDeviceType")
        {
          LOG4CPLUS_INFO(getApplicationLogger(), "Input Device Type : " << inputDeviceType_.toString());
	  // destroyDeviceReader();
	  // createDeviceReader();
        }
      else if ( item == "nEventCredits")
        {
          LOG4CPLUS_INFO(getApplicationLogger(), "Events Credits : " << nEventCredits_.toString());
	  defEventCredits_=nEventCredits_;
        }
      else if ( item == "prescalingFactor")
        {
          LOG4CPLUS_INFO(getApplicationLogger(), "Prescaling Factor : " << prescalingFactor_.toString());
          // destroyDeviceReader();
          // createDeviceReader();
        }

      else if ( item == "inputDeviceName")
        {
          LOG4CPLUS_INFO(getApplicationLogger(), "Input Device Name : " << inputDeviceName_.toString());
          // destroyDeviceReader();
          // createDeviceReader();
        }
      else if ( item == "inputDataFormat")
        {
          LOG4CPLUS_INFO(getApplicationLogger(), "Input Data Format : " << inputDataFormat_.toString());
          // destroyDeviceReader();
          // createDeviceReader();
        }
      else if ( item == "useAltFileReader")
        {
          LOG4CPLUS_INFO(getApplicationLogger(), "Use Alt File Reader : " << useAltFileReader_.toString());
          // destroyDeviceReader();
          // createDeviceReader();
        } 
      else if ( item == "plotterSaveTimer")
        {
          LOG4CPLUS_INFO(getApplicationLogger(), "Plotter Save Timer : " << plotterSaveTimer_.toString());
	  if ((plotterSaveTimer_>xdata::Integer(0))
	      && (timer_ != NULL)) {
	    timer_->setTimer(plotterSaveTimer_);
    	  }

        }
      else if ( item == "binCheckMask")
        {
          LOG4CPLUS_INFO(getApplicationLogger(), "Binary Checks Mask : 0x" << std::hex << std::uppercase << binCheckMask_ << std::dec);
												      if (binCheckMask_ >= xdata::UnsignedInteger(0)
													  && (plotter_ != NULL)) {
													plotter_->setBinCheckMask(binCheckMask_);
												      }

        }
      else if ( item == "dduCheckMask")
        {
          LOG4CPLUS_INFO(getApplicationLogger(), "DDU Checks Mask : 0x" << std::hex << std::uppercase << dduCheckMask_ << std::dec);
												   if (dduCheckMask_ >= xdata::UnsignedInteger(0)
												       && (plotter_ != NULL)) {
												     plotter_->setDDUCheckMask(dduCheckMask_);
												   }

        }
      else if ( item == "outputROOTFile")
        {
          LOG4CPLUS_INFO(getApplicationLogger(), "Output ROOT File : " << outputROOTFile_.toString());
          if (outputROOTFile_.toString().length()
	      && (plotter_ != NULL)) {
	    plotter_->setHistoFile(outputROOTFile_);
          }
        }
      else if ( item == "xmlCfgFile")
        {
	  
          LOG4CPLUS_INFO(getApplicationLogger(), 
			 "Histograms Booking XML Config File for plotter : " << xmlHistosBookingCfgFile_.toString());
	  if (plotter_ != NULL) {
	    plotter_->setXMLHistosBookingCfgFile(xmlHistosBookingCfgFile_.toString());
	  }
	  
        }
      else if ( item == "xmlCanvasesCfgFile") 
	{
	  LOG4CPLUS_INFO(getApplicationLogger(),
			 "Canvases XML Config File for plotter : " << xmlCanvasesCfgFile_.toString());
	  if (plotter_ != NULL) {
	    plotter_->setXMLCanvasesCfgFile(xmlCanvasesCfgFile_.toString());
          }

	}
      else if ( item == "cscMapFile")
        {
          LOG4CPLUS_INFO(getApplicationLogger(),
                         "CSC Mapping File for plotter : " << cscMapFile_.toString());
          if (plotter_ != NULL) {
            plotter_->setCSCMapFile(cscMapFile_.toString());
          }

        }

    }
}

std::string EmuMonitor::getROOTFileName() 
{
  std::string histofile="dqm_results";
  std::string path=outputROOTFile_;
  if (path.rfind("/") != std::string::npos) {
    path = path.substr(0, path.rfind("/")+1);
  } else path="";
   
  if (readoutMode_.toString() == "internal") {
    if (inputDeviceName_.toString().rfind(".") != std::string::npos) {
      histofile = inputDeviceName_.toString();
      if (histofile.rfind("/") != std::string::npos)
	histofile.erase(0, histofile.rfind("/")+1);
      if (histofile.rfind(".") != std::string::npos)
	histofile.erase(histofile.rfind("."), histofile.size());
    }
  }
  if (readoutMode_.toString() == "external") {
    if (serversClassName_.toString() != "") { 
      std::ostringstream st;
      st.clear();
      st << "online_" << std::setw(8) << std::setfill('0') << runNumber_ << "_" << serversClassName_.toString();
	st << std::setw(2) << std::setfill('0') << appTid_ << "_" << getDateTime();
      /*
	std::set<xdaq::ApplicationDescriptor*>::iterator pos;
      for (pos=dataservers_.begin(); pos!=dataservers_.end(); ++pos) {
	st << "_" << (*pos)->getInstance();
      }
	*/
      histofile = st.str();
    }
  }

   
   
  return (path+histofile+".root");
}

// == Get Application Descriptors for specified Data Server class name == //
void EmuMonitor::getDataServers(xdata::String className) 
{
  try
    {
      dataservers_.clear();
      if (serverTIDs_.size() > 0) {
	std::set<xdaq::ApplicationDescriptor*> tmpdataservers_;
	//	tmpdataservers_ = getApplicationContext()->getApplicationGroup()->getApplicationDescriptors(className.toString().c_str());
        xdaq::ApplicationGroup *g = getApplicationContext()->getDefaultZone()->getApplicationGroup(daqGroup_);
        tmpdataservers_  = g->getApplicationDescriptors(className.toString().c_str());
	std::set<xdaq::ApplicationDescriptor*>::iterator pos;
        for (pos=tmpdataservers_.begin(); pos!=tmpdataservers_.end(); ++pos) {
          for (uint32_t j = 0; j<serverTIDs_.size(); j++) {
            if ((serverTIDs_.at(j) != xdata::UnsignedInteger(0)) && (serverTIDs_.at(j) == xdata::UnsignedInteger(i2o::utils::getAddressMap()->getTid(*pos))))
              dataservers_.insert((*pos));
          }
        }

      } else {
	xdaq::ApplicationGroup *g = getApplicationContext()->getDefaultZone()->getApplicationGroup(daqGroup_);
        dataservers_  = g->getApplicationDescriptors(className.toString().c_str());

	//	dataservers_ = getApplicationContext()->getApplicationGroup()->getApplicationDescriptors(className.toString().c_str());
      }
      if (dataservers_.size() == 0) {
	LOG4CPLUS_ERROR (getApplicationLogger(),
			 "No " << className.toString() << " Data Servers matching serverTIDs list found."); 
      }
      //hasSet_serversClassName_ = true;
    }
  catch (xdaq::exception::Exception& e)
    {
      LOG4CPLUS_ERROR (getApplicationLogger(),
		       "No Data Servers with class name " << className.toString() <<
		       "found. EmuMonitor cannot be configured." <<
		       xcept::stdformat_exception_history(e));
      //            XCEPT_RETHROW (xdaq::exception::Exception,
      //                           toolbox::toString("No %s application instance found. EmuMonitor cannot be configured.",
      //                                             serversClassName_.toString().c_str()),
      //                           e);
    }
}

// == Get Application Descriptors for specified Data Server class name == //
void EmuMonitor::getCollectors(xdata::String className)
{
  try
    {
      collectors_.clear();
      // collectors_ = getApplicationContext()->getApplicationGroup()->getApplicationDescriptors(className.toString().c_str());
      xdaq::ApplicationGroup *g = getApplicationContext()->getDefaultZone()->getApplicationGroup("dqm");
      collectors_  =    g->getApplicationDescriptors(className.toString().c_str());

      //hasSet_serversClassName_ = true;
    }
  catch (xdaq::exception::Exception& e)
    {
      LOG4CPLUS_ERROR (getApplicationLogger(),
		       "No Collectors with class name " << className.toString() <<
		       "found. EmuMonitor cannot be configured." <<
		       xcept::stdformat_exception_history(e));
      //            XCEPT_RETHROW (xdaq::exception::Exception,
      //                           toolbox::toString("No %s application instance found. EmuMonitor cannot be configured.",
      //                                             serversClassName_.toString().c_str()),
      //                           e);
    }
}


void EmuMonitor::ConfigureAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception)
{
  //  doStop();
  doConfigure();
  LOG4CPLUS_WARN(getApplicationLogger(), e->type() << " from "
                 << fsm_.getStateName(fsm_.getCurrentState()));
}

void EmuMonitor::doStop()
{
  appBSem_.take();
  if (plotter_ != NULL && isReadoutActive) { 
    if (timer_ != NULL && timer_->isActive())
      timer_->kill();	
    if (rateMeter != NULL && rateMeter->isActive()) {
	rateMeter->kill();
	}
    if (fSaveROOTFile_ == xdata::Boolean(true) && (sessionEvents_ > xdata::UnsignedInteger(0))) {
	
      if (timer_ != NULL) {
	timer_->setPlotter(plotter_);
	timer_->setROOTFileName(getROOTFileName());
	if (plotterSaveTimer_>xdata::Integer(0)) {
	  timer_->setTimer(plotterSaveTimer_);
	}
	timer_->activate();

      }
	      
//      plotter_->saveToROOTFile(getROOTFileName());
    }
   
  }
  disableReadout();
  pmeter_->init(200);
  pmeterCSC_->init(200);
  appBSem_.give();

}

void EmuMonitor::doConfigure()
{
  appBSem_.take();
  disableReadout();
  pmeter_->init(200);
  pmeterCSC_->init(200);
  // !! EmuMonitor or EmuRUI tid?
  // appTid_ = i2o::utils::getAddressMap()->getTid(this->getApplicationDescriptor());
  /*
    if (xmlCfgFile_ != "" && plotter_ != NULL) plotter_->setXMLCfgFile(xmlCfgFile_.toString());
    if (plotter_ != NULL) plotter_->book(appTid_);
  */
  if (timer_ != NULL && timer_->isActive())
    timer_->kill();

  setupPlotter();
  if (plotter_ != NULL) {
    // timer_->setPlotter(plotter_);
    if (outputROOTFile_.toString().length()) {
      LOG4CPLUS_INFO (getApplicationLogger(),
                      "plotter::outputROOTFile: 0x" << outputROOTFile_.toString());
      plotter_->setHistoFile(outputROOTFile_);
    }
    if (dduCheckMask_ >= xdata::UnsignedInteger(0)) {
      LOG4CPLUS_INFO (getApplicationLogger(),
                      "plotter::dduCheckMask: 0x" << std::hex << strtoul((dduCheckMask_.toString()).c_str(),NULL,0));
      plotter_->setDDUCheckMask(dduCheckMask_);
    }
    if (binCheckMask_ >= xdata::UnsignedInteger(0)) {
      LOG4CPLUS_INFO (getApplicationLogger(),
                      "plotter::binCheckMask: 0x" << std::hex << strtoul((binCheckMask_.toString()).c_str(),NULL,0));
      plotter_->setBinCheckMask(binCheckMask_);
    }

    plotter_->book();
  }
  /*
    if (plotterSaveTimer_>xdata::Integer(0)) {
    timer_->setTimer(plotterSaveTimer_);
    }
  */
  configureReadout();
  appBSem_.give();

}

void  EmuMonitor::doStart()
{
  appBSem_.take();
  sessionEvents_=0;
  creditMsgsSent_ = 0;
  creditsHeld_ = 0;
  eventsRequested_ = 0;
  eventsReceived_ = 0;
  cscUnpacked_ = 0;
  runNumber_ = 0;
  nEventCredits_ = defEventCredits_;
  
  //    configureReadout();
  // if (plotter_ != NULL) timer_->activate();
  if (timer_ != NULL && timer_->isActive())
    timer_->kill();
  enableReadout();
  pmeter_->init(200);
  pmeterCSC_->init(200);
  if (rateMeter != NULL) {
        rateMeter->init();
        rateMeter->activate();
  }
  appBSem_.give();
  // bindI2Ocallbacks();
}

void EmuMonitor::HaltAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception )
{
  doStop();
  LOG4CPLUS_WARN(getApplicationLogger(), e->type() << " from "
                 << fsm_.getStateName(fsm_.getCurrentState()));
}

void EmuMonitor::EnableAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception )
{
  if (fsm_.getCurrentState() == 'H') doConfigure();
  doStart();
  LOG4CPLUS_WARN(getApplicationLogger(), e->type() << " from " 
                 << fsm_.getStateName(fsm_.getCurrentState()));
}

void EmuMonitor::noAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception )
{
  LOG4CPLUS_WARN(getApplicationLogger(), "Attempt of invalid FSM state change to " <<  e->type()
                 << " from "
                 << fsm_.getStateName(fsm_.getCurrentState()) << ". Ignored.");
  fsm_.setInitialState(fsm_.getCurrentState());
}




// == Web Events that trigger state changes (result of wsm::fireEvent) == //
void EmuMonitor::Configure(xgi::Input * in ) throw (xgi::exception::Exception)
{
  try
    {
      toolbox::Event::Reference e(new toolbox::Event("Configure", this));
      fsm_.fireEvent(e);
    }
  catch (toolbox::fsm::exception::Exception & e)
    {
      XCEPT_RETHROW(xgi::exception::Exception, "invalid command", e);
    }
}

void EmuMonitor::Enable(xgi::Input * in ) throw (xgi::exception::Exception)
{
  try
    {
      toolbox::Event::Reference e(new toolbox::Event("Enable", this));
      fsm_.fireEvent(e);
    }
  catch (toolbox::fsm::exception::Exception & e)
    {
      XCEPT_RETHROW(xgi::exception::Exception, "invalid command", e);
    }
}

void EmuMonitor::Halt(xgi::Input * in ) throw (xgi::exception::Exception)
{
  try
    {
      toolbox::Event::Reference e(new toolbox::Event("Halt", this));
      fsm_.fireEvent(e);
    }
  catch (toolbox::fsm::exception::Exception & e)
    {
      XCEPT_RETHROW(xgi::exception::Exception, "invalid command", e);
    }
}



void EmuMonitor::emuDataMsg(toolbox::mem::Reference *bufRef){
  // Emu-specific stuff
  creditsHeld_ = ((I2O_EMU_DATA_MESSAGE_FRAME*)bufRef->getDataLocation())->nEventCreditsHeld;
  if (fsm_.getCurrentState() != 'E') {
    LOG4CPLUS_WARN(getApplicationLogger(),"Dropping received Data. Not in Enabled state.");
	
    LOG4CPLUS_WARN(getApplicationLogger(),"EmuRUI holding " << creditsHeld_ << " event credits");
    bufRef->release();
    return;	
  }

  appBSem_.take();
  dataMessages_.push_back( bufRef );
  eventsReceived_++;

  //   // Process the oldest message, i.e., the one at the front of the queue
  toolbox::mem::Reference *oldestMessage = dataMessages_.front();

  I2O_EMU_DATA_MESSAGE_FRAME *msg =
    (I2O_EMU_DATA_MESSAGE_FRAME*)oldestMessage->getDataLocation();

  char *startOfPayload = (char*) oldestMessage->getDataLocation()
    + sizeof(I2O_EMU_DATA_MESSAGE_FRAME);

  uint32_t sizeOfPayload = oldestMessage->getDataSize()-sizeof(I2O_EMU_DATA_MESSAGE_FRAME);

  uint32_t errorFlag = msg->errorFlag;
  uint32_t serverTID = msg->PvtMessageFrame.StdMessageFrame.InitiatorAddress;
  uint32_t status = 0;
  runNumber_ = msg->runNumber;
  
  if( errorFlag==EmuFileReader::Type2 ) status |= 0x8000;
  if( errorFlag==EmuFileReader::Type3 ) status |= 0x4000;
  if( errorFlag==EmuFileReader::Type4 ) status |= 0x2000;
  if( errorFlag==EmuFileReader::Type5 ) status |= 0x1000;
  if( errorFlag==EmuFileReader::Type6 ) status |= 0x0800;
  
/*
  if (eventsReceived_%200 == 0) {
    time_t nowmark=time(NULL);
    averageRate_ = sessionEvents_/(nowmark-startmark);
  }
*/
 
  if (eventsReceived_%1000 == 0) { 
    LOG4CPLUS_WARN(getApplicationLogger(),
		   // "Received " << bufRef->getDataSize() <<
		   "Received evt#" << eventsReceived_ << " (req: " << eventsRequested_ << ")" <<
		   sizeOfPayload << " bytes, run " << msg->runNumber <<
		   ", errorFlag 0x"  << std::hex << status << std::dec <<
		   " from " << serversClassName_.toString() <<
		   ":" << serverTID <<
		   ", still holding " << creditsHeld_ << " event credits, " <<
		   "pool size " << dataMessages_.size());
/*
    LOG4CPLUS_INFO(getApplicationLogger(),
		   "==> Average rate: " << averageRate_ << " evt/sec");
    LOG4CPLUS_INFO(getApplicationLogger(),
		   "==> Current rate: " << pmeter_->rate() << " evt/sec");
*/
  }

  if (status == 0) processEvent(reinterpret_cast<const char *>(startOfPayload), sizeOfPayload, status, serverTID);

  // Free the Emu data message
  bufRef->release();
  dataMessages_.erase( dataMessages_.begin() );
  appBSem_.give();
  //  eventsReceived_++;
}

// == Send an I2O token message to all servers == //
int EmuMonitor::sendDataRequest(uint32_t last)
{
  //  creditMsgsSent += 1;
  std::set<xdaq::ApplicationDescriptor*>::iterator pos;
  for (pos=dataservers_.begin(); pos!=dataservers_.end(); ++pos)
    {
      if (eventsReceived_ > xdata::UnsignedInteger(1000)) {
	uint32_t newRate = averageRate_;
	if (newRate>10) newRate=(newRate/10)*10;
	if ((newRate>=defEventCredits_) && (newRate <=500)) {
	  LOG4CPLUS_WARN (getApplicationLogger(), "Adjusting nEventCredits to " << newRate);
	  nEventCredits_ = newRate;
	}
      }

      creditMsgsSent_ ++;
      eventsRequested_ = eventsRequested_ +  nEventCredits_;
      toolbox::mem::Reference * ref = 0;
      try
        {
          ref = toolbox::mem::getMemoryPoolFactory()->getFrame(pool_, maxFrameSize_);

          PI2O_EMUMONITOR_CREDIT_MESSAGE_FRAME frame = (PI2O_EMUMONITOR_CREDIT_MESSAGE_FRAME) ref->getDataLocation();


          frame->PvtMessageFrame.StdMessageFrame.MsgFlags         = 0;
          frame->PvtMessageFrame.StdMessageFrame.VersionOffset    = 0;
          frame->PvtMessageFrame.StdMessageFrame.TargetAddress    = i2o::utils::getAddressMap()->getTid((*pos));
          frame->PvtMessageFrame.StdMessageFrame.InitiatorAddress = i2o::utils::getAddressMap()->getTid(this->getApplicationDescriptor());
          frame->PvtMessageFrame.StdMessageFrame.MessageSize      = (sizeof(I2O_EMUMONITOR_CREDIT_MESSAGE_FRAME)) >> 2;

          frame->PvtMessageFrame.StdMessageFrame.Function = I2O_PRIVATE_MESSAGE;
          frame->PvtMessageFrame.XFunctionCode            = I2O_EMUMONITOR_CODE;
          frame->PvtMessageFrame.OrganizationID           = XDAQ_ORGANIZATION_ID;

          frame->nEventCredits    = nEventCredits_;
          frame->prescalingFactor = prescalingFactor_;

          ref->setDataSize(frame->PvtMessageFrame.StdMessageFrame.MessageSize << 2);
          LOG4CPLUS_WARN(getApplicationLogger(),
			 "Sending credit #" << creditMsgsSent_ << " to tid: " << frame->PvtMessageFrame.StdMessageFrame.TargetAddress
			 //                                   << ". maxFrameSize=" << maxFrameSize_
			 );
          getApplicationContext()->postFrame(ref, this->getApplicationDescriptor(), (*pos));
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
		  getApplicationContext()->postFrame(ref,  this->getApplicationDescriptor(), (*pos));
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

// == Configure Readout == //
void EmuMonitor::configureReadout() 
{
  sessionEvents_=0;
  if (readoutMode_.toString() == "internal") {
    LOG4CPLUS_DEBUG(getApplicationLogger(),
		    "Configure Readout for internal mode");
    destroyDeviceReader();
    createDeviceReader();
  } else if (readoutMode_.toString() == "external") {
    LOG4CPLUS_DEBUG(getApplicationLogger(),
		    "Configure Readout for external mode");
    getDataServers(serversClassName_);
  } else {
    LOG4CPLUS_ERROR(getApplicationLogger(),
		    "Unknown Readout Mode: " << readoutMode_.toString() 
		    << "Use \"internal\" or \"external\"");
  }
}

// == Enable Readout == //
void EmuMonitor::enableReadout()
{
  if (!isReadoutActive) {
    keepRunning = true;
    this->activate();
    isReadoutActive = true;
  }
}

// == Disable Readout == //
void EmuMonitor::disableReadout()
{
  if (isReadoutActive) {
    keepRunning = false;
    // this->kill();
    isReadoutActive = false;
  }
}

// == Create Internal input Device Reader == //
void EmuMonitor::createDeviceReader()
{

  if ( (inputDeviceName_.toString() != "") 
       && (readoutMode_.toString() == "internal") )
    {

      // Create reader
      int inputDataFormatInt_ = -1;
      if      ( inputDataFormat_ == "DDU" ) inputDataFormatInt_ = EmuReader::DDU;
      else if ( inputDataFormat_ == "DCC" ) inputDataFormatInt_ = EmuReader::DCC;
      else     LOG4CPLUS_ERROR(getApplicationLogger(),
			       "No such data format: " << inputDataFormat_.toString() <<
			       "Use \"DDU\" or \"DCC\"");
      LOG4CPLUS_INFO(getApplicationLogger(), 
		     "Creating " << inputDeviceType_.toString() <<
		     " reader for " << inputDeviceName_.toString());
      deviceReader_ = NULL;
      try {
	// == Check if file exists
	struct stat stats;
        if (stat((inputDeviceName_.toString()).c_str(), &stats)<0) {
                LOG4CPLUS_FATAL(getApplicationLogger(), inputDeviceType_.toString() << ": " <<
                        strerror(errno));
        } else { 
		if      ( inputDeviceType_ == "spy"  )
	 	 deviceReader_ = new EmuSpyReader(  inputDeviceName_.toString(), inputDataFormatInt_ );
		else if ( inputDeviceType_ == "file" )
		  deviceReader_ = new EmuFileReader( inputDeviceName_.toString(), inputDataFormatInt_ );
		// TODO: slink
		else     LOG4CPLUS_ERROR(getApplicationLogger(),
					 "Bad device type: " << inputDeviceType_.toString() <<
					 "Use \"file\", \"spy\", or \"slink\"");
	}
      }
      catch(char* e){

	std::stringstream oss;
	oss << "Failed to create " << inputDeviceType_.toString()
	    << " reader for "      << inputDeviceName_.toString()
	    << ": "                << e;
	LOG4CPLUS_ERROR(getApplicationLogger(), oss.str())

	  // Don't raise exception as it would be interpreted as FSM transition error
	  //      XCEPT_RAISE(toolbox::fsm::exception::Exception, oss.str());
	  }
    }
}

// == Destroy Internal input Device Reader == //
void EmuMonitor::destroyDeviceReader()
{
  if (deviceReader_ != NULL) {
    LOG4CPLUS_DEBUG(getApplicationLogger(),
		    "Destroying reader for " << deviceReader_->getName() );
    delete deviceReader_;
    deviceReader_ = NULL;
  }
}

int EmuMonitor::svc()
{
  LOG4CPLUS_INFO (getApplicationLogger(), "Starting Readout loop...");
  //  return 0;

  keepRunning = true;
  bool readValid = true;
  startmark=time(NULL);
  sleep(1);

  while(keepRunning && readValid)
    {
      if ( (readoutMode_.toString() == "internal") && 
	   ( (deviceReader_ != NULL))) 
	{
	  readValid = deviceReader_->readNextEvent();
	  if ( !keepRunning || !readValid)
	    {
	      LOG4CPLUS_ERROR(getApplicationLogger(),
			      " " << inputDataFormat_.toString() << " " << inputDeviceType_.toString() <<
			      " " << inputDeviceName_.toString() << " read error.");
	      if (plotter_ != NULL) {
		isReadoutActive = false;
		if (fSaveROOTFile_== xdata::Boolean(true) && (sessionEvents_ > xdata::UnsignedInteger(0)) ) {
		  appBSem_.take();
		  plotter_->saveToROOTFile(getROOTFileName());
		  appBSem_.give();
                }

	      }
	    } else {
	      uint32_t errorFlag = 0;
	      appBSem_.take();      
	      processEvent(deviceReader_->data(), deviceReader_->dataLength(), errorFlag, appTid_);
	      appBSem_.give();
	/*
	      if ((sessionEvents_ % 100) == 1) {
	       	time_t nowmark=time(NULL);	      	      
	        averageRate_ = sessionEvents_/(nowmark-startmark);
	      } 
	*/
	    }

        }
      
      uint32_t timeout = 3*1000000; // 5sec
      uint32_t wait = 10000; // 10ms
      uint32_t waittime = 0;
      if (readoutMode_.toString() == "external") { 
	if (creditsHeld_ > nEventCredits_) {
	  LOG4CPLUS_WARN (getApplicationLogger(), "Waiting to clear " << creditsHeld_ << " event credits from EmuRUI");
        }
	while (creditsHeld_ > nEventCredits_) {
	  usleep(wait);
        }
	if (eventsReceived_ > eventsRequested_) eventsRequested_=eventsReceived_;
	while ((eventsReceived_ < eventsRequested_) && ( waittime <= timeout )) {
	  usleep(wait);
	  waittime += wait;
	} 
	if (waittime >= timeout) {
	  LOG4CPLUS_WARN (getApplicationLogger(), toolbox::toString("Timeout waiting for events from server."));
	  LOG4CPLUS_WARN (getApplicationLogger(), "Missed " << (eventsRequested_ - eventsReceived_) << " events");
	  
	  /*
          time_t nowmark=time(NULL);
          averageRate_ = sessionEvents_/(nowmark-startmark);
	  */

          if ((eventsRequested_ - eventsReceived_) >= nEventCredits_) {
            // nEventCredits_ = ((nEventCredits_-10)>1)? nEventCredits_-10: 1;
            continue;
          }

	  eventsRequested_ = eventsReceived_;
	  
	}
	waittime = 0;
	//usleep(200000);
	if (!pool_->isHighThresholdExceeded())
	  {
	    // Stop if there is an error in sending
	    if (this->sendDataRequest(0) == 1)
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
    }

  return 0;
}


// == Process Event data == //
void EmuMonitor::processEvent(const char * data, int dataSize, uint32_t errorFlag, int node)
{

  if (plotter_ != NULL) {
    totalEvents_++;
    sessionEvents_++;
    LOG4CPLUS_INFO(getApplicationLogger(), "Event #" << sessionEvents_.toString() 
		   << " (Total processed: " << totalEvents_.toString() << ")"); 
    plotter_->processEvent(data, dataSize, errorFlag, node);
    pmeter_->addSample(dataSize);
    cscUnpacked_ = cscUnpacked_ + plotter_->getUnpackedDMBCount();
    for (unsigned i=0; i< plotter_->getUnpackedDMBCount(); i++) {
      pmeterCSC_->addSample(1);
    }
    lastEventTime_ = now();
  }
}



