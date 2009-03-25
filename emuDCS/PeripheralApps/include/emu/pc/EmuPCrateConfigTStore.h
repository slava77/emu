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

#include "emu/pc/CCB.h"
#include "emu/pc/MPC.h"
#include "emu/pc/VMEController.h"
#include "emu/pc/TMB.h"
#include "emu/pc/ALCTController.h"
#include "emu/pc/RAT.h"
#include "emu/pc/DAQMB.h"
#include "emu/pc/CFEB.h"
#include "emu/pc/Chamber.h"
#include "emu/pc/Crate.h"
#include "emu/pc/EmuEndcap.h"
#include "emu/pc/VMECC.h"

#include <sstream>

namespace emu {
  namespace pc {

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
  
  void displayChildConfiguration(xgi::Output * out,const std::string &configName,const std::string &parentIdentifier);
  void displayConfiguration(xgi::Output * out,const std::string &configName,const std::string &identifier);
  void displayConfiguration(xgi::Output * out,const std::string &configName,int crateID);
  void outputTableEditControls(xgi::Output * out,const std::string &tableName,const std::string &prefix="");
  void outputSingleValue(xgi::Output * out,xdata::Serializable *value,const std::string &tableName="",const std::string &identifier="",const std::string &column="",int rowIndex=0);
  void outputTable(xgi::Output * out,xdata::Table &results,const std::string &tableName="",const std::string &identifier="");
void outputShowHideButton(xgi::Output * out,const std::string &configName,const std::string &identifier);
  // Actions
  void parseConfigFromXML(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void uploadConfigToDB(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void readConfigFromDB(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void synchronizeToFromDB(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void SelectConfFile(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void SetTypeDesc(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void incrementValue(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void setValue(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void changeSingleValue(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void showTable(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void hideTable(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  // Communication with TStore
  xoap::MessageReference sendSOAPMessage(xoap::MessageReference &message) throw (xcept::Exception);
  std::string connect() throw (xcept::Exception);
  void disconnect(const std::string &connectionID) throw (xcept::Exception);
  void query(const std::string &connectionID, const std::string &queryViewName, const std::string &emu_config_id, xdata::Table &results) throw (xcept::Exception);
  void query(const std::string &connectionID, const std::string &queryViewName, const std::string &emu_config_id, const std::string &xxx_config_id, xdata::Table &results) throw (xcept::Exception);
  void queryMaxId(const std::string &connectionID,const std::string &queryViewName, const std::string &dbTable, const std::string &dbColumn, const std::string endcap_side, xdata::Table &results) throw (xcept::Exception);
  void getDefinition(const std::string &connectionID, const std::string &insertViewName) throw (xcept::Exception);
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
  void uploadCCB(const std::string &connectionID, xdata::UnsignedInteger64 &periph_config_id, int crateID) throw (xcept::Exception);
  void uploadMPC(const std::string &connectionID, xdata::UnsignedInteger64 &periph_config_id, int crateID) throw (xcept::Exception);
  void uploadVMECC(const std::string &connectionID, xdata::UnsignedInteger64 &periph_config_id, int crateID) throw (xcept::Exception);
  void uploadCSC(const std::string &connectionID, xdata::UnsignedInteger64 &periph_config_id, const std::vector<Chamber *> &TStore_allChambers,int crateID) throw (xcept::Exception);
  void uploadDAQMB(const std::string &connectionID, xdata::UnsignedInteger64 &csc_config_id, DAQMB * &TStore_thisDAQMB,int slot,const std::string &chamber) throw (xcept::Exception);
  void uploadTMB(const std::string &connectionID, xdata::UnsignedInteger64 &csc_config_id, TMB * &TStore_thisTMB,int slot,const std::string &chamber) throw (xcept::Exception);
  void uploadALCT(const std::string &connectionID, xdata::UnsignedInteger64 &tmb_config_id, ALCTController * &TStore_thisALCT,const std::string &identifier) throw (xcept::Exception);
  void uploadAnodeChannel(const std::string &connectionID, xdata::UnsignedInteger64 &alct_config_id, ALCTController * &TStore_thisALCT,const std::string &identifier) throw (xcept::Exception);
  void uploadCFEB(const std::string &connectionID, xdata::UnsignedInteger64 &daqmb_config_id,const std::string &identifier) throw (xcept::Exception);

  // DB Data Reads
  EmuEndcap * getConfiguredEndcap(const std::string &emu_config_id) throw (xcept::Exception);
  void readConfiguration(const std::string &connectionID, const std::string &emu_config_id, EmuEndcap * endcap) throw (xcept::Exception);
  void readPeripheralCrate(const std::string &connectionID, const std::string &emu_config_id, EmuEndcap * endcap) throw (xcept::Exception);
  void readVCC(const std::string &connectionID, const std::string &emu_config_id, const std::string &periph_config_id, Crate * theCrate) throw (xcept::Exception);
  void readCCB(const std::string &connectionID, const std::string &emu_config_id, const std::string &periph_config_id, Crate * theCrate) throw (xcept::Exception);
  void readMPC(const std::string &connectionID, const std::string &emu_config_id, const std::string &periph_config_id, Crate * theCrate) throw (xcept::Exception);
  void readCSC(const std::string &connectionID, const std::string &emu_config_id, const std::string &periph_config_id, Crate * theCrate) throw (xcept::Exception);
  void readDAQMB(const std::string &connectionID, const std::string &emu_config_id, const std::string &csc_config_id, Crate * theCrate, Chamber * theChamber) throw (xcept::Exception);
  void readCFEB(const std::string &connectionID, const std::string &emu_config_id, const std::string &daqmb_config_id, DAQMB * theDaqmb,const std::string &cacheIdentifier) throw (xcept::Exception);
  void readTMB(const std::string &connectionID, const std::string &emu_config_id, const std::string &csc_config_id, Crate * theCrate, Chamber * theChamber) throw (xcept::Exception);
  void readALCT(const std::string &connectionID, const std::string &emu_config_id, const std::string &tmb_config_id, TMB * theTmb,const std::string &identifier) throw (xcept::Exception);
  void readAnodeChannel(const std::string &connectionID, const std::string &emu_config_id, const std::string &alct_config_id, ALCTController * theAlct,const std::string &identifier) throw (xcept::Exception);

private:
	bool shouldDisplayConfiguration(const std::string &configName,const std::string &identifier);
	std::string fullTableID(const std::string &configName,const std::string &identifier);
	bool tableHasColumn(xdata::Table &table,const std::string &column);
	void setValueFromString(xdata::Serializable *value,const std::string &newValue);
	void getRangeOfTables(const cgicc::Cgicc &cgi,std::map<std::string,xdata::Table> &tables,std::map<std::string,xdata::Table>::iterator &firstTable,std::map<std::string,xdata::Table>::iterator &lastTable);
	void setConfigID(xdata::Table &newRows,size_t rowId,const std::string &columnName,xdata::UnsignedInteger64 &id);
	std::string crateIdentifierString(int crateID);
	std::string chamberID(int crateID,const std::string &chamberLabel);
	std::string DAQMBID(const std::string &chamber,int slot);
	std::string getEndcapSide(std::vector<Crate *> &myCrates) throw (xcept::Exception);
	xdata::Table &getCachedTable(const std::string &insertViewName,const std::string &identifier) throw (xcept::Exception);
	void setCachedTable(const std::string &insertViewName,const std::string &identifier,xdata::Table &table) throw (xcept::Exception);

	xdata::Table &getCachedTable(const std::string &insertViewName,int crateIndex) throw (xcept::Exception);
	void setCachedTable(const std::string &insertViewName,int crateIndex,xdata::Table &table) throw (xcept::Exception);
	bool canChangeColumn(const std::string &columnName);
	bool isNumericType(const std::string &xdataType);
	void getTableDefinitionsIfNecessary() throw ();
	void getTableDefinitions(const std::string &connectionID);
	void copyAnodeChannelToTable(xdata::Table &newRows,ALCTController * &TStore_thisALCT);
	void copyALCTToTable(xdata::Table &newRows,ALCTController * &TStore_thisALCT);
	void copyTMBToTable(xdata::Table &newRows,TMB * TStore_thisTMB);
	void copyCFEBToTable(xdata::Table &newRows,DAQMB * TStore_thisDAQMB);
	void copyDAQMBToTable(xdata::Table &newRows,DAQMB * TStore_thisDAQMB);
	void copyCSCToTable(xdata::Table &newRows,Chamber * chamber);
	void copyMPCToTable(xdata::Table &newRows,Crate * TStore_thisCrate);
	void copyCCBToTable(xdata::Table &newRows,Crate * TStore_thisCrate);
	void copyVMECCToTable(xdata::Table &newRows,Crate * TStore_thisCrate);
  std::string config_type_;
  std::string config_desc_;
  std::string xmlpath_;
  std::string xmlfile_;
  std::string dbUserFile_;
  std::string dbUserAndPassword_;
  EmuEndcap * TStore_myEndcap_;
  xdata::UnsignedInteger64 emu_config_id_;
  
  //the data is kept as tables in memory, to allow a common interface for changing values
  //the most recent data is always in these tables.
  //the key in the first map is the table name as passed to TStore (e.g. "vcc", "tmb")
  //the value is a map of all tables which come from a query of that table (e.g. vcc from a particular crate)
  //the key in this inside map can be anything which uniquely identifies that particular table 
  //so it can be edited.
  //it is named hierarchically beginning with the crate identifier so that it's easy to loop through
  //just the tables relating to a particular crate or chamber to change the values.
  std::map<std::string,std::map<std::string,xdata::Table> > currentTables;
  std::map<std::string,bool> tablesToDisplay;
  
  std::map<std::string,xdata::Table> tableDefinitions;
  std::map<std::string,std::vector<std::string> > tableNames;

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
  } else{
    dataHex = to_string(buffer);
    converted = true;
  }
  return converted;
}

} // namespace emu::pc
} // namespace emu


#endif
