#ifndef __EMU_DB_EMUENDCAPCONFIGWRAPPER_H__
#define __EMU_DB_EMUENDCAPCONFIGWRAPPER_H__

#include "emu/db/ConfigTree.h"

namespace emu { namespace pc {
class CCB;
class MPC;
class VMEController;
class TMB;
class ALCTController;
class RAT;
class DAQMB;
class CFEB;
class Chamber;
class Crate;
class EmuEndcap;
class VMECC;
}}

namespace emu { namespace pc {

/** @class EmuEndcapConfigWrapper
 * Wrapper for creating configured EmuEndcap from configuration data in ConfigTree
 **/
class EmuEndcapConfigWrapper
{
public:

  /// constructor
  EmuEndcapConfigWrapper(emu::db::ConfigTree *tree);

  /// the main procedure that return configured EmuEndcap
  EmuEndcap * getConfiguredEndcap(bool verbose=false) throw (emu::exception::ConfigurationException);

  /// data mapper functions for different devices
  void readPeripheralCrate(emu::db::ConfigRow *conf, EmuEndcap * endcap) throw (emu::exception::ConfigurationException);
  void readVCC(emu::db::ConfigRow *conf, Crate * theCrate) throw (emu::exception::ConfigurationException);
  void readCCB(emu::db::ConfigRow *conf, Crate * theCrate) throw (emu::exception::ConfigurationException);
  void readMPC(emu::db::ConfigRow *conf, Crate * theCrate) throw (emu::exception::ConfigurationException);
  void readCSC(emu::db::ConfigRow *conf, Crate * theCrate) throw (emu::exception::ConfigurationException);
  void readDAQMB(emu::db::ConfigRow *conf, Crate * theCrate, Chamber * theChamber) throw (emu::exception::ConfigurationException);
  void readCFEB(emu::db::ConfigRow *conf, DAQMB * theDaqmb) throw (emu::exception::ConfigurationException);
  void readTMB(emu::db::ConfigRow *conf, Crate * theCrate, Chamber * theChamber) throw (emu::exception::ConfigurationException);
  void readALCT(emu::db::ConfigRow *conf, TMB * theTmb) throw (emu::exception::ConfigurationException);
  void readAnodeChannel(emu::db::ConfigRow *conf, ALCTController * theAlct) throw (emu::exception::ConfigurationException);

  //  void getConfigIds(std::vector<std::string> &configIDs,const std::string endcap_side,int max_ids) throw (emu::exception::ConfigurationException);
  //  std::string getLastConfigIdUsed(const std::string &endcap_side) throw (emu::exception::ConfigurationException);

  //  void getDefinition(const std::string &insertViewName, xdata::Table &results) throw (emu::exception::ConfigurationException);
  //  void insert(const std::string &insertViewName, xdata::Table &newRows) throw (emu::exception::ConfigurationException);
  //  void recordFlashWrite(const std::string &configID) throw (emu::exception::ConfigurationException);
  //  int readFlashList(std::vector<std::string> &configKeys, std::vector<std::string> &configTimes, const std::string &endcap_side) throw (emu::exception::ConfigurationException);

private:

  std::string getString(emu::db::ConfigRow *conf, std::string columnName) throw (emu::exception::ConfigurationException);
  int getInt(emu::db::ConfigRow *conf, std::string columnName) throw (emu::exception::ConfigurationException);
  float getFloat(emu::db::ConfigRow *conf, std::string columnName) throw (emu::exception::ConfigurationException);
  int getHexStringAsInt(emu::db::ConfigRow *conf, std::string columnName) throw (emu::exception::ConfigurationException);
  long int getStringAsLongInt(emu::db::ConfigRow *conf, std::string columnName) throw (emu::exception::ConfigurationException);
  long long int getStringAsLongLongInt(emu::db::ConfigRow *conf, std::string columnName) throw (emu::exception::ConfigurationException);

  bool verbose_;
  
  emu::db::ConfigTree *tree_;
};


}} // namespaces

#endif
