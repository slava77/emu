// $Id: EmuPeripheralCrateBase.cc,v 1.11 2011/11/11 19:10:40 liu Exp $

#include "emu/pc/EmuPeripheralCrateBase.h"

#include "xdaq/NamespaceURI.h"  // XDAQ_NS_URI
#include "xoap/MessageFactory.h"  // createMessage()
#include "xoap/SOAPPart.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/domutils.h"  // XMLCh2String()
#include "toolbox/fsm/FailedEvent.h"
#include <time.h>
#include <iomanip>
#include <sstream>

namespace emu {
  namespace pc {

EmuPeripheralCrateBase::EmuPeripheralCrateBase(xdaq::ApplicationStub * s): xdaq::WebApplication(s)
{	
        fsm_.setFailedStateTransitionAction(this, &EmuPeripheralCrateBase::transitionFailed);
        fsm_.setFailedStateTransitionChanged(this, &EmuPeripheralCrateBase::changeState);
        fsm_.setStateName('F', "Failed");

        state_ = "";
        getApplicationInfoSpace()->fireItemAvailable("State", &state_);
        getApplicationInfoSpace()->fireItemAvailable("stateName", &state_);

        LOG4CPLUS_INFO(getApplicationLogger(), "Supervised");
        activeEndcap_ = NULL;
        activeParser_ = NULL;
        activeTStore_ = NULL;
        xml_or_db_ = 0;
        real_key_ = "";
}  

void EmuPeripheralCrateBase::changeState(toolbox::fsm::FiniteStateMachine &fsm)
        throw (toolbox::fsm::exception::Exception)
{
    state_ = fsm.getStateName(fsm.getCurrentState());

    LOG4CPLUS_DEBUG(getApplicationLogger(), "StateChanged: " << (std::string)state_);
}

void EmuPeripheralCrateBase::transitionFailed(toolbox::Event::Reference event)
        throw (toolbox::fsm::exception::Exception)
{
	toolbox::fsm::FailedEvent &failed =
			dynamic_cast<toolbox::fsm::FailedEvent &>(*event);

	LOG4CPLUS_INFO(getApplicationLogger(),
			"Failure occurred when performing transition"
			<< " from: " << failed.getFromState()
			<< " to: " << failed.getToState()
			<< " exception: " << failed.getException().what());
}

void EmuPeripheralCrateBase::fireEvent(std::string name)
		throw (toolbox::fsm::exception::Exception)
{
	toolbox::Event::Reference event((new toolbox::Event(name, this)));

	fsm_.fireEvent(event);
}

xoap::MessageReference EmuPeripheralCrateBase::createReply(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	std::string command = "";

	DOMNodeList *elements =
			message->getSOAPPart().getEnvelope().getBody()
			.getDOMNode()->getChildNodes();

	for (unsigned int i = 0; i < elements->getLength(); i++) {
		DOMNode *e = elements->item(i);
		if (e->getNodeType() == DOMNode::ELEMENT_NODE) {
			command = xoap::XMLCh2String(e->getLocalName());
			break;
		}
	}

	xoap::MessageReference reply = xoap::createMessage();
	xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
	xoap::SOAPName responseName = envelope.createName(
			command + "Response", "xdaq", XDAQ_NS_URI);
	envelope.getBody().addBodyElement(responseName);

	return reply;
}

xoap::MessageReference EmuPeripheralCrateBase::createReplywithAttr(xoap::MessageReference message, std::string tag, std::string attr)
		throw (xoap::exception::Exception)
{
	std::string command = "";

	DOMNodeList *elements =
			message->getSOAPPart().getEnvelope().getBody()
			.getDOMNode()->getChildNodes();

	for (unsigned int i = 0; i < elements->getLength(); i++) {
		DOMNode *e = elements->item(i);
		if (e->getNodeType() == DOMNode::ELEMENT_NODE) {
			command = xoap::XMLCh2String(e->getLocalName());
			break;
		}
	}

	xoap::MessageReference reply = xoap::createMessage();
	xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
	xoap::SOAPName responseName = envelope.createName(
			command + "Response", "xdaq", XDAQ_NS_URI);
	envelope.getBody().addBodyElement(responseName);

        xoap::SOAPName p = envelope.createName(tag);
        envelope.getBody().addAttribute(p, attr);

	return reply;
}

////////////////////////////////////////////////////////////////////
// sending soap commands
////////////////////////////////////////////////////////////////////
int EmuPeripheralCrateBase::PCsendCommand(std::string command, std::string klass, int instance)
  throw (xoap::exception::Exception, xdaq::exception::Exception){
  //
  int num = 0;

  // find applications
  std::set<xdaq::ApplicationDescriptor *> apps;
  //
  try {
    if(instance < 0)
       apps = getApplicationContext()->getDefaultZone()->getApplicationDescriptors(klass);
    else
       apps.insert(getApplicationContext()->getDefaultZone()->getApplicationDescriptor(klass, instance));
  }
  catch (xdaq::exception::ApplicationDescriptorNotFound e) {
    return num; // Do nothing if the target doesn't exist
  }
  //
  // prepare a SOAP message
  xoap::MessageReference message = PCcreateCommandSOAP(command);
  xoap::MessageReference reply;
  xdaq::ApplicationDescriptor *ori=this->getApplicationDescriptor();
  //
  // send the message one-by-one
  std::set<xdaq::ApplicationDescriptor *>::iterator i = apps.begin();
  for (; i != apps.end(); ++i) {
    // postSOAP() may throw an exception when failed.
    try {
        reply = getApplicationContext()->postSOAP(message, *ori, *(*i));
        num++;
    }
    catch (xcept::Exception &e) {
        std::cout << getLocalDateTime() << " PCsendCommand failed, command=" << command 
                  << ", class=" << klass << ", instance=" << instance << ", return=" << num << std::endl;
    }
    //
  }
  return num;
}

int EmuPeripheralCrateBase::PCsendCommandwithAttr(std::string command, std::string tag, std::string attr, std::string klass, int instance)
  throw (xoap::exception::Exception, xdaq::exception::Exception){
  //
  int num = 0;

  // find applications
  std::set<xdaq::ApplicationDescriptor *> apps;
  //
  try {
    if(instance < 0)
       apps = getApplicationContext()->getDefaultZone()->getApplicationDescriptors(klass);
    else
       apps.insert(getApplicationContext()->getDefaultZone()->getApplicationDescriptor(klass, instance));
  }
  catch (xdaq::exception::ApplicationDescriptorNotFound e) {
    return num; // Do nothing if the target doesn't exist
  }
  //
  // prepare a SOAP message
  xoap::MessageReference message = PCcreateCommandSOAPwithAttr(command, tag, attr);
  xoap::MessageReference reply;
  xdaq::ApplicationDescriptor *ori=this->getApplicationDescriptor();
  //
  // send the message one-by-one
  std::set<xdaq::ApplicationDescriptor *>::iterator i = apps.begin();
  for (; i != apps.end(); ++i) {
    // postSOAP() may throw an exception when failed.
    try {
        reply = getApplicationContext()->postSOAP(message, *ori, *(*i));
        num++;
    }
    catch (xcept::Exception &e) {
        std::cout << getLocalDateTime() << " PCsendCommandwithAttr failed, command=" << command 
                  << ", class=" << klass << ", instance=" << instance << ", return=" << num << std::endl;
    }
    //
  }
  return num;
}
//
xoap::MessageReference EmuPeripheralCrateBase::PCcreateCommandSOAP(std::string command) {
  //
  //This is copied from CSCSupervisor::createCommandSOAP
  //
  xoap::MessageReference message = xoap::createMessage();
  xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
  xoap::SOAPName name = envelope.createName(command, "xdaq", "urn:xdaq-soap:3.0");
  envelope.getBody().addBodyElement(name);
  //
  return message;
}

xoap::MessageReference EmuPeripheralCrateBase::PCcreateCommandSOAPwithAttr(
                std::string command, std::string tag, std::string attr)
{
        xoap::MessageReference message = xoap::createMessage();
        xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
        xoap::SOAPName name = envelope.createName(command, "xdaq", XDAQ_NS_URI);
        xoap::SOAPElement element = envelope.getBody().addBodyElement(name);

        xoap::SOAPName p = envelope.createName(tag);
        envelope.getBody().addAttribute(p, attr);

        return message;
}

std::string EmuPeripheralCrateBase::getAttrFromSOAP(xoap::MessageReference message, std::string tag)
{
        xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
        xoap::SOAPName name = envelope.createName(tag);
        std::string value = envelope.getBody().getAttributeValue(name);
        return value;
}

std::string EmuPeripheralCrateBase::getLocalDateTime(bool AsFileName){
  time_t t;
  struct tm *tm;

  time( &t );
  tm = localtime( &t );
  std::string gap0 = "-";
  std::string gap1 = (AsFileName) ? gap0 : " ";
  std::string gap2 = (AsFileName) ? gap0 : ":";

  std::stringstream ss;
  ss << std::setfill('0') << std::setw(4) << tm->tm_year+1900 << gap0
     << std::setfill('0') << std::setw(2) << tm->tm_mon+1     << gap0
     << std::setfill('0') << std::setw(2) << tm->tm_mday      << gap1
     << std::setfill('0') << std::setw(2) << tm->tm_hour      << gap2
     << std::setfill('0') << std::setw(2) << tm->tm_min       << gap2
     << std::setfill('0') << std::setw(2) << tm->tm_sec;

  return ss.str();
}

bool EmuPeripheralCrateBase::CommonParser(std::string XML_or_DB, std::string configKey)
{
  std::string Valid_key, InFlash_key;
  xdata::UnsignedInteger64 Valid_key_64, InFlash_key_64;
//  EmuEndcap* myEndcap_=NULL;
  int use_flash=0;
    //
  std::cout << "XML_or_DB: " << XML_or_DB << std::endl;
  if(XML_or_DB == "xml" || XML_or_DB == "XML")
  {
    //
    LOG4CPLUS_INFO(getApplicationLogger(),"Parsing Configuration XML");
    // Check if filename exists
    //
      std::ifstream filename(configKey.c_str());
      if(filename.is_open()) {
	filename.close();
      }
      else {
	LOG4CPLUS_ERROR(getApplicationLogger(), "Filename doesn't exist");
	XCEPT_RAISE (toolbox::fsm::exception::Exception, "Filename doesn't exist");
	return false;
      }
    //
    //cout <<"Start Parsing"<<endl;

    // if activeEndcap_ exists, return without doing anything
    if ( activeEndcap_) return true;

    //
    if (!activeParser_)  activeParser_ = new XMLParser();

    activeParser_->parseFile(configKey);
    //
    activeEndcap_ = activeParser_->GetEmuEndcap();
    if(!activeEndcap_) return false;
    xml_or_db_ = 0;
    return true;
  }
  else if (XML_or_DB == "db" || XML_or_DB == "DB")
  {
    //
    LOG4CPLUS_INFO(getApplicationLogger(),"Parsing Configuration DB");
    
    if(!activeTStore_) GetEmuTStore();
    if(!activeTStore_)
    {  std::cout << "Can't create object TStoreReadWriter" << std::endl;
       return false;  
    }

    if( configKey=="-1" || configKey=="-2")
    {
       try 
       {
          InFlash_key_64 = activeTStore_->readLastConfigIdFlashed((configKey=="-1")?"plus":"minus" );
          InFlash_key = InFlash_key_64.toString();
       } catch (...) {}

       if (InFlash_key=="") return false;
       use_flash=2;
       Valid_key = InFlash_key;
       Valid_key_64 = InFlash_key_64;
    }   
    else if( configKey!="")
    {  
       Valid_key = configKey;
       Valid_key_64 = atoi(configKey.c_str());
    }

    if( Valid_key != "" && Valid_key != real_key_)
    {  // re-configuration needed
       if ( activeEndcap_) {
          std::cout << "Delete existing EmuEndcap" << std::endl;      
          delete activeEndcap_ ;
       }
       std::cout << "Configuration ID: " << Valid_key << std::endl;
       activeTStore_->read(Valid_key_64);
       emu::db::ConfigTree tree_from_db(activeTStore_->configTables());
       EmuEndcapConfigWrapper wrapper_from_db(&tree_from_db);
       activeEndcap_ = wrapper_from_db.getConfiguredEndcap();   
       if(!activeEndcap_) 
        {  std::cout << "No EmuEndcap returned from TStore" << std::endl;
           return false;
        }
       xml_or_db_ = 1-use_flash;
       real_key_ = Valid_key;
       return true;
    }
    else return false;
  }
  else
  {
    std::cout << "No valid XML_or_DB found..." << std::endl;
    return false;
  }
}

emu::db::TStoreReadWriter *EmuPeripheralCrateBase::GetEmuTStore()
{ 
    if(!activeTStore_)
    {
         emu::db::PCConfigHierarchy *h= new emu::db::PCConfigHierarchy("/opt/xdaq/htdocs/emu/emuDCS/OnlineDB/xml/EMUsystem.view");
         activeTStore_ = new emu::db::TStoreReadWriter(h, this, "", 0);
    }
    return activeTStore_; 
}
 
 }  // namespace emu::pc
}  // namespace emu
