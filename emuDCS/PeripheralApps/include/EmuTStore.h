#ifndef _EmuTStore_h_
#define _EmuTStore_h_

#include "xdaq/Application.h"
#include "xdaq/ApplicationGroup.h"
#include "xdaq/ApplicationContext.h"
#include "xdaq/ApplicationStub.h"
#include "xdaq/exception/Exception.h"

#include "xdaq/NamespaceURI.h"

#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"

#include "xgi/Utils.h"
#include "xgi/Method.h"
#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"
#include "xdata/Table.h"
#include "xdata/UnsignedInteger64.h"

#include "CCB.h"
#include "MPC.h"
#include "VMEController.h"
#include "TMB.h"
#include "ALCTController.h"
#include "RAT.h"
#include "DAQMB.h"
#include "CFEB.h"
#include "Chamber.h"
#include "Crate.h"
#include "EmuEndcap.h"
#include "VMECC.h"

#include <sstream>


namespace emu {
  namespace pc {

class EmuTStore
{

public:

  EmuTStore(xdaq::Application *s);

  // Communication with TStore
  xoap::MessageReference sendSOAPMessage(xoap::MessageReference &message) throw (xcept::Exception);
  std::string connect() throw (xcept::Exception);
  void disconnect(const std::string &connectionID) throw (xcept::Exception);
  void query(const std::string &connectionID, const std::string &queryViewName, const std::string &emu_config_id, xdata::Table &results) throw (xcept::Exception);
  void query(const std::string &connectionID, const std::string &queryViewName, const std::string &emu_config_id, const std::string &xxx_config_id, xdata::Table &results) throw (xcept::Exception);
  void queryMaxId(const std::string &connectionID,const std::string &queryViewName, const std::string &dbTable, const std::string &dbColumn, const std::string endcap_side, xdata::Table &results) throw (xcept::Exception);
  void getDefinition(const std::string &connectionID, const std::string &insertViewName, xdata::Table &results) throw (xcept::Exception);
  void insert(const std::string &connectionID, const std::string &insertViewName, xdata::Table &newRows) throw (xcept::Exception);
  void getConfiguration(const std::string &xpath) throw (xcept::Exception);

  //
  void getDbUserData();

  // DB Data Reads
  EmuEndcap * getConfiguredEndcap(const std::string &emu_config_id) throw (xcept::Exception);
  void readConfiguration(const std::string &connectionID, const std::string &emu_config_id, EmuEndcap * endcap) throw (xcept::Exception);
  void readPeripheralCrate(const std::string &connectionID, const std::string &emu_config_id, EmuEndcap * endcap) throw (xcept::Exception);
  void readVCC(const std::string &connectionID, const std::string &emu_config_id, const std::string &periph_config_id, Crate * theCrate) throw (xcept::Exception);
  void readCCB(const std::string &connectionID, const std::string &emu_config_id, const std::string &periph_config_id, Crate * theCrate) throw (xcept::Exception);
  void readMPC(const std::string &connectionID, const std::string &emu_config_id, const std::string &periph_config_id, Crate * theCrate) throw (xcept::Exception);
  void readCSC(const std::string &connectionID, const std::string &emu_config_id, const std::string &periph_config_id, Crate * theCrate) throw (xcept::Exception);
  void readDAQMB(const std::string &connectionID, const std::string &emu_config_id, const std::string &csc_config_id, Crate * theCrate, Chamber * theChamber) throw (xcept::Exception);
  void readCFEB(const std::string &connectionID, const std::string &emu_config_id, const std::string &daqmb_config_id, DAQMB * theDaqmb) throw (xcept::Exception);
  void readTMB(const std::string &connectionID, const std::string &emu_config_id, const std::string &csc_config_id, Crate * theCrate, Chamber * theChamber) throw (xcept::Exception);
  void readALCT(const std::string &connectionID, const std::string &emu_config_id, const std::string &tmb_config_id, TMB * theTmb) throw (xcept::Exception);
  void readAnodeChannel(const std::string &connectionID, const std::string &emu_config_id, const std::string &alct_config_id, ALCTController * theAlct) throw (xcept::Exception);

private:

  xdaq::Application *thisApp;
  std::string xmlfile_;
  std::string dbUserFile_;
  std::string dbUserAndPassword_;
  EmuEndcap * TStore_myEndcap_;
  xdata::UnsignedInteger64 emu_config_id_;
  
  xdata::Table tableDefinition_emu_configuration;
  xdata::Table tableDefinition_emu_peripheralcrate;
  xdata::Table tableDefinition_emu_ccb;
  xdata::Table tableDefinition_emu_mpc;
  xdata::Table tableDefinition_emu_vcc;
  xdata::Table tableDefinition_emu_csc;
  xdata::Table tableDefinition_emu_daqmb;
  xdata::Table tableDefinition_emu_cfeb;
  xdata::Table tableDefinition_emu_tmb;
  xdata::Table tableDefinition_emu_alct;
  xdata::Table tableDefinition_emu_anodechannel;

};

// Convert to string

template <class T>
inline std::string to_string (const T& t)
{
std::stringstream ss;
ss << t;
return ss.str();
}

// Convert to HEX

template <class T>
inline bool convertToHex(std::string &dataHex, const std::string format, const T& data)
{
  char buffer [100];
  int err;
  bool converted = false;
  err = sprintf(buffer,format.c_str(),data);
  if (err==0){
    std::cout << "Could not convert " << to_string(data) << " to hexadecimal format" << std::endl;
    dataHex = "";
  }else{
    dataHex = to_string(buffer);
    converted = true;
  }
  return converted;
}
  } // namespace emu::pc
  } // namespace emu

#endif
