#ifndef _EmuPCrateConfigTStore_h_
#define _EmuPCrateConfigTStore_h_

#include "emu/db/ConfigurationEditor.h"
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

#include "xdata/String.h"

#include <sstream>

namespace emu {
namespace pc {

typedef std::map<std::string,unsigned int> TableChangeSummary;
typedef std::map<std::string,TableChangeSummary > ChangeSummary;


class EmuPCrateConfigTStore: public emu::db::ConfigurationEditor
{

public:

  XDAQ_INSTANTIATOR();

  EmuPCrateConfigTStore(xdaq::ApplicationStub * s) throw (xdaq::exception::Exception);
  //
  //void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

  // HyperDAQ interface
  //void outputException(xgi::Output * out, xcept::Exception &e);
  void outputStandardInterface(xgi::Output * out);
  bool getVersionNumber(const std::string &columnName, std::string &versionNumber);
  std::string withoutVersionNumber(const std::string &columnName);
  std::string displayName(const std::string &configName, const std::string &identifier, xdata::Table &data);
  //void sumChanges(TableChangeSummary &allChanges, TableChangeSummary &changesToThisTable);

  void SelectTestSummaryFile(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception);
  void parseTestSummary(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception);
  void applyTestSummary(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception);
  void parseConfigFromXML(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception);
  void readConfigFromDB(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception);
  void SelectConfFile(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception);
  void SetTypeDesc(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception);

  // Communication with TStore
  //void getConfigIds(std::vector<std::string> &configIDs, const std::string endcap_side) throw (xcept::Exception);
  void queryMaxId(const std::string &connectionID, const std::string &queryViewName, const std::string &dbTable,
      const std::string &dbColumn, const std::string endcap_side, xdata::Table &results) throw (xcept::Exception);
  //void getConfiguration(const std::string &xpath) throw (xcept::Exception);
  void diff(const std::string &connectionID, const std::string &queryViewName, const std::string &old_emu_config_id,
      const std::string &new_emu_config_id, xdata::Table &results) throw (xcept::Exception);
  void diff(const std::string &connectionID, const std::string &queryViewName, const std::string &old_emu_config_id,
      const std::string &old_xxx_config_id, const std::string &new_emu_config_id, const std::string &new_xxx_config_id,
      xdata::Table &results) throw (xcept::Exception);
  void query(const std::string &connectionID, const std::string &queryViewName, const std::string &emu_config_id,
      xdata::Table &results) throw (xcept::Exception);
  void query(const std::string &connectionID, const std::string &queryViewName, const std::string &emu_config_id,
      const std::string &xxx_config_id, xdata::Table &results) throw (xcept::Exception);
  void insert(const std::string &connectionID, const std::string &insertViewName, xdata::Table &newRows)
      throw (xcept::Exception);

  /** Gets database user's name and password from \ref dbUserFile_ .
   * implementation of virtual function from ConfigurationEditor */
  void getDbUserData();

  // XML Data Uploads

  /** Uploads input config to DB .
   * implementation of virtual function from ConfigurationEditor */
  void startUpload(xgi::Input * in) throw (xcept::Exception);

  xdata::UnsignedInteger64 getConfigId(const std::string &dbTable, const std::string &dbColumn,
      const std::string endcap_side) throw (xcept::Exception);
  void uploadConfiguration(const std::string &connectionID, const std::string endcap_side) throw (xcept::Exception);
  void uploadPeripheralCrate(const std::string &connectionID, const std::vector<Crate *> &TStore_allCrates)
      throw (xcept::Exception);
  void uploadCCB(const std::string &connectionID, xdata::UnsignedInteger64 &periph_config_id, int crateID)
      throw (xcept::Exception);
  void uploadMPC(const std::string &connectionID, xdata::UnsignedInteger64 &periph_config_id, int crateID)
      throw (xcept::Exception);
  void uploadVMECC(const std::string &connectionID, xdata::UnsignedInteger64 &periph_config_id, int crateID)
      throw (xcept::Exception);
  void uploadCSC(const std::string &connectionID, xdata::UnsignedInteger64 &periph_config_id, const std::vector<
      Chamber *> &TStore_allChambers, int crateID) throw (xcept::Exception);
  void uploadDAQMB(const std::string &connectionID, xdata::UnsignedInteger64 &csc_config_id, DAQMB * &TStore_thisDAQMB,
      int slot, const std::string &chamber) throw (xcept::Exception);
  void uploadTMB(const std::string &connectionID, xdata::UnsignedInteger64 &csc_config_id, TMB * &TStore_thisTMB,
      int slot, const std::string &chamber) throw (xcept::Exception);
  void uploadALCT(const std::string &connectionID, xdata::UnsignedInteger64 &tmb_config_id,
      ALCTController * &TStore_thisALCT, const std::string &identifier) throw (xcept::Exception);
  void uploadAnodeChannel(const std::string &connectionID, xdata::UnsignedInteger64 &alct_config_id,
      ALCTController * &TStore_thisALCT, const std::string &identifier) throw (xcept::Exception);
  void uploadCFEB(const std::string &connectionID, xdata::UnsignedInteger64 &daqmb_config_id,
      const std::string &identifier) throw (xcept::Exception);

  // DB Data Reads
  int readCrateID(xdata::Table &results, unsigned rowIndex) throw (xcept::Exception);
  EmuEndcap * getConfiguredEndcap(const std::string &emu_config_id) throw (xcept::Exception);
  void readConfiguration(const std::string &connectionID, const std::string &emu_config_id, EmuEndcap * endcap)
      throw (xcept::Exception);
  void diffCrate(const std::string &connectionID, const std::string &old_emu_config_id,
      const std::string &new_emu_config_id) throw (xcept::Exception);
  void readPeripheralCrate(const std::string &connectionID, const std::string &emu_config_id, EmuEndcap * endcap)
      throw (xcept::Exception);
  void simpleDiff(const std::string &queryViewName, const std::string &connectionID,
      const std::string &old_emu_config_id, const std::string &old_xxx_config_id, const std::string &new_emu_config_id,
      const std::string &new_xxx_config_id, const std::string &identifier) throw (xcept::Exception);
  void simpleCrateDiff(const std::string &queryViewName, const std::string &connectionID,
      const std::string &old_emu_config_id, const std::string &old_periph_config_id,
      const std::string &new_emu_config_id, const std::string &new_periph_config_id, int crateID)
      throw (xcept::Exception);
  //void diffVCC(const std::string &connectionID, const std::string &old_emu_config_id, const std::string &old_periph_config_id, const std::string &new_emu_config_id, const std::string &new_periph_config_id,int crateID) throw (xcept::Exception);
  void readVCC(const std::string &connectionID, const std::string &emu_config_id, const std::string &periph_config_id,
      Crate * theCrate) throw (xcept::Exception);
  void readCCB(const std::string &connectionID, const std::string &emu_config_id, const std::string &periph_config_id,
      Crate * theCrate) throw (xcept::Exception);
  void readMPC(const std::string &connectionID, const std::string &emu_config_id, const std::string &periph_config_id,
      Crate * theCrate) throw (xcept::Exception);
  void diffCSC(const std::string &connectionID, const std::string &old_emu_config_id,
      const std::string &old_periph_config_id, const std::string &new_emu_config_id,
      const std::string &new_periph_config_id, int crateID) throw (xcept::Exception);
  void readCSC(const std::string &connectionID, const std::string &emu_config_id, const std::string &periph_config_id,
      Crate * theCrate) throw (xcept::Exception);
  void diffDAQMB(const std::string &connectionID, const std::string &old_emu_config_id,
      const std::string &old_csc_config_id, const std::string &new_emu_config_id, const std::string &new_csc_config_id,
      const std::string &chamberID) throw (xcept::Exception);
  void readDAQMB(const std::string &connectionID, const std::string &emu_config_id, const std::string &csc_config_id,
      Crate * theCrate, Chamber * theChamber) throw (xcept::Exception);
  void diffCFEB(const std::string &connectionID, const std::string &old_emu_config_id,
      const std::string &old_daqmb_config_id, const std::string &new_emu_config_id,
      const std::string &new_daqmb_config_id, const std::string &cacheIdentifier) throw (xcept::Exception);
  void readCFEB(const std::string &connectionID, const std::string &emu_config_id, const std::string &daqmb_config_id,
      DAQMB * theDaqmb, const std::string &cacheIdentifier) throw (xcept::Exception);
  void diffTMB(const std::string &connectionID, const std::string &old_emu_config_id,
      const std::string &old_csc_config_id, const std::string &new_emu_config_id, const std::string &new_csc_config_id,
      const std::string &chamber) throw (xcept::Exception);
  void readTMB(const std::string &connectionID, const std::string &emu_config_id, const std::string &csc_config_id,
      Crate * theCrate, Chamber * theChamber) throw (xcept::Exception);
  void diffALCT(const std::string &connectionID, const std::string &old_emu_config_id,
      const std::string &old_tmb_config_id, const std::string &new_emu_config_id, const std::string &new_tmb_config_id,
      const std::string &chamber) throw (xcept::Exception);
  void readALCT(const std::string &connectionID, const std::string &emu_config_id, const std::string &tmb_config_id,
      TMB * theTmb, const std::string &identifier) throw (xcept::Exception);
  void diffAnodeChannel(const std::string &connectionID, const std::string &old_emu_config_id,
      const std::string &old_alct_config_id, const std::string &new_emu_config_id,
      const std::string &new_alct_config_id, const std::string &cacheIdentifier) throw (xcept::Exception);
  void readAnodeChannel(const std::string &connectionID, const std::string &emu_config_id,
      const std::string &alct_config_id, ALCTController * theAlct, const std::string &identifier)
      throw (xcept::Exception);

private:
  bool parseTestSummaryChamber(std::istringstream &testSummary, xdata::Table &changes, int &TTCrxID,
      std::string &chamberName) throw (xdaq::exception::Exception);
  bool shouldDisplayInHex(const std::string &columnName);
  //std::string valueToString(xdata::Serializable *value, const std::string &columnName);
  std::string xdataToHex(xdata::Serializable *xdataValue);
  DOMNode *DOMOfCurrentTables();
  std::string attributeNameFromColumnName(const std::string &column);
  std::string elementNameFromTableName(const std::string &column);
  void checkLine(std::istream &input, const std::string &expectedValue, char delimiter = '\n')
      throw (xdaq::exception::Exception);

  //void addChildNodes(DOMElement *parentElement, const std::string &configName, const std::string &parentIdentifier);
  //void addNode(DOMElement *crateElement, const std::string &configName, int crateID); // never defined
  //std::string configIDOptions(std::vector<std::string> &configIDs);
  //std::string newCell(xdata::Serializable *newValue, xdata::Serializable *oldValue);
  //bool getNextColumn(std::vector<std::string>::iterator &nextColumn, std::string &columnWithoutVersionNumber,
  //    const std::vector<std::string>::iterator &currentColumn, const std::vector<std::string>::iterator &end);
  //bool shouldDisplayConfiguration(const std::string &configName, const std::string &identifier);
  //std::string fullTableID(const std::string &configName, const std::string &identifier);
  //void setValueFromString(xdata::Serializable *value, const std::string &newValue);
  //void getRangeOfTables(const cgicc::Cgicc &cgi, std::map<std::string, xdata::Table> &tables, std::map<std::string,
  //    xdata::Table>::iterator &firstTable, std::map<std::string, xdata::Table>::iterator &lastTable);
  void setConfigID(xdata::Table &newRows, size_t rowId, const std::string &columnName, xdata::UnsignedInteger64 &id);
  std::string chamberID(int crateID, const std::string &chamberLabel);
  std::string DAQMBID(const std::string &chamber, int slot);
  std::string getEndcapSide(std::vector<Crate *> &myCrates) throw (xcept::Exception);

  //xdata::Table
  //    &getCachedTableFrom(std::map<std::string, std::map<std::string, xdata::Table> > &cache,
  //        const std::string &insertViewName, const std::string &identifier/*,xdata::UnsignedInteger64 &_vcc_config_id*//*,Crate *thisCrate*/)
  //        throw (xcept::Exception);

  bool canChangeColumnGlobally(const std::string &columnName, const std::string &tableName);
  bool columnIsUniqueIdentifier(const std::string &columnName, const std::string &tableName);
  bool columnIsDatabaseOnly(const std::string &columnName, const std::string &tableName);
  bool canChangeColumn(const std::string &columnName, const std::string &tableName);

  using ConfigurationEditor::setCachedDiff;//(const std::string &insertViewName,const std::string &identifier,xdata::Table &table) throw (xcept::Exception);
  using ConfigurationEditor::setCachedTable;//(const std::string &insertViewName,const std::string &identifier,xdata::Table &table) throw (xcept::Exception);
  void setCachedDiff(const std::string &insertViewName, int crateIndex, xdata::Table &table) throw (xcept::Exception);
  void setCachedTable(const std::string &insertViewName, int crateIndex, xdata::Table &table) throw (xcept::Exception);
  xdata::Table &getCachedDiff(const std::string &insertViewName, int crateID) throw (xcept::Exception);
  xdata::Table &getCachedTable(const std::string &insertViewName, int crateID) throw (xcept::Exception);

  void getTableDefinitions(const std::string &connectionID);

  void copyPeripheralCrateToTable(xdata::Table &newRows, Crate * TStore_thisCrate);
  void copyAnodeChannelToTable(xdata::Table &newRows, ALCTController * &TStore_thisALCT);
  void copyALCTToTable(xdata::Table &newRows, ALCTController * &TStore_thisALCT);
  void copyTMBToTable(xdata::Table &newRows, TMB * TStore_thisTMB);
  void copyCFEBToTable(xdata::Table &newRows, DAQMB * TStore_thisDAQMB);
  void copyDAQMBToTable(xdata::Table &newRows, DAQMB * TStore_thisDAQMB);
  void copyCSCToTable(xdata::Table &newRows, Chamber * chamber);
  void copyMPCToTable(xdata::Table &newRows, Crate * TStore_thisCrate);
  void copyCCBToTable(xdata::Table &newRows, Crate * TStore_thisCrate);
  void copyVMECCToTable(xdata::Table &newRows, Crate * TStore_thisCrate);

  //bool moreThanOneChildConfigurationExists(const std::string &configName, const std::string &parentIdentifier);
  std::string crateForChamber(const std::string &chamberName) throw (xdaq::exception::Exception);
  std::string keyContaining(std::map<std::string, xdata::Table> &haystack, const std::string &needle)
      throw (xdaq::exception::Exception);
  xdata::Table &valueForKeyContaining(std::map<std::string, xdata::Table> &haystack, const std::string &needle)
      throw (xdaq::exception::Exception);

  std::string testSummary_;
  EmuEndcap * TStore_myEndcap_;
  std::map<std::string, xdata::Table> currentTestSummary;
  std::map<std::string, signed int> TTCrxIDs;

  std::string config_type_;
  std::string config_desc_;

  xdata::String tstoreViewXML_;
};

} // namespace emu::pc
} // namespace emu


#endif
