#include <sstream>

#include "EmuMonitor.h"
#include "EmuLocalPlotter.h"
#include "EmuGlobalPlotter.h"
#include "xdaq.h"
#include "xgi/Method.h"
#include "i2o/Method.h"
#include "i2o/utils/AddressMap.h"
#include "pt/PeerTransportAgent.h"
#include "xoap/Method.h"
#include "xdaq/NamespaceURI.h"
#include "xoap/MessageReference.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPPart.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/domutils.h"
#include "xcept/tools.h"



#include "TH1.h"
#include "TBuffer.h"
#include "TMessage.h"

//#include "EventBuilderI2oMsgs.h"
//#include "i2oEVBMsgs.h"
#include "I2O_DQMDataRequest.h"

XDAQ_INSTANTIATOR_IMPL(EmuMonitor)

  EmuMonitor::EmuMonitor(xdaq::ApplicationStub* stub) 
  throw (xdaq::exception::Exception)
    : EmuBaseMonitor(stub)
{   

  plotter_ = NULL;
  plotterSaveTimer_ = 30;
  plotterHistoFile_ = "";
  displayServer_ = "";
  displayServerInstance_ = 0;
  displayServerTID_ = NULL;
  dduCheckMask_ = 0xFFFFFFFF;
  binCheckMask_ = 0xFFFFFFFF;
  timer_ = new EmuMonitorTimerTask();
  ddu2004_ = 1;

  getApplicationInfoSpace()->fireItemAvailable("Plotter Name", &plotterName_);
  getApplicationInfoSpace()->fireItemAvailable("Save Histos Timer", &plotterSaveTimer_);
  getApplicationInfoSpace()->fireItemAvailable("DQM Histo File", &plotterHistoFile_);
  getApplicationInfoSpace()->fireItemAvailable("Display Server", &displayServer_);
  getApplicationInfoSpace()->fireItemAvailable("Display Server Instance", &displayServerInstance_);
  getApplicationInfoSpace()->fireItemAvailable("DDU Reader Error Mask", &dduCheckMask_);
  getApplicationInfoSpace()->fireItemAvailable("Binary Checker Error Mask", &binCheckMask_);
  getApplicationInfoSpace()->fireItemAvailable("DDU2004", &ddu2004_);

  fsm_.addState ('H', "Halted");
  fsm_.addState ('R', "Ready");
  fsm_.addState ('E', "Enabled");
  fsm_.addStateTransition ('H','R', "Configure", this, &EmuMonitor::ConfigureAction);
  fsm_.addStateTransition ('R','E', "Enable", this, &EmuMonitor::EnableAction);
  fsm_.addStateTransition ('E','H', "Halt", this, &EmuMonitor::HaltAction);
  fsm_.addStateTransition ('R','H', "Halt", this, &EmuMonitor::HaltAction);
  fsm_.setInitialState('H');
  fsm_.reset();

  // Bind SOAP callbacks for control messages
  xoap::bind (this, &EmuMonitor::fireEvent, "Configure", XDAQ_NS_URI);
  xoap::bind (this, &EmuMonitor::fireEvent, "Enable", XDAQ_NS_URI);
  xoap::bind (this, &EmuMonitor::fireEvent, "Halt", XDAQ_NS_URI);

  // Bind CGI callbacks
  xgi::bind(this, &EmuMonitor::dispatch, "dispatch");

  // Define Web state machine
  wsm_.addState('H', "Halted",    this, &EmuMonitor::stateMachinePage);
  wsm_.addState('R', "Ready",     this, &EmuMonitor::stateMachinePage);
  wsm_.addState('E', "Enabled",   this, &EmuMonitor::stateMachinePage);
  wsm_.addStateTransition('H','R', "Configure", this, &EmuMonitor::Configure, &EmuMonitor::failurePage);
  wsm_.addStateTransition('R','E', "Enable",    this, &EmuMonitor::Enable,    &EmuMonitor::failurePage);
  wsm_.addStateTransition('R','H', "Halt",      this, &EmuMonitor::Halt,      &EmuMonitor::failurePage);
  wsm_.addStateTransition('E','H', "Halt",      this, &EmuMonitor::Halt,      &EmuMonitor::failurePage);
  wsm_.setInitialState('H');

  xoap::bind (this, &EmuMonitor::sendList, "sendList", XDAQ_NS_URI);
  xoap::bind (this, &EmuMonitor::sendObjects, "sendObjects", XDAQ_NS_URI);

}


EmuMonitor::~EmuMonitor() {
  timer_->kill();
  delete plotter_;
  cout << "plotter deleted" << endl;
}

void EmuMonitor::ConfigureAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception)
{
  if(plotterName_ == "EmuLocalPlotter") {
    plotter_ = new EmuLocalPlotter();
    if (plotter_ != NULL) {
      timer_->setPlotter(plotter_);
    }
    if (plotterSaveTimer_>xdata::Integer(0)) {
      //	plotter_->SetSaveTimer(plotterSaveTimer_);
      timer_->setTimer(plotterSaveTimer_);
    }
    if (plotterHistoFile_.toString().length()) {
      plotter_->SetHistoFile(plotterHistoFile_);
    }
    if (dduCheckMask_ >= xdata::UnsignedLong(0)) {
      plotter_->SetDDUCheckMask(dduCheckMask_);
    }
    if (binCheckMask_ >= xdata::UnsignedLong(0)) {
      plotter_->SetBinCheckMask(binCheckMask_);
    }
    if (ddu2004_ >= xdata::Integer(0)) plotter_->SetDDU2004(ddu2004_); 
  } 
  else if(plotterName_ == "EmuGlobalPlotter") {
    plotter_ = new EmuGlobalPlotter();
  } else {
    string errmsg = "Cannot recognize plotter type " + plotterName_.toString();
    throw(errmsg);
  }
  if (displayServer_ != "") {
	 xdaq::ApplicationDescriptor * displayServerTID_ = getApplicationContext()->
                                      getApplicationGroup()->
                                      getApplicationDescriptor(displayServer_, displayServerInstance_);
//    displayServerTID_ = xdaq::getTid(displayServer_, displayServerInstance_); 
  }
  plotter_->book();
  EmuBaseMonitor::Configure();
}

void EmuMonitor::EnableAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception)
{
  if (plotter_) {
    // plotter_->StartTimer();
    timer_->activate();
    cout << "EmuMonitor started" <<endl;
  }
  EmuBaseMonitor::Enable();
}

void EmuMonitor::HaltAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception)
{
  if (plotter_) {
    // plotter_->StopTimer();
    timer_->kill();
    cout << "EmuMonitor stopped" <<endl;	
  }
  EmuBaseMonitor::Halt();		
}

void EmuMonitor::processEvent(const char * data, int dataSize, unsigned long errorFlag) {
  // send to the plotter to deal with.  Why unsigned?
  plotter_->fill((unsigned char *)data, dataSize, errorFlag);
  if (plotter_->isListModified()) {
    updateList();
    updateObjects();
    plotter_->setListModified(false);
  }
  cout << "done with EmuMonitor" << endl;
}

void EmuMonitor::updateList()
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
      //        cout << "MyHisto: " << h_itr->first << " size: " << sizeof(*h_itr->second) << endl;
    }
  }   
  
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
      //        cout << "MyHisto: " << h_itr->first << " size: " << sizeof(*h_itr->second) << endl;
    }
  }
  

  try
    {
//      xdaq::ApplicationDescriptor* d = i2o::utils::getAddressMap()->
//	getApplicationDescriptor(displayServerTID_);
      xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, displayServerTID_);

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
		    cout << "The server status is: " << statusElement[0].getValue() << endl;
		  } else {
		    cout << "Value of element is: " << statusElement[0].getValue() << endl;
		  }
	      } else
		{
		  cout << "Response contains wrong number of elements: " << content.size() << endl;
		}
	  }
    }
  catch (xdaq::exception::Exception& e)
    {
      // handle exception
    }


  //   return reply;
}


void EmuMonitor::asktoupdateList()
{
  xoap::MessageReference msg = xoap::createMessage();
  xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope();
  xoap::SOAPBody body = envelope.getBody();
  xoap::SOAPName commandName = envelope.createName("updateList");
  /*
  xoap::SOAPName originator = envelope.createName("originator");
  xoap::SOAPName targetAddr = envelope.createName("targetAddr");
  */
  xoap::SOAPElement command = body.addBodyElement(commandName );

  /*
    xdaqInteger localTid  (tid());
    xdaqInteger serverTid (displayServerTID_);

    command.addAttribute (targetAddr, serverTid.toString() );
    command.addAttribute (originator, localTid.toString() );
  */

  try
    {
//      xdaq::ApplicationDescriptor* d = i2o::utils::getAddressMap()->
//	getApplicationDescriptor(displayServerTID_);
      xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, displayServerTID_);
 
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
		    cout << "The server status is: " << statusElement[0].getValue() << endl;
		  } else {
		    cout << "Value of element is: " << statusElement[0].getValue() << endl;
		  }
	      } else
		{
		  cout << "Response contains wrong number of elements: " << content.size() << endl;
		}
	  }
    }
  catch (xdaq::exception::Exception& e)
    {
      // handle exception
    }

  //   return reply;
}

xoap::MessageReference EmuMonitor::sendList (xoap::MessageReference msg) throw (xoap::exception::Exception)
{
  /*
  xdaqInteger localTid  (tid());
  SOAPMessage msg;
  SOAPEnvelope envelope = msg.getSOAPPart().getEnvelope();
  SOAPBody b = envelope.getBody();
  SOAPName replyName = envelope.createName("sendListReply", "", "");
  SOAPBodyElement e = b.addBodyElement ( replyName );
  SOAPName monitorName = envelope.createName("DQMNode", "", "");
  SOAPElement monitorElement = e.addChildElement(monitorName);
  monitorElement.addTextNode(localTid.toString());
  SOAPName histoName = envelope.createName("Obj", "", "");
  SOAPName histodirName = envelope.createName("Branch", "", "");
  for (map<int, map<string, TH1*> >::iterator itr = plotter_->histos.begin();
       itr != plotter_->histos.end(); ++itr) {
    xdaqInteger dir (itr->first);
    SOAPElement histodirElement = monitorElement.addChildElement(histodirName);
    histodirElement.addTextNode(dir.toString());
    for (map<string, TH1*>::iterator h_itr = itr->second.begin();
         h_itr != itr->second.end(); ++h_itr) {
      xdaqString hname  (h_itr->first);
      SOAPElement histoElement = histodirElement.addChildElement(histoName);
      histoElement.addTextNode(hname);
      //        cout << "MyHisto: " << h_itr->first << " size: " << sizeof(*h_itr->second) << endl;
    }
  }
  */
  return msg;
}


void EmuMonitor::updateObjects()
{

  xoap::MessageReference msg = xoap::createMessage();
  xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope();
  xoap::SOAPBody body = envelope.getBody();
  xoap::SOAPName commandName = envelope.createName("updateObjects");

//  xoap::SOAPName originator = envelope.createName("originator");
//  xoap::SOAPName targetAddr = envelope.createName("targetAddr");
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
  // b.addTextNode ("Monitor Objects Attached");
  //int cnt=0;
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
      //         cout << "Object: " << h_itr->first << " buffer size:" << buf.BufferSize() <<endl;
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
      //         cout << "Object: " << h_itr->first << " buffer size:" << buf.BufferSize() <<endl;
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
//      xdaq::ApplicationDescriptor* d = i2o::utils::getAddressMap()->
//	getApplicationDescriptor(displayServerTID_);
      xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, displayServerTID_);

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
		    cout << "The server status is: " << statusElement[0].getValue() << endl;
		  } else {
		    cout << "Value of element is: " << statusElement[0].getValue() << endl;
		  }
	      } else
		{
		  cout << "Response contains wrong number of elements: " << content.size() << endl;
		}
	  }
      //   return reply;
    }
  catch (xdaq::exception::Exception& e)
    {
      // handle exception
    }

}

void EmuMonitor::asktoupdateObjects()
{
  xoap::MessageReference msg = xoap::createMessage();
  xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope();
  xoap::SOAPBody body = envelope.getBody();
  xoap::SOAPName commandName = envelope.createName("updateObjects");
  xoap::SOAPName originator = envelope.createName("originator");
  xoap::SOAPName targetAddr = envelope.createName("targetAddr");
  xoap::SOAPElement command = body.addBodyElement(commandName );
  /*
    xdaqInteger localTid  (tid());
    xdaqInteger serverTid (displayServerTID_);

    command.addAttribute (targetAddr, serverTid.toString() );
    command.addAttribute (originator, localTid.toString() );
  */
  try
    {
//      xdaq::ApplicationDescriptor* d = i2o::utils::getAddressMap()->
//	getApplicationDescriptor(displayServerTID_);
      xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, displayServerTID_);

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
		    cout << "The server status is: " << statusElement[0].getValue() << endl;
		  } else {
		    cout << "Value of element is: " << statusElement[0].getValue() << endl;
		  }
	      } else
		{
		  cout << "Response contains wrong number of elements: " << content.size() << endl;
		}
	  }
      //   return reply;
    }
  catch (xdaq::exception::Exception& e)
    {
      // handle exception
    }
}


xoap::MessageReference EmuMonitor::sendObjects (xoap::MessageReference msg) throw (xoap::exception::Exception)
{
/*
  xdaqInteger localTid  (tid());
  SOAPMessage msg;
  SOAPEnvelope envelope = msg.getSOAPPart().getEnvelope();
  SOAPBody b = envelope.getBody();
  SOAPName replyName = envelope.createName("sendObjectsReply", "", "");
  SOAPBodyElement e = b.addBodyElement ( replyName );
  SOAPName monitorName = envelope.createName("DQMNode", "", "");
  SOAPElement monitorElement = e.addChildElement(monitorName);
  monitorElement.addTextNode(localTid.toString());
  // b.addTextNode ("Monitor Objects Attached");
  int cnt=0;
  for (map<int, map<string, TH1*> >::iterator itr = plotter_->histos.begin();
       itr != plotter_->histos.end(); ++itr) {
    string dir="";
    if (int id=itr->first) {
      stringstream stdir;
      dir.clear();
      stdir.clear();
      stdir << "CSC_" << ((id>>4)&0xFF) << "_" << (id&0xF) << "/" << endl;
      stdir >> dir;
    }
    string location = dir;
    for (map<string, TH1*>::iterator h_itr = itr->second.begin();
	 h_itr != itr->second.end(); ++h_itr) {
      // cout << "Count: " << cnt << endl;
      if (cnt < 1) {
	xdaqString hname  (h_itr->first);
	TMessage buf(kMESS_OBJECT);
	buf.Reset();
	buf.SetWriteMode();
	buf.WriteObjectAny(h_itr->second, h_itr->second->Class());
	//         cout << "Object: " << h_itr->first << " buffer size:" << buf.BufferSize() <<endl;
	char * attch_buf = new char[buf.BufferSize()];
	buf.Reset();
	buf.SetReadMode();
	buf.ReadBuf(attch_buf, buf.BufferSize());
	string contenttype = "content/unknown";
	// string contenttype = "application/octet-stream";
	// AttachmentPart * attachment = msg.createAttachmentPart(attch_buf, buf.BufferSize(), "content/unknown");
	// attachment->addMimeHeader("Content-Description", h_itr->first);
	// attachment->setContentLocation(location+h_itr->first);
	// msg.addAttachmentPart(attachment);
	delete attch_buf;
	cnt++;
      }
    }
  }
*/
  return msg;
}

//
// SOAP Callback trigger state change
//
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

// XGI Call back

void EmuMonitor::Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
        wsm_.displayPage(out);
}

// WSM Dispatcher function
void EmuMonitor::dispatch (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception)
{
        cgicc::Cgicc cgi(in);
        //const cgicc::CgiEnvironment& env = cgi.getEnvironment();
        cgicc::const_form_iterator stateInputElement = cgi.getElement("StateInput");
        std::string stateInput = (*stateInputElement).getValue();
        wsm_.fireEvent(stateInput,in,out);
}


//
// Web Events that trigger state changes (result of wsm::fireEvent)
//
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

//
// Web Navigation Pages
//
void EmuMonitor::stateMachinePage( xgi::Output * out ) throw (xgi::exception::Exception)
{
        *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
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
                        *out << cgicc::input() .set("type", "submit").set("name", "StateInput").set("value", (*i) ).set("disabled",
"true");
                }

                *out << cgicc::form();
                *out << "</td>" << std::endl;
        }
       *out << "</tr>" << std::endl;
        *out << "</table>" << std::endl;
        //

        *out << cgicc::hr() << std::endl;
/*
        std::string measurementURL = "/";
        measurementURL += getApplicationDescriptor()->getURN();
        measurementURL += "/displayMeasurements";

        *out << cgicc::br() << "<a href=\"" << measurementURL << "\">" << "Display Measurements" << "</a>" << cgicc::br() << endl;
*/

        xgi::Utils::getPageFooter(*out);
}

//
// Failure Pages
//
void EmuMonitor::failurePage(xgi::Output * out, xgi::exception::Exception & e)  throw (xgi::exception::Exception)
{
        *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
        *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;

        xgi::Utils::getPageHeader
                (out,
                "EmuDisplayServer Failure",
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



