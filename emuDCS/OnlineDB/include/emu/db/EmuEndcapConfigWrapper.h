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

namespace emu { namespace db {

/** @class EmuEndcapConfigWrapper
 * Wrapper for creating configured EmuEndcap from configuration data in ConfigTree
 **/
class EmuEndcapConfigWrapper
{
public:

  /// constructor
  EmuEndcapConfigWrapper(ConfigTree *tree);

  /// the main procedure that return configured EmuEndcap
  emu::pc::EmuEndcap * getConfiguredEndcap(bool verbose=false) throw (emu::exception::ConfigurationException);

  /// data mapper functions for different devices
  void readPeripheralCrate(ConfigRow *conf, emu::pc::EmuEndcap * endcap) throw (emu::exception::ConfigurationException);
  void readVCC(ConfigRow *conf, emu::pc::Crate * theCrate) throw (emu::exception::ConfigurationException);
  void readCCB(ConfigRow *conf, emu::pc::Crate * theCrate) throw (emu::exception::ConfigurationException);
  void readMPC(ConfigRow *conf, emu::pc::Crate * theCrate) throw (emu::exception::ConfigurationException);
  void readCSC(ConfigRow *conf, emu::pc::Crate * theCrate) throw (emu::exception::ConfigurationException);
  void readDAQMB(ConfigRow *conf, emu::pc::Crate * theCrate, emu::pc::Chamber * theChamber) throw (emu::exception::ConfigurationException);
  void readCFEB(ConfigRow *conf, emu::pc::DAQMB * theDaqmb) throw (emu::exception::ConfigurationException);
  void readTMB(ConfigRow *conf, emu::pc::Crate * theCrate, emu::pc::Chamber * theChamber) throw (emu::exception::ConfigurationException);
  void readALCT(ConfigRow *conf, emu::pc::TMB * theTmb) throw (emu::exception::ConfigurationException);
  void readAnodeChannel(ConfigRow *conf, emu::pc::ALCTController * theAlct) throw (emu::exception::ConfigurationException);

  //  void getConfigIds(std::vector<std::string> &configIDs,const std::string endcap_side,int max_ids) throw (emu::exception::ConfigurationException);
  //  std::string getLastConfigIdUsed(const std::string &endcap_side) throw (emu::exception::ConfigurationException);

  //  void getDefinition(const std::string &insertViewName, xdata::Table &results) throw (emu::exception::ConfigurationException);
  //  void insert(const std::string &insertViewName, xdata::Table &newRows) throw (emu::exception::ConfigurationException);
  //  void recordFlashWrite(const std::string &configID) throw (emu::exception::ConfigurationException);
  //  int readFlashList(std::vector<std::string> &configKeys, std::vector<std::string> &configTimes, const std::string &endcap_side) throw (emu::exception::ConfigurationException);

private:

  std::string getString(ConfigRow *conf, std::string columnName) throw (emu::exception::ConfigurationException);
  int getInt(ConfigRow *conf, std::string columnName) throw (emu::exception::ConfigurationException);
  float getFloat(ConfigRow *conf, std::string columnName) throw (emu::exception::ConfigurationException);
  int getHexStringAsInt(ConfigRow *conf, std::string columnName) throw (emu::exception::ConfigurationException);
  long int getStringAsLongInt(ConfigRow *conf, std::string columnName) throw (emu::exception::ConfigurationException);
  long long int getStringAsLongLongInt(ConfigRow *conf, std::string columnName) throw (emu::exception::ConfigurationException);

  bool verbose_;
  
  ConfigTree *tree_;
};


}} // namespaces

#endif
