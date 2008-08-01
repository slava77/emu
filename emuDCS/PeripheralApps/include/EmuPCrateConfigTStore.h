#ifndef _EmuPCrateConfigTStore_h_
#define _EmuPCrateConfigTStore_h_

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


class EmuPCrateConfigTStore: public xdaq::Application
{

public:

  XDAQ_INSTANTIATOR();

  EmuPCrateConfigTStore(xdaq::ApplicationStub * s) throw (xdaq::exception::Exception);
  //
  void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  
  // HyperDAQ interface
  void outputHeader(xgi::Output * out);
  void outputFooter(xgi::Output * out);
  void outputException(xgi::Output * out,xcept::Exception &e);
  void outputStandardInterface(xgi::Output * out);
  
  // Actions
  void parseConfigFromXML(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void uploadConfigToDB(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void readConfigFromDB(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void synchronizeToFromDB(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void getConfigurationFromXML(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

  // Communication with TStore
  xoap::MessageReference sendSOAPMessage(xoap::MessageReference &message) throw (xcept::Exception);
  std::string connect() throw (xcept::Exception);
  void disconnect(const std::string &connectionID) throw (xcept::Exception);
  void query(const std::string &connectionID, const std::string &queryViewName, const std::string &emu_config_id, xdata::Table &results) throw (xcept::Exception);
  void query(const std::string &connectionID, const std::string &queryViewName, const std::string &emu_config_id, const std::string &xxx_config_id, xdata::Table &results) throw (xcept::Exception);
  void queryMaxId(const std::string &connectionID,const std::string &queryViewName, const std::string &dbTable, const std::string &dbColumn, const std::string endcap_side, xdata::Table &results) throw (xcept::Exception);
  void getDefinition(const std::string &connectionID, const std::string &insertViewName, xdata::Table &results) throw (xcept::Exception);
  void insert(const std::string &connectionID, const std::string &insertViewName, xdata::Table &newRows) throw (xcept::Exception);
  void synchronize(const std::string &connectionID, const std::string &syncMode, const std::string &syncPattern) throw (xcept::Exception);
  void getConfiguration(const std::string &xpath) throw (xcept::Exception);

  //
  void getDbUserData();

  // XML Data Uploads
  void startUpload() throw (xcept::Exception);
  xdata::UnsignedInteger64 getConfigId(const std::string &dbTable, const std::string &dbColumn, const std::string endcap_side) throw (xcept::Exception);
  void uploadConfiguration(const std::string &connectionID, const std::string endcap_side) throw (xcept::Exception);
  void uploadPeripheralCrate(const std::string &connectionID, const std::vector<Crate *> &TStore_allCrates) throw (xcept::Exception);
  void uploadCCB(const std::string &connectionID, xdata::UnsignedInteger64 &periph_config_id, CCB * TStore_thisCCB) throw (xcept::Exception);
  void uploadMPC(const std::string &connectionID, xdata::UnsignedInteger64 &periph_config_id, MPC * TStore_thisMPC) throw (xcept::Exception);
  void uploadVMECC(const std::string &connectionID, xdata::UnsignedInteger64 &periph_config_id, Crate * TStore_thisCrate) throw (xcept::Exception);
  void uploadCSC(const std::string &connectionID, xdata::UnsignedInteger64 &periph_config_id, const std::vector<Chamber *> &TStore_allChambers) throw (xcept::Exception);
  void uploadDAQMB(const std::string &connectionID, xdata::UnsignedInteger64 &csc_config_id, DAQMB * &TStore_thisDAQMB) throw (xcept::Exception);
  void uploadTMB(const std::string &connectionID, xdata::UnsignedInteger64 &csc_config_id, TMB * &TStore_thisTMB) throw (xcept::Exception);
  void uploadALCT(const std::string &connectionID, xdata::UnsignedInteger64 &tmb_config_id, ALCTController * &TStore_thisALCT) throw (xcept::Exception);
  void uploadAnodeChannel(const std::string &connectionID, xdata::UnsignedInteger64 &alct_config_id, ALCTController * &TStore_thisALCT) throw (xcept::Exception);
  void uploadCFEB(const std::string &connectionID, xdata::UnsignedInteger64 &daqmb_config_id, DAQMB * &TStore_thisDAQMB) throw (xcept::Exception);

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

#endif
