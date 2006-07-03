/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#include "EmuMonitor.h"


XDAQ_INSTANTIATOR_IMPL(EmuMonitor)

// == EmuMonitor Constructor == //
  EmuMonitor::EmuMonitor(xdaq::ApplicationStub* c)
  throw(xdaq::exception::Exception)
    :xdaq::WebApplication(c)
    ,Task("EmuMonitor")
    ,applicationBSem_(BSem::FULL)
{
  LOG4CPLUS_INFO(this->getApplicationLogger(),"Constructor called");
  
/*  
  getApplicationDescriptor()->setAttribute("icon",
        "/emu/emuDQM/EmuMonitor/images/dqm64x64.gif");
 */

  plotter_ = NULL;
  deviceReader_ = NULL;
  altFileReader_ = NULL;
  pool_ = NULL;
  initProperties();
  setMemoryPool();
  // setupPlotter();
  timer_ = new EmuMonitorTimerTask();

  errorHandler_ = toolbox::exception::bind (this, &EmuMonitor::onError, "onError");

  defineFSM();
  defineWebSM();

  //bindI2Ocallbacks();
  bindSOAPcallbacks();
  bindCGIcallbacks();
  appTid_ = i2o::utils::getAddressMap()->getTid(this->getApplicationDescriptor());
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

  plotterSaveTimer_ = 120;
  outputROOTFile_ = "";
  outputImagesPath_ = ""; 
  xmlCfgFile_ = ""; 
  dduCheckMask_ = 0xFFFFFFFF;
  binCheckMask_ = 0xFFFFFFFF;


  totalEvents_ 		= 0;
  sessionEvents_ 	= 0;
  readoutMode_ 		= "internal";

  inputDataFormat_      = "DDU";
  inputDeviceType_	= "file";
  inputDeviceName_      = "";
  
  serversClassName_ 	= "EmuRUI";
  serverTIDs_.clear();
  // serverTIDs_.push_back(0);
  collectorsClassName_	= "EmuDisplayServer";
  collectorID_		= 0;
  transport_            = "i2o";
  committedPoolSize_	= 0x500000; // 1Mb
  nEventCredits_	= 1;
  prescalingFactor_	= 100;

  useAltFileReader_	= false;
  

  getApplicationInfoSpace()->fireItemAvailable("totalEvents",&totalEvents_);
  getApplicationInfoSpace()->fireItemAvailable("sessionEvents",&sessionEvents_);

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

  getApplicationInfoSpace()->fireItemAvailable("xmlCfgFile",&xmlCfgFile_);
  getApplicationInfoSpace()->fireItemAvailable("plotterSaveTimer",&plotterSaveTimer_);
  getApplicationInfoSpace()->fireItemAvailable("outputROOTFile",&outputROOTFile_);
  getApplicationInfoSpace()->fireItemAvailable("outputImagesPath",&outputImagesPath_);
  getApplicationInfoSpace()->fireItemAvailable("dduCheckMask",&dduCheckMask_);
  getApplicationInfoSpace()->fireItemAvailable("binCheckMask",&dduCheckMask_);

  getApplicationInfoSpace()->fireItemAvailable("useAltFileReader",&useAltFileReader_);

  getApplicationInfoSpace()->addItemChangedListener ("readoutMode", this);
  getApplicationInfoSpace()->addItemChangedListener ("transport", this);
  getApplicationInfoSpace()->addItemChangedListener ("collectorsClassName", this);
  getApplicationInfoSpace()->addItemChangedListener ("committedPoolSize",this);
  getApplicationInfoSpace()->addItemChangedListener ("serversClassName", this);
  getApplicationInfoSpace()->addItemChangedListener ("serverTIDs", this);
  getApplicationInfoSpace()->addItemChangedListener ("nEventCredits",    this);
  getApplicationInfoSpace()->addItemChangedListener ("prescalingFactor", this);
  
  getApplicationInfoSpace()->addItemChangedListener ("inputDataFormat", this);
  getApplicationInfoSpace()->addItemChangedListener ("inputDeviceType", this);
  getApplicationInfoSpace()->addItemChangedListener ("inputDeviceName", this);

  getApplicationInfoSpace()->addItemChangedListener ("xmlCfgFile", this);
  getApplicationInfoSpace()->addItemChangedListener ("outputROOTFile", this);
  getApplicationInfoSpace()->addItemChangedListener ("plotterSaveTimer", this);
  getApplicationInfoSpace()->addItemChangedListener ("dduCheckMask", this);
  getApplicationInfoSpace()->addItemChangedListener ("binCheckMask", this);

  getApplicationInfoSpace()->addItemChangedListener ("useAltFileReader", this);

  getApplicationInfoSpace()->addItemRetrieveListener ("totalEvents", this);
  getApplicationInfoSpace()->addItemRetrieveListener ("sessionEvents", this);

};


// == Define Application FSM == //
void EmuMonitor::defineFSM() 
{
  fsm_.addState ('H', "Halted");
  fsm_.addState ('R', "Ready");
  fsm_.addState ('E', "Enabled");
  fsm_.addStateTransition ('H','R', "Configure", this, &EmuMonitor::ConfigureAction);
  fsm_.addStateTransition ('R','E', "Enable", this, &EmuMonitor::EnableAction);
  fsm_.addStateTransition ('E','H', "Halt", this, &EmuMonitor::HaltAction);
  fsm_.addStateTransition ('R','H', "Halt", this, &EmuMonitor::HaltAction);
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
              LOG4CPLUS_INFO (getApplicationLogger(), "Committed pool size is " << (unsigned long) committedPoolSize_);
              toolbox::mem::CommittedHeapAllocator* a = new toolbox::mem::CommittedHeapAllocator(committedPoolSize_);
              toolbox::net::URN urn ("toolbox-mem-pool", "EmuMonitor_EMU_MsgPool");
              pool_ = toolbox::mem::getMemoryPoolFactory()->createPool(urn, a);
//            LOG4CPLUS_INFO (getApplicationLogger(), "Set high watermark to 90% and low watermark to 70%");
              LOG4CPLUS_INFO (getApplicationLogger(), "Set event credit message buffer's high watermark to 70%");
              pool_->setHighThreshold ( (unsigned long) (committedPoolSize_ * 0.7));
//            pool_->setLowThreshold ((unsigned long) (committedPoolSize_ * 0.7));
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
	plotter_ = new EmuLocalPlotter(getApplicationLogger());
//	plotter_ = new EmuPlotter(getApplicationLogger());
//	if (xmlCfgFile_ != "") plotter_->setXMLCfgFile(xmlCfgFile_.toString());
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
}

// == Bind CGI Callbacks ==//
void EmuMonitor::bindCGIcallbacks()
{
  xgi::bind(this, &EmuMonitor::dispatch, "dispatch");
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

// == Run Control requests current parameter values == //
void EmuMonitor::actionPerformed (xdata::Event& e)
{
  // update measurements monitors
  if (e.type() == "ItemRetrieveEvent")
    {
      	std::string item = dynamic_cast<xdata::ItemRetrieveEvent&>(e).itemName();
      	if ( item == "totalEvents")
	{
	  LOG4CPLUS_INFO(getApplicationLogger(), "Total Events : " << totalEvents_.toString());
	} 
	else if ( item == "sessionEvents")
	{
	  LOG4CPLUS_INFO(getApplicationLogger(), "Session Events: " << sessionEvents_.toString());
	} 
	else if ( item == "dataRate")
	{
	  LOG4CPLUS_INFO(getApplicationLogger(), "Data Rate: " << dataRate_.toString());
	}
	else if ( item == "readoutMode")
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
	else if ( item == "serverTIDs")
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
	  destroyDeviceReader();
	  createDeviceReader();
        }
        else if ( item == "inputDeviceName")
        {
          LOG4CPLUS_INFO(getApplicationLogger(), "Input Device Name : " << inputDeviceName_.toString());
          destroyDeviceReader();
          createDeviceReader();
        }
	else if ( item == "inputDataFormat")
        {
          LOG4CPLUS_INFO(getApplicationLogger(), "Input Data Format : " << inputDataFormat_.toString());
          destroyDeviceReader();
          createDeviceReader();
        }
	else if ( item == "useAltFileReader")
        {
          LOG4CPLUS_INFO(getApplicationLogger(), "Use Alt File Reader : " << useAltFileReader_.toString());
          destroyDeviceReader();
          createDeviceReader();
        } 
	else if ( item == "plotterSaveTimer")
        {
          LOG4CPLUS_INFO(getApplicationLogger(), "Plotter Save Timer : " << plotterSaveTimer_.toString());
	  if ((plotterSaveTimer_>xdata::Integer(0))
		&& (timer_ != NULL)) {
	      timer_->setTimer(plotterSaveTimer_);
    	  }

        }
	else if ( item == "dduCheckMask")
        {
          LOG4CPLUS_INFO(getApplicationLogger(), "DDU Checks Mask : " << dduCheckMask_.toString());
	  if (dduCheckMask_ >= xdata::UnsignedLong(0)
		&& (plotter_ != NULL)) {
	      plotter_->SetDDUCheckMask(dduCheckMask_);
    	  }

        }
	else if ( item == "binCheckMask")
        {
          LOG4CPLUS_INFO(getApplicationLogger(), "Binary Checks Mask : " << binCheckMask_.toString());
	  if (binCheckMask_ >= xdata::UnsignedLong(0)
		&& (plotter_ != NULL)) {
      		plotter_->SetBinCheckMask(binCheckMask_);
	  }

        }
	else if ( item == "outputROOTFile")
        {
          LOG4CPLUS_INFO(getApplicationLogger(), "Output ROOT File : " << outputROOTFile_.toString());
          if (outputROOTFile_.toString().length()
                && (plotter_ != NULL)) {
              plotter_->SetHistoFile(outputROOTFile_);
          }
        }
        else if ( item == "xmlCfgFile")
        {
	  /*
          LOG4CPLUS_INFO(getApplicationLogger(), 
		"XML Config File for plotter : " << xmlCfgFile_.toString());
	  if (plotter_ != NULL) {
		plotter_->setXMLCfgFile(xmlCfgFile_.toString());
	  }
	  */
        }
/*
	if (readoutMode_.toString() == "external") {
                this->activate();
          }
*/

    }
}

// == Get Application Descriptors for specified Data Server class name == //
void EmuMonitor::getDataServers(xdata::String className) 
{
	try
           {
	     dataservers_.clear();
	     if (serverTIDs_.size() > 0) {
             	std::vector<xdaq::ApplicationDescriptor*> tmpdataservers_;
             	tmpdataservers_ = getApplicationContext()->getApplicationGroup()->getApplicationDescriptors(className.toString().c_str());
	     	for (unsigned int i = 0; i<tmpdataservers_.size(); i++) {
			for (unsigned int j = 0; j<serverTIDs_.size(); j++) {
				if ((serverTIDs_.at(j) != xdata::UnsignedLong(0)) && (serverTIDs_.at(j) == xdata::UnsignedLong(i2o::utils::getAddressMap()->getTid(tmpdataservers_[i]))))
					dataservers_.push_back(tmpdataservers_[i]);
			}
		}
	     } else {
		dataservers_ = getApplicationContext()->getApplicationGroup()->getApplicationDescriptors(className.toString().c_str());
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
             collectors_ = getApplicationContext()->getApplicationGroup()->getApplicationDescriptors(className.toString().c_str());
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
	// !! EmuMonitor or EmuRUI tid?
	appTid_ = i2o::utils::getAddressMap()->getTid(this->getApplicationDescriptor());
	/*
	if (xmlCfgFile_ != "" && plotter_ != NULL) plotter_->setXMLCfgFile(xmlCfgFile_.toString());
	if (plotter_ != NULL) plotter_->book(appTid_);
	*/
	setupPlotter();
        if (plotter_ != NULL) {
      		timer_->setPlotter(plotter_);
		if (outputROOTFile_.toString().length()) {
			LOG4CPLUS_INFO (getApplicationLogger(),
                        "plotter::outputROOTFile: 0x" << outputROOTFile_.toString());
	     	 	plotter_->SetHistoFile(outputROOTFile_);
    		}
		if (dduCheckMask_ >= xdata::UnsignedLong(0)) {
			LOG4CPLUS_INFO (getApplicationLogger(),
                        "plotter::dduCheckMask: 0x" << std::hex << dduCheckMask_.value_);
		      	plotter_->SetDDUCheckMask(dduCheckMask_);
    		}
    		if (binCheckMask_ >= xdata::UnsignedLong(0)) {
			LOG4CPLUS_INFO (getApplicationLogger(), 
			"plotter::binCheckMask: 0x" << std::hex << binCheckMask_.value_);
      			plotter_->SetBinCheckMask(binCheckMask_);
    		}
	
	plotter_->book();
 	}
	if (plotterSaveTimer_>xdata::Integer(0)) {
              timer_->setTimer(plotterSaveTimer_);
        }

	configureReadout();
}

void EmuMonitor::HaltAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception )
{
	if (plotter_ != NULL) timer_->kill();
	disableReadout();
}

void EmuMonitor::EnableAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception )
{
	sessionEvents_=0;
	creditMsgsSent_ = 0;
	eventsRequested_ = 0;
	eventsReceived_ = 0;
//	configureReadout();
	if (plotter_ != NULL) timer_->activate();
	enableReadout();
	bindI2Ocallbacks();
}

// == XGI Call back == //
void EmuMonitor::Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
  wsm_.displayPage(out);
}

// == WSM Dispatcher function == //
void EmuMonitor::dispatch (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception)
{
  cgicc::Cgicc cgi(in);
  //const cgicc::CgiEnvironment& env = cgi.getEnvironment();
  cgicc::const_form_iterator stateInputElement = cgi.getElement("StateInput");
  std::string stateInput = (*stateInputElement).getValue();
  wsm_.fireEvent(stateInput,in,out);
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

// == Print application parameters == //
void EmuMonitor::printParametersTable( xgi::Output * out ) throw (xgi::exception::Exception)
{

  // xdata::InfoSpace * params_list = getApplicationInfoSpace(); 
  // xdata::InfoSpace::iterator itr;
  std::map<std::string, xdata::Serializable*, std::less<std::string> > *params_list = getApplicationInfoSpace(); 
  std::map<std::string, xdata::Serializable*, std::less<std::string> >::iterator itr;
  *out 	<< "<table border>"
	<< "<tr>"
        << "<th colspan=3 bgcolor=#7F7FFF>" << "Parameters List" << "</th>"
	<< "</tr>"
	<< "<tr>"
  	<< "<th>" << "Name" << "</th>"
	<< "<th>" << "Type" << "</th>" 
	<< "<th>" << "Value" << "</th>"
 	<< "</tr>" << std::endl;
  for (itr=params_list->begin(); itr != params_list->end(); ++itr) 
  {
    if (itr->second->type() == "properties") continue;
    *out << "<tr>" << std::endl;
    *out << "<td>" << itr->first << "</td>" << std::endl;
    *out << "<td>" << itr->second->type() << "</td>" << std::endl;
    if (itr->second->type() == "vector") {
        *out << "<td>";
	// =VB= !!! possible XDAQ bug: returns wrong pointer to xdata::Vector (+4 bytes offset) 
	for (int i=0; i < reinterpret_cast<xdata::Vector<xdata::Serializable>* >((int)(itr->second)-4)->elements(); i++) {
	   *out << reinterpret_cast<xdata::Vector<xdata::Serializable>*>((int)(itr->second)-4)->elementAt(i)->toString() << " ";
	 }
	*out << "</td>" << std::endl;
    } else {
       *out << "<td>" << itr->second->toString() << "</td>" << std::endl;
    }

    *out << "</tr>" << std::endl;
  }
  *out 	<< "</table>" << std::endl;
}
/*
string EmuMonitor::ageOfPageClock(){
  stringstream ss;
  ss << "<script type=\"text/javascript\">"                        << endl;
  ss << "   ageOfPage=0"                                           << endl;
  ss << "   function countSeconds(){"                              << endl;
  ss << "      hours=Math.floor(ageOfPage/3600)"                   << endl;
  ss << "      minutes=Math.floor(ageOfPage/60)%60"                << endl;
  ss << "      age=\"\""                                           << endl;
  ss << "      if (hours) age+=hours+\" h \""                      << endl;
  ss << "      if (minutes) age+=minutes+\" m \""                  << endl;
  ss << "      age+=ageOfPage%60+\" s \""                          << endl;
  ss << "      document.getElementById('ageOfPage').innerHTML=age" << endl;
  ss << "      ageOfPage=ageOfPage+1"                              << endl;
  ss << "      setTimeout('countSeconds()',1000)"                  << endl;
  ss << "   }"                                                     << endl;
  ss << "</script>"                                                << endl;
  return ss.str();
}

*/
// == Web Navigation Pages == //
void EmuMonitor::stateMachinePage( xgi::Output * out ) throw (xgi::exception::Exception)
{
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
/*
  if ( fsm_.getCurrentState() == 'E' ){
     *out << "<meta http-equiv=\"Refresh\" content=\"5\">"              << endl;
    }
*/
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;

  xgi::Utils::getPageHeader
    (out,
     "EmuMonitor",
     getApplicationDescriptor()->getContextDescriptor()->getURL(),
     getApplicationDescriptor()->getURN(),
     "/daq/xgi/images/Application.gif"
     );

  std::string url = "/";
  url += getApplicationDescriptor()->getURN();
  url += "/dispatch";

  // display FSM
  std::set<std::string> possibleInputs = wsm_.getInputs(wsm_.getCurrentState());
  std::set<std::string> allInputs = wsm_.getInputs();


  *out << cgicc::h3("Finite State Machine").set("style", "font-family: arial") << std::endl;

//  printParametersTable(out);

//  *out << cgicc::hr() << std::endl;

  *out << "<table border cellpadding=10 cellspacing=0>" << std::endl;
  *out << "<tr>" << std::endl;
  *out << "<th>" << wsm_.getStateName(wsm_.getCurrentState()) << "</th>" << std::endl;
  *out << "</tr>" << std::endl;
  *out << "<tr>" << std::endl;
  std::set<std::string>::iterator i;
  for ( i = allInputs.begin(); i != allInputs.end(); i++)
    {
      *out << "<td>";
      *out << cgicc::form().set("method","get").set("action", url).set("enctype","multipart/form-data") << std::endl;

      if ( possibleInputs.find(*i) != possibleInputs.end() )
	{
	  *out << cgicc::input().set("type", "submit").set("name", "StateInput").set("value", (*i) );
	}
      else
	{
	  *out << cgicc::input() .set("type", "submit").set("name", "StateInput").set("value", (*i) ).set("disabled", "true");
	}

      *out << cgicc::form();
      *out << "</td>" << std::endl;
    }

  *out << "</tr>" << std::endl;
  *out << "</table>" << std::endl;
  //

  *out << cgicc::hr() << std::endl;

  printParametersTable(out);
  

  xgi::Utils::getPageFooter(*out);
}




// == Failure Pages == //
void EmuMonitor::failurePage(xgi::Output * out, xgi::exception::Exception & e)  throw (xgi::exception::Exception)
{
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;

  xgi::Utils::getPageHeader
    (out,
     "EmuMonitor Failure",
     getApplicationDescriptor()->getContextDescriptor()->getURL(),
     getApplicationDescriptor()->getURN(),
     "/daq/xgi/images/Application.gif"
     );

  *out << cgicc::br() << e.what() << cgicc::br() << endl;
  std::string url = "/";
  url += getApplicationDescriptor()->getURN();

  *out << cgicc::br() << "<a href=\"" << url << "\">" << "retry" << "</a>" << cgicc::br() << endl;

  xgi::Utils::getPageFooter(*out);
}

void EmuMonitor::emuDataMsg(toolbox::mem::Reference *bufRef){
  // Emu-specific stuff

  dataMessages_.push_back( bufRef );
  eventsReceived_++;

//   // Process the oldest message, i.e., the one at the front of the queue
   toolbox::mem::Reference *oldestMessage = dataMessages_.front();

  I2O_EMU_DATA_MESSAGE_FRAME *msg =
    (I2O_EMU_DATA_MESSAGE_FRAME*)oldestMessage->getDataLocation();

  char *startOfPayload = (char*) oldestMessage->getDataLocation()
    + sizeof(I2O_EMU_DATA_MESSAGE_FRAME);

  unsigned long sizeOfPayload = oldestMessage->getDataSize()-sizeof(I2O_EMU_DATA_MESSAGE_FRAME);

  unsigned long errorFlag = msg->errorFlag;

  LOG4CPLUS_INFO(getApplicationLogger(),
                 // "Received " << bufRef->getDataSize() <<
                 "Received evt#" << eventsReceived_ << " (req: " << eventsRequested_ << ")" <<
		  sizeOfPayload << " bytes, run " << msg->runNumber <<
                 ", errorFlag 0x"  << std::hex << msg->errorFlag << std::dec <<
                 " from " << serversClassName_.toString() <<
		 ":" << msg->PvtMessageFrame.StdMessageFrame.InitiatorAddress <<
                 ", still holding " << msg->nEventCreditsHeld << " event credits, " <<
		 "pool size " << dataMessages_.size());

  processEvent(reinterpret_cast<const char *>(startOfPayload), sizeOfPayload, errorFlag);
//  usleep(2500);

  // Free the Emu data message
  bufRef->release();
  dataMessages_.erase( dataMessages_.begin() );
//  eventsReceived_++;
}

// == Send an I2O token message to all servers == //
int EmuMonitor::sendDataRequest(unsigned long last)
{
//  creditMsgsSent += 1;
  for (unsigned int i = 0; i < dataservers_.size(); i++)
    {
      creditMsgsSent_ ++;
      eventsRequested_ = eventsRequested_ +  nEventCredits_;
      toolbox::mem::Reference * ref = 0;
      try
        {
          ref = toolbox::mem::getMemoryPoolFactory()->getFrame(pool_, maxFrameSize_);

          PI2O_EMUMONITOR_CREDIT_MESSAGE_FRAME frame = (PI2O_EMUMONITOR_CREDIT_MESSAGE_FRAME) ref->getDataLocation();


          frame->PvtMessageFrame.StdMessageFrame.MsgFlags         = 0;
          frame->PvtMessageFrame.StdMessageFrame.VersionOffset    = 0;
          frame->PvtMessageFrame.StdMessageFrame.TargetAddress    = i2o::utils::getAddressMap()->getTid(dataservers_[i]);
          frame->PvtMessageFrame.StdMessageFrame.InitiatorAddress = i2o::utils::getAddressMap()->getTid(this->getApplicationDescriptor());
          frame->PvtMessageFrame.StdMessageFrame.MessageSize      = (sizeof(I2O_EMUMONITOR_CREDIT_MESSAGE_FRAME)) >> 2;

          frame->PvtMessageFrame.StdMessageFrame.Function = I2O_PRIVATE_MESSAGE;
          frame->PvtMessageFrame.XFunctionCode            = I2O_EMUMONITOR_CODE;
          frame->PvtMessageFrame.OrganizationID           = XDAQ_ORGANIZATION_ID;

          frame->nEventCredits    = nEventCredits_;
          frame->prescalingFactor = prescalingFactor_;

          ref->setDataSize(frame->PvtMessageFrame.StdMessageFrame.MessageSize << 2);
          LOG4CPLUS_INFO (getApplicationLogger(),
                           "Sending credit #" << creditMsgsSent_ << " to tid: " << frame->PvtMessageFrame.StdMessageFrame.TargetAddress
                           //                                   << ". maxFrameSize=" << maxFrameSize_
                           );
          getApplicationContext()->postFrame(ref, this->getApplicationDescriptor(), dataservers_[i]);
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
                  getApplicationContext()->postFrame(ref,  this->getApplicationDescriptor(), dataservers_[i]);
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
  this->activate();
}

// == Disable Readout == //
void EmuMonitor::disableReadout()
{
  this->kill();
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
      altFileReader_ = NULL;
      try {
	if      ( inputDeviceType_ == "spy"  )
	  deviceReader_ = new EmuSpyReader(  inputDeviceName_.toString(), inputDataFormatInt_ );
	else if ( inputDeviceType_ == "file" )
	  if (useAltFileReader_ == xdata::Boolean(true)) {
		altFileReader_ = new FileReaderDDU();
		altFileReader_->openFile((inputDeviceName_.toString()).c_str());
		LOG4CPLUS_INFO(getApplicationLogger(),
			"Will use Alternative File Reader");
	  } else {
	  	deviceReader_ = new EmuFileReader( inputDeviceName_.toString(), inputDataFormatInt_ );
	  }
	// TODO: slink
	else     LOG4CPLUS_ERROR(getApplicationLogger(),
				 "Bad device type: " << inputDeviceType_.toString() <<
				 "Use \"file\", \"spy\", or \"slink\"");
      }
      catch(char* e){

	stringstream oss;
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
  if (altFileReader_ != NULL) {
	LOG4CPLUS_DEBUG(getApplicationLogger(),
                "Destroying alternative File Reader" );
        delete altFileReader_;
        altFileReader_ = NULL;
  }
}

int EmuMonitor::svc()
{
  LOG4CPLUS_INFO (getApplicationLogger(), "Starting Readout loop...");
//  return 0;

  bool keepRunning = true;

  while(keepRunning)
    {
      if ( (readoutMode_.toString() == "internal") && 
	 ( (deviceReader_ != NULL) || ( (useAltFileReader_ == xdata::Boolean(true) && altFileReader_ != NULL) ) )) 
	{
	  if (useAltFileReader_ == xdata::Boolean(true) ) {
		keepRunning = altFileReader_->readNextEvent();
	  } else {
	  	keepRunning = deviceReader_->readNextEvent();
	  }
	  if ( !keepRunning )
	    {
	      LOG4CPLUS_ERROR(getApplicationLogger(),
			      " " << inputDataFormat_.toString() << " " << inputDeviceType_.toString() <<
			      " " << inputDeviceName_.toString() << " read error.");
		// plotter_->saveToROOTFile(outputROOTFile_);
		if (plotter_ != NULL) {
			updateList(collectorID_);
                	updateObjects(collectorID_);
                	plotter_->setListModified(false);
			plotter_->SaveImages(outputImagesPath_, "png", 1200, 900);
			// plotter_->SaveImagesFromROOTFile(outputROOTFile_, outputImagesPath_, "png", 1200, 900);
			plotter_->SaveHistos();
		}
	    } else {
	      unsigned long errorFlag = 0;
	      if (useAltFileReader_ == xdata::Boolean(true) ) {
                errorFlag = altFileReader_->status();
		processEvent(altFileReader_->data(), altFileReader_->dataLength(), errorFlag, appTid_);
	      } else {
	      		processEvent(deviceReader_->data(), deviceReader_->dataLength(), errorFlag, appTid_);
	      }
	    }

        }
      
      unsigned long timeout = 3*1000000; // 5sec
      unsigned long wait = 10000; // 10ms
      unsigned long waittime = 0;
      if (readoutMode_.toString() == "external") { 
	// ::sleep(1);
	// usleep(500000);
	while ((eventsReceived_ < eventsRequested_) && ( waittime <= timeout )) {
		usleep(wait);
		waittime += wait;
	} 
	if (waittime >= timeout) {
		LOG4CPLUS_WARN (getApplicationLogger(), toolbox::toString("Timeout waiting for events from server."));
		LOG4CPLUS_WARN (getApplicationLogger(), "Missed " << (eventsRequested_ - eventsReceived_) << " events");
		eventsRequested_ = eventsReceived_;
	}
	waittime = 0;
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
void EmuMonitor::processEvent(const char * data, int dataSize, unsigned long errorFlag, int node)
{
   totalEvents_++;
   sessionEvents_++;
   LOG4CPLUS_INFO(getApplicationLogger(), "Event #" << sessionEvents_.toString() 
	<< " (Total processed: " << totalEvents_.toString() << ")"); 
   // plotter_->processEvent(data, dataSize, errorFlag, node);
   plotter_->fill((unsigned char *)data, dataSize, errorFlag);
   if (plotter_->isListModified()) {
    updateList(collectorID_);
    updateObjects(collectorID_);
    plotter_->setListModified(false);
  }
}

void EmuMonitor::updateList(xdata::Integer id)
{

  xoap::MessageReference msg = xoap::createMessage();
  xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope();
  xoap::SOAPBody body = envelope.getBody();
  xoap::SOAPName commandName = envelope.createName("updateList","xdaq", "urn:xdaq-soap:3.0");
/*
  xoap::SOAPName originator = envelope.createName("originator");
  xoap::SOAPName targetAddr = envelope.createName("targetAddr");
*/
  xoap::SOAPElement command = body.addBodyElement(commandName );


  xdata::Integer localTid(i2o::utils::getAddressMap()->getTid(this->getApplicationDescriptor()));
  /*
    xdata::Integer serverTid (displayServerTID_);

    command.addAttribute (targetAddr, serverTid.toString() );
    command.addAttribute (originator, localTid.toString() );
  */
  xoap::SOAPName monitorName = envelope.createName("DQMNode", "", "");
  xoap::SOAPElement monitorElement = command.addChildElement(monitorName);
  monitorElement.addTextNode(localTid.toString());

  xoap::SOAPName histoName = envelope.createName("Obj", "", "");
  xoap::SOAPName histodirName = envelope.createName("Branch", "", "");
/*
  for (map<int, map<string, TH1*> >::iterator itr = plotter_->histos.begin();
       itr != plotter_->histos.end(); ++itr) {
    xdata::Integer dir (itr->first);
    xoap::SOAPElement histodirElement = monitorElement.addChildElement(histodirName);
    histodirElement.addTextNode(dir.toString());
    for (map<string, TH1*>::iterator h_itr = itr->second.begin();
         h_itr != itr->second.end(); ++h_itr) {
      xdata::String hname  (h_itr->first);
      xoap::SOAPElement histoElement = histodirElement.addChildElement(histoName);
      histoElement.addTextNode(hname);
//              LOG4CPLUS_DEBUG(getApplicationLogger(), 
			"MyHistograms: " << h_itr->first << " size: " << sizeof(*h_itr->second));
    }
  }
*/
  for (map<int, map<string, ConsumerCanvas*> >::iterator itr = plotter_->canvases.begin();
       itr != plotter_->canvases.end(); ++itr) {
    xdata::Integer dir (itr->first);
    xoap::SOAPElement histodirElement = monitorElement.addChildElement(histodirName);
    histodirElement.addTextNode(dir.toString());
    for (map<string, ConsumerCanvas*>::iterator h_itr = itr->second.begin();
         h_itr != itr->second.end(); ++h_itr) {
      xdata::String hname  (h_itr->first);
      xoap::SOAPElement histoElement = histodirElement.addChildElement(histoName);
      histoElement.addTextNode(hname);
//              LOG4CPLUS_DEBUG(getApplicationLogger(), 
//		"MyCanvases: " << h_itr->first << " size: " << sizeof(*h_itr->second));
    }
  }
try
    {
    xdaq::ApplicationDescriptor* collectorDescriptor =
    getApplicationContext()
    ->getApplicationGroup()
    ->getApplicationDescriptor( collectorsClassName_, id );
      xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, collectorDescriptor);

      /*
        cout << endl;
        reply.writeTo(cout);
        cout << endl;
      */
      xoap::SOAPBody rb = reply->getSOAPPart().getEnvelope().getBody();

      if (rb.hasFault() )
        {
          xoap::SOAPFault fault = rb.getFault();
          string errmsg = "Server: ";
          errmsg += fault.getFaultString();
          XCEPT_RAISE(xoap::exception::Exception, errmsg);
        } else
          {
            vector<xoap::SOAPElement> content = rb.getChildElements ();
            if (content.size() == 1)
              {
                xoap::SOAPName statusTag ("Status", "", "");
                vector<xoap::SOAPElement> statusElement = content[0].getChildElements (statusTag );
                if (statusElement[0].getElementName() == statusTag)
                  {
                    LOG4CPLUS_INFO(getApplicationLogger(), 
			"The server status is: " << statusElement[0].getValue());
                  } else {
                    LOG4CPLUS_DEBUG(getApplicationLogger(), 
			"Value of element is: " << statusElement[0].getValue());
                  }
              } else
                {
                  LOG4CPLUS_DEBUG(getApplicationLogger(), 
			"Response contains wrong number of elements: " << content.size());
                }
          }
    }
  catch (xdaq::exception::Exception& e)
    {
      // handle exception
    }


  //   return reply;
}

void EmuMonitor::updateObjects(xdata::Integer id)
{

  xoap::MessageReference msg = xoap::createMessage();
  xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope();
  xoap::SOAPBody body = envelope.getBody();
  xoap::SOAPName commandName = envelope.createName("updateObjects", "xdaq", "urn:xdaq-soap:3.0");

  xoap::SOAPName originator = envelope.createName("originator");
 // xoap::SOAPName targetAddr = envelope.createName("targetAddr");
  xoap::SOAPElement command = body.addBodyElement(commandName );

  xdata::Integer localTid(i2o::utils::getAddressMap()->getTid(this->getApplicationDescriptor()));
  /*
    xdata::Integer serverTid (displayServerTID_);

    command.addAttribute (targetAddr, serverTid.toString() );
    command.addAttribute (originator, localTid.toString() );
  */
  command.addAttribute (originator, localTid.toString() );
  xoap::SOAPName monitorName = envelope.createName("DQMNode", "", "");
  xoap::SOAPElement monitorElement = command.addChildElement(monitorName);
  monitorElement.addTextNode(localTid.toString());
  // b.addTextNode ("Monitor Objects Attached");
  //int cnt=0;

/*
  for (map<int, map<string, TH1*> >::iterator itr = plotter_->histos.begin();
       itr != plotter_->histos.end(); ++itr) {
    string dir="";
    if (int id=itr->first) {
      stringstream stdir;
      dir.clear();
      stdir.clear();
      stdir << "CSC_" << ((id>>4)&0xFF) << "_" << (id&0xF) << "/";
      stdir >> dir;
    }
    if (itr->first == SLIDES_ID) {
      dir = string(SLIDES) +"/";
    }
 string location = dir;
    for (map<string, TH1*>::iterator h_itr = itr->second.begin();
         h_itr != itr->second.end(); ++h_itr) {
      //cout << "Count: " << cnt << endl;
      //if (cnt < 1) {
      xdata::String hname  (h_itr->first);
      TMessage buf(kMESS_OBJECT);
      buf.Reset();
      buf.SetWriteMode();
      buf.WriteObjectAny(h_itr->second, h_itr->second->Class());
      //        LOG4CPLUS_DEBUG(getApplicationLogger(), "Histogram: " << h_itr->first << " buffer size:" << buf.BufferSize());
      char * attch_buf = new char[buf.BufferSize()];
      buf.Reset();
      buf.SetReadMode();
      buf.ReadBuf(attch_buf, buf.BufferSize());
      // string contenttype = "content/unknown";
      string contenttype = "application/octet-stream";
      xoap::AttachmentPart * attachment = msg->createAttachmentPart(attch_buf, buf.BufferSize(), contenttype);
      //attachment->addMimeHeader("Content-Description", h_itr->first);
      attachment->setContentLocation(location+h_itr->first);
      attachment->setContentEncoding("binary");
      msg->addAttachmentPart(attachment);
      delete []attch_buf;
      //cnt++;
      //}
    }
  }
*/

for (map<int, map<string, ConsumerCanvas*> >::iterator itr = plotter_->canvases.begin();
       itr != plotter_->canvases.end(); ++itr) {
    string dir="";
    if (int id=itr->first) {
      stringstream stdir;
      dir.clear();
      stdir.clear();
      stdir << "CSC_" << ((id>>4)&0xFF) << "_" << (id&0xF) << "/";
      stdir >> dir;
    }
    if (itr->first == SLIDES_ID) {
      dir = string(SLIDES) +"/";
    }
    string location = dir;
    for (map<string, ConsumerCanvas*>::iterator h_itr = itr->second.begin();
         h_itr != itr->second.end(); ++h_itr) {
      //cout << "Count: " << cnt << endl;
      //if (cnt < 1) {
      xdata::String hname  (h_itr->first);
      TMessage buf(kMESS_OBJECT);
      buf.Reset();
      buf.SetWriteMode();
      buf.WriteObjectAny(h_itr->second, h_itr->second->Class());
    //           LOG4CPLUS_DEBUG(getApplicationLogger(), "Canvas: " << h_itr->first << " buffer size:" << buf.BufferSize());
      char * attch_buf = new char[buf.BufferSize()];
      buf.Reset();
      buf.SetReadMode();
      buf.ReadBuf(attch_buf, buf.BufferSize());
      // string contenttype = "content/unknown";
      string contenttype = "application/octet-stream";
      xoap::AttachmentPart * attachment = msg->createAttachmentPart(attch_buf, buf.BufferSize(), contenttype);
      //attachment->addMimeHeader("Content-Description", h_itr->first);
      attachment->setContentLocation(location+h_itr->first);
      attachment->setContentEncoding("binary");
      msg->addAttachmentPart(attachment);
      delete []attch_buf;
      //cnt++;
      //}
    }
  }

try
    {
     xdaq::ApplicationDescriptor* collectorDescriptor =
	getApplicationContext()
    	->getApplicationGroup()
    	->getApplicationDescriptor( collectorsClassName_, id );

      xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, collectorDescriptor);
      /*
        cout << endl;
        reply.writeTo(cout);
        cout << endl;
      */
      xoap::SOAPBody rb = reply->getSOAPPart().getEnvelope().getBody();

      if (rb.hasFault() )
        {
          xoap::SOAPFault fault = rb.getFault();
          string errmsg = "Server: ";
          errmsg += fault.getFaultString();
          XCEPT_RAISE(xoap::exception::Exception, errmsg);

        } else
          {
            vector<xoap::SOAPElement> content = rb.getChildElements ();
            if (content.size() == 1)
              {
                xoap::SOAPName statusTag ("UpdateStatus", "", "");
                vector<xoap::SOAPElement> statusElement = content[0].getChildElements (statusTag );
                if (statusElement[0].getElementName() == statusTag)
                  {
                    LOG4CPLUS_INFO(getApplicationLogger(), 
			"The server status is: " << statusElement[0].getValue());
                  } else {
                    LOG4CPLUS_DEBUG(getApplicationLogger(), 
			"Value of element is: " << statusElement[0].getValue());
                  }
              } else
                {
                  LOG4CPLUS_DEBUG(getApplicationLogger(), 
			"Response contains wrong number of elements: " << content.size());
                }
          }
      //   return reply;
    }
  catch (xdaq::exception::Exception& e)
    {
      // handle exception
    }

}



