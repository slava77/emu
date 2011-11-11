// $Id: EmuPeripheralCrateBase.h,v 1.8 2011/11/11 19:10:40 liu Exp $

#ifndef _EmuPeripheralCrateBase_h_
#define _EmuPeripheralCrateBase_h_

#include "xgi/Utils.h"
#include "xgi/Method.h"

#include "xdaq/Application.h"
#include "xdaq/ApplicationGroup.h"
#include "xdaq/ApplicationContext.h"
#include "xdaq/ApplicationStub.h"
#include "xdaq/exception/Exception.h"
#include "xdaq/WebApplication.h"

#include "xdaq/NamespaceURI.h"

#include "xoap/MessageReference.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/Method.h"
#include "toolbox/fsm/FiniteStateMachine.h"
#include <string>
#include "xdata/String.h"
#include "xdata/UnsignedInteger64.h"
#include "emu/pc/EmuEndcap.h"
#include "emu/pc/XMLParser.h"

#include "emu/db/TableDefinitions.h"
#include "emu/db/PCConfigHierarchy.h"
#include "emu/db/ConfigTable.h"
#include "emu/db/ConfigRow.h"  
#include "emu/db/ConfigTree.h" 
#include "emu/db/TStoreReadWriter.h"
#include "emu/pc/EmuEndcapConfigWrapper.h"


namespace emu {
  namespace pc {
  
class EmuPeripheralCrateBase: public xdaq::WebApplication
{
  
public:
  
  EmuPeripheralCrateBase(xdaq::ApplicationStub * s);
  //
protected:
  void changeState(toolbox::fsm::FiniteStateMachine &fsm)
                        throw (toolbox::fsm::exception::Exception);
  void transitionFailed(toolbox::Event::Reference event)
                        throw (toolbox::fsm::exception::Exception);
  void fireEvent(std::string event) throw (toolbox::fsm::exception::Exception);

  xoap::MessageReference createReply(xoap::MessageReference message)
                        throw (xoap::exception::Exception);
  xoap::MessageReference createReplywithAttr(xoap::MessageReference message, std::string tag, std::string attr)
                        throw (xoap::exception::Exception);
  std::string getAttrFromSOAP(xoap::MessageReference message, std::string tag);

  //
  xoap::MessageReference PCcreateCommandSOAP(std::string command);
  int PCsendCommand(std::string command, std::string klass, int instance = -1) 
                        throw (xoap::exception::Exception, xdaq::exception::Exception);

  xoap::MessageReference PCcreateCommandSOAPwithAttr(std::string command, std::string tag, std::string attr);
  int PCsendCommandwithAttr(std::string command, std::string tag, std::string attr, std::string klass, int instance = -1) 
                        throw (xoap::exception::Exception, xdaq::exception::Exception);
  std::string getLocalDateTime(bool AsFileName = false);
  bool CommonParser(std::string XML_or_DB, std::string xmlFile);
  inline EmuEndcap *GetEmuEndcap() { return activeEndcap_; }
  emu::db::TStoreReadWriter *GetEmuTStore();
  inline int Xml_or_Db() { return xml_or_db_; }
  inline std::string GetRealKey() { return real_key_; }
  //
  toolbox::fsm::FiniteStateMachine fsm_;
  xdata::String state_;

private:
  int xml_or_db_;
  std::string real_key_;
  EmuEndcap * activeEndcap_;
  XMLParser * activeParser_;
  emu::db::TStoreReadWriter* activeTStore_;
};

  } // namespace emu::pc
} // namespace emu
  
#endif
