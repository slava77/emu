// $Id: EmuEndcapConfigWrapper.cc,v 1.3 2012/02/20 12:41:47 liu Exp $

#include "emu/pc/EmuEndcapConfigWrapper.h"
#include "emu/utils/SimpleTimer.h"

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

#include "xdata/UnsignedShort.h"
#include "xdata/Float.h"
#include "xdata/UnsignedInteger64.h"
#include "xdata/String.h"


namespace emu { namespace pc {


EmuEndcapConfigWrapper::EmuEndcapConfigWrapper(emu::db::ConfigTree *tree)
: verbose_(0)
, tree_(tree)
{}


EmuEndcap * EmuEndcapConfigWrapper::getConfiguredEndcap(bool verbosity)
throw (emu::exception::ConfigurationException)
{
  emu::utils::SimpleTimer timer;

  verbose_ = verbosity;

  EmuEndcap * endcap = new EmuEndcap();
  std::cout << "######## Empty EmuEndcap is created." << std::endl;

  std::vector<emu::db::ConfigRow *> pcrates = tree_->head()->children();
  for (std::vector<emu::db::ConfigRow *>::iterator icrate = pcrates.begin(); icrate != pcrates.end(); icrate++)
  {
    if ((*icrate)->type() == "PeripheralCrate")
    {
      readPeripheralCrate(*icrate, endcap);
    }
    else
    {
      XCEPT_RAISE(emu::exception::ConfigurationException, "Expected table type PeripheralCrate, got " + (*icrate)->type() );
    }
  }
  std::cout << "######## EmuEndcap is built in "<<timer.sec()<<" sec"<<std::endl;

  return endcap;
}


void EmuEndcapConfigWrapper::readPeripheralCrate(emu::db::ConfigRow *conf, EmuEndcap * endcap)
throw (emu::exception::ConfigurationException)
{
  if(verbose_) std::cout << "==== Peripheral Crate ====" << std::endl;

  int crateid = getInt(conf, "crateID");
  std::string label = getString(conf, "label");

  if(verbose_) std::cout <<"  crateID:"<<crateid<< " label: "<< label<< std::endl;
  
  VMEController * controller = new VMEController();
  Crate * crate = new Crate(crateid,controller);
  crate->SetLabel(label);
  endcap->addCrate(crate);

  std::vector<emu::db::ConfigRow *> kids = conf->children();
  for (std::vector<emu::db::ConfigRow *>::iterator kid = kids.begin(); kid != kids.end(); kid++)
  {
    std::string type = (*kid)->type();
    if (type == "VCC")
    {
      readVCC(*kid, crate);
    }
    else if (type == "CCB")
    {
      readCCB(*kid, crate);
    }
    else if (type == "MPC")
    {
      readMPC(*kid, crate);
    }
    else if (type == "CSC")
    {
      readCSC(*kid, crate);
    }
    else
    {
      XCEPT_RAISE(emu::exception::ConfigurationException, "Unexpected child table type for PeripheralCrate: " + type );
    }
  }
  if(verbose_) std::cout << "#### Peripheral Crate added ####"<< std::endl;
}


void EmuEndcapConfigWrapper::readVCC(emu::db::ConfigRow *conf, Crate * theCrate)
throw (emu::exception::ConfigurationException)
{
  if(verbose_) std::cout << "==== VCC ====" << std::endl;

  // add VMECC(VMEModule in slot 1) to crate
  VMECC * vcc = new VMECC(theCrate, 1);
  VMEController * vmec = theCrate->vmeController();

  std::string mac = getString(conf, "MAC_addr");
  vmec->SetMAC(0,mac);
  size_t pos = mac.find('-');
  while(pos != std::string::npos){
    mac.replace(pos, 1, 1, ':');
    pos = mac.find('-');
  }
  vmec->SetVMEAddress(mac);

  if (conf->has("VCC_frmw_ver")) vcc->SetVCC_frmw_ver( getString(conf, "VCC_frmw_ver"));
  if (conf->has("eth_port"))     vmec->SetPort( getInt(conf, "eth_port"));
  if (conf->has("MCAST_1"))      vmec->SetMAC(1, getString(conf, "MCAST_1"));
  if (conf->has("MCAST_2"))      vmec->SetMAC(2, getString(conf, "MCAST_2"));
  if (conf->has("MCAST_3"))      vmec->SetMAC(3, getString(conf, "MCAST_3"));
  if (conf->has("Dflt_Srv_MAC")) vmec->SetMAC(4, getString(conf, "Dflt_Srv_MAC"));
  if (conf->has("Ethernet_CR"))  vmec->SetCR(0, getString(conf, "Ethernet_CR"));
  if (conf->has("Ext_FIFO_CR"))  vmec->SetCR(1, getString(conf, "Ext_FIFO_CR"));
  if (conf->has("Rst_Misc_CR"))  vmec->SetCR(2, getString(conf, "Rst_Misc_CR"));
  if (conf->has("VME_CR"))       vmec->SetCR(3, getString(conf, "VME_CR"));
  if (conf->has("BTO"))          vmec->SetCR(4, getString(conf, "BTO"));
  if (conf->has("BGTO"))         vmec->SetCR(5, getString(conf, "BGTO"));
  if (conf->has("Msg_Lvl"))      vmec->SetMsg_Lvl( getInt(conf, "Msg_Lvl"));

  if(conf->has("Warn_On_Shtdwn") && getString(conf, "Warn_On_Shtdwn") == "true"){
    vmec->SetWarn_On_Shtdwn(true);
  }else{
    vmec->SetWarn_On_Shtdwn(false);
  }
  if(conf->has("Pkt_On_Startup") && getString(conf, "Pkt_On_Startup") == "true"){
    vmec->SetPkt_On_Startup(true);
  }else{
    vmec->SetPkt_On_Startup(false);
  }

  if(verbose_)
  {
    xdata::Table t = conf->row();
    t.writeTo(std::cout); std::cout<< std::endl;
    std::cout << "#### VCC added ####"<< std::endl;
  }
}


void EmuEndcapConfigWrapper::readCSC(emu::db::ConfigRow *conf, Crate * theCrate)
throw (emu::exception::ConfigurationException)
{
  if(verbose_) std::cout << "==== CSC chamber ====" << std::endl;

  Chamber * csc_ = new Chamber(theCrate);

  if (conf->has("LABEL"))         csc_->SetLabel( getString(conf, "LABEL"));
  if (conf->has("KNOWN_PROBLEM")) csc_->SetProblemDescription( getString(conf, "KNOWN_PROBLEM"));
  if (conf->has("PROBLEM_MASK"))  csc_->SetProblemMask( getInt(conf, "PROBLEM_MASK"));

  if(verbose_)
  {
    xdata::Table t = conf->row();
    t.writeTo(std::cout); std::cout<< std::endl;
  }

  std::vector<emu::db::ConfigRow *> kids = conf->children();
  for (std::vector<emu::db::ConfigRow *>::iterator kid = kids.begin(); kid != kids.end(); kid++)
  {
    std::string type = (*kid)->type();
    if (type == "DAQMB")
    {
      readDAQMB(*kid, theCrate, csc_);
    }
    else if (type == "TMB")
    {
      readTMB(*kid, theCrate, csc_);
    }
    else
    {
      XCEPT_RAISE(emu::exception::ConfigurationException, "Unexpected child table type for CSC: " + type );
    }
  }

  if(verbose_) std::cout << "#### CSC chamber added ####"<< std::endl;
}


void EmuEndcapConfigWrapper::readCCB(emu::db::ConfigRow *conf, Crate * theCrate)
throw (emu::exception::ConfigurationException)
{
  if(verbose_) std::cout << "==== CCB ====" << std::endl;

  // add CCB(slot 13) to crate
  CCB * ccb_ = new CCB(theCrate, 13);

  if (conf->has("CCBmode"))            ccb_->SetCCBmode( getInt(conf, "CCBmode"));
  if (conf->has("l1aDelay"))           ccb_->Setl1adelay( getInt(conf, "l1aDelay"));
  if (conf->has("TTCrxID"))            ccb_->SetTTCrxID( getInt(conf, "TTCrxID"));
  if (conf->has("TTCrxCoarseDelay"))   ccb_->SetTTCrxCoarseDelay( getInt(conf, "TTCrxCoarseDelay"));
  if (conf->has("TTCrxFineDelay"))     ccb_->SetTTCrxFineDelay( getInt(conf, "TTCrxFineDelay"));
  if (conf->has("ccb_firmware_year"))  ccb_->SetExpectedFirmwareYear( getInt(conf, "ccb_firmware_year"));
  if (conf->has("ccb_firmware_month")) ccb_->SetExpectedFirmwareMonth( getInt(conf, "ccb_firmware_month"));
  if (conf->has("ccb_firmware_day"))   ccb_->SetExpectedFirmwareDay( getInt(conf, "ccb_firmware_day"));
  
  if(verbose_)
  {
    xdata::Table t = conf->row();
    t.writeTo(std::cout); std::cout<< std::endl;
    std::cout << "#### CCB added ####"<< std::endl;
  }
}


void EmuEndcapConfigWrapper::readMPC(emu::db::ConfigRow *conf, Crate * theCrate)
throw (emu::exception::ConfigurationException)
{
  if(verbose_) std::cout << "==== MPC ====" << std::endl;

  // add MPC(slot 12) to crate
  MPC * mpc_ = new MPC(theCrate, 12);
  mpc_->SetBoardID(theCrate->CrateID());

  if (conf->has("serializermode"))     mpc_->SetTLK2501TxMode( getInt(conf, "serializermode"));
  if (conf->has("TransparentMode"))    mpc_->SetTransparentMode( getInt(conf, "TransparentMode"));
  if (conf->has("mpc_firmware_year"))  mpc_->SetExpectedFirmwareYear( getInt(conf, "mpc_firmware_year"));
  if (conf->has("mpc_firmware_month")) mpc_->SetExpectedFirmwareMonth( getInt(conf, "mpc_firmware_month"));
  if (conf->has("mpc_firmware_day"))   mpc_->SetExpectedFirmwareDay( getInt(conf, "mpc_firmware_day"));

  if(verbose_)
  {
    xdata::Table t = conf->row();
    t.writeTo(std::cout); std::cout<< std::endl;
    std::cout << "#### MPC added ####"<< std::endl;
  }
}


void EmuEndcapConfigWrapper::readDAQMB(emu::db::ConfigRow *conf, Crate * theCrate, Chamber * theChamber)
throw (emu::exception::ConfigurationException)
{
  if(verbose_) std::cout << "==== DAQMB ====" << std::endl;

  int slot = getInt(conf, "SLOT");
  if (slot == 0)
  {
    XCEPT_RAISE(emu::exception::ConfigurationException, std::string("Failed to get DAQMB slot") );
  }
  DAQMB * daqmb_ = new DAQMB(theCrate, theChamber, slot);
  daqmb_->SetCrateId(theCrate->CrateID());

  if (conf->has("CALIBRATION_LCT_DELAY"))   daqmb_->SetCalibrationLctDelay( getInt(conf, "CALIBRATION_LCT_DELAY"));
  if (conf->has("CALIBRATION_L1ACC_DELAY")) daqmb_->SetCalibrationL1aDelay( getInt(conf, "CALIBRATION_L1ACC_DELAY"));
  if (conf->has("PULSE_DELAY"))             daqmb_->SetPulseDelay( getInt(conf, "PULSE_DELAY"));
  if (conf->has("INJECT_DELAY"))            daqmb_->SetInjectDelay( getInt(conf, "INJECT_DELAY"));
  if (conf->has("KILL_INPUT"))              daqmb_->SetKillInput( getInt(conf, "KILL_INPUT"));
  if (conf->has("PUL_DAC_SET"))             daqmb_->SetPulseDac( getFloat(conf, "PUL_DAC_SET"));
  if (conf->has("INJ_DAC_SET"))             daqmb_->SetInjectorDac( getFloat(conf, "INJ_DAC_SET"));
  if (conf->has("CFEB_CABLE_DELAY"))        daqmb_->SetCfebCableDelay( getInt(conf, "CFEB_CABLE_DELAY"));
  if (conf->has("TMB_LCT_CABLE_DELAY"))     daqmb_->SetTmbLctCableDelay( getInt(conf, "TMB_LCT_CABLE_DELAY"));
  if (conf->has("CFEB_DAV_CABLE_DELAY"))    daqmb_->SetCfebDavCableDelay( getInt(conf, "CFEB_DAV_CABLE_DELAY"));
  if (conf->has("ALCT_DAV_CABLE_DELAY"))    daqmb_->SetAlctDavCableDelay( getInt(conf, "ALCT_DAV_CABLE_DELAY"));
  if (conf->has("FEB_CLOCK_DELAY"))         daqmb_->SetCfebClkDelay( getInt(conf, "FEB_CLOCK_DELAY"));
  if (conf->has("XFINELATENCY"))            daqmb_->SetxFineLatency( getInt(conf, "XFINELATENCY"));
  if (conf->has("DMB_CNTL_FIRMWARE_TAG"))   daqmb_->SetExpectedControlFirmwareTag( getStringAsLongInt(conf, "DMB_CNTL_FIRMWARE_TAG"));
  if (conf->has("DMB_VME_FIRMWARE_TAG"))    daqmb_->SetExpectedVMEFirmwareTag( getInt(conf, "DMB_VME_FIRMWARE_TAG"));
  if (conf->has("POWER_MASK"))              daqmb_->SetPowerMask( getInt(conf, "POWER_MASK"));

  float float_val;
  int int_val;

  if (conf->has("SET_COMP_THRESH"))
  {
    float_val = getFloat(conf, "SET_COMP_THRESH");
    daqmb_->SetCompThresh(float_val);
    for(int cfeb=0; cfeb<5; cfeb++) daqmb_->SetCompThresholdsCfeb(cfeb, float_val);
  }
  if (conf->has("COMP_MODE"))
  {
    int int_val = getInt(conf, "COMP_MODE");
    daqmb_->SetCompMode(int_val);
    for(int cfeb=0; cfeb<5; cfeb++) daqmb_->SetCompModeCfeb(cfeb, int_val);
  }
  if (conf->has("COMP_TIMING"))
  {
    int_val = getInt(conf, "COMP_TIMING");
    daqmb_->SetCompTiming(int_val);
    for(int cfeb=0; cfeb<5; cfeb++) daqmb_->SetCompTimingCfeb(cfeb, int_val);
  }
  if (conf->has("PRE_BLOCK_END"))
  {
    int_val = getInt(conf, "PRE_BLOCK_END");
    daqmb_->SetPreBlockEnd(int_val);
    for(int cfeb=0; cfeb<5; cfeb++) daqmb_->SetPreBlockEndCfeb(cfeb, int_val);
  }
  if (conf->has("XLATENCY"))
  {
    int_val = getInt(conf, "XLATENCY");
    daqmb_->SetxLatency(int_val);
    for (int cfeb=0; cfeb<5; cfeb++) daqmb_->SetL1aExtraCfeb(cfeb, int_val);
  }

  if(verbose_)
  {
    xdata::Table t = conf->row();
    t.writeTo(std::cout); std::cout<< std::endl;
  }

  std::vector<emu::db::ConfigRow *> kids = conf->children();
  for (std::vector<emu::db::ConfigRow *>::iterator kid = kids.begin(); kid != kids.end(); kid++)
  {
    std::string type = (*kid)->type();
    if (type == "CFEB")
    {
      readCFEB(*kid, daqmb_);
    }
    else
    {
      XCEPT_RAISE(emu::exception::ConfigurationException, "Unexpected child table type for DAQMB: " + type );
    }
  }

  if(verbose_) std::cout << "#### DAQMB added ####"<< std::endl;
}


void EmuEndcapConfigWrapper::readCFEB(emu::db::ConfigRow *conf, DAQMB * theDaqmb)
throw (emu::exception::ConfigurationException)
{
  if(verbose_) std::cout << "==== CFEB ====" << std::endl;

  int number = getInt(conf, "CFEB_NUMBER");
  if(verbose_) std::cout << "CFEB(" << number << ")" << std::endl;
  CFEB cfeb(number);

  theDaqmb->SetExpectedCFEBFirmwareTag(number, getStringAsLongInt(conf, "CFEB_FIRMWARE_TAG"));

  int kill_chip[6]={0x0000,0x0000,0x0000,0x0000,0x0000,0x0000};

  if (conf->has("KILL_CHIP0")) kill_chip[0] = getHexStringAsInt(conf, "KILL_CHIP0");
  if (conf->has("KILL_CHIP1")) kill_chip[1] = getHexStringAsInt(conf, "KILL_CHIP1");
  if (conf->has("KILL_CHIP2")) kill_chip[2] = getHexStringAsInt(conf, "KILL_CHIP2");
  if (conf->has("KILL_CHIP3")) kill_chip[3] = getHexStringAsInt(conf, "KILL_CHIP3");
  if (conf->has("KILL_CHIP4")) kill_chip[4] = getHexStringAsInt(conf, "KILL_CHIP4");
  if (conf->has("KILL_CHIP5")) kill_chip[5] = getHexStringAsInt(conf, "KILL_CHIP5");

  for(int chip=0;chip<6;chip++){
    theDaqmb->SetKillChip(number, chip, kill_chip[chip]);
    for(int chan=0; chan<16; chan++)
    {
      unsigned short int mask = (1<<chan);
      if((mask & kill_chip[chip])==0x0000)
      {
        theDaqmb->SetShiftArray(number, 5-chip, chan, NORM_RUN);
      }
      else
      {
        theDaqmb->SetShiftArray(number, 5-chip, chan, KILL_CHAN);
      }
    }
  }

  theDaqmb->cfebs_.push_back(cfeb);

  if(verbose_)
  {
    xdata::Table t = conf->row();
    t.writeTo(std::cout); std::cout<< std::endl;
    std::cout << "#### CFEB added ####"<< std::endl;
  }
}


void EmuEndcapConfigWrapper::readTMB(emu::db::ConfigRow *conf, Crate * theCrate, Chamber * theChamber)
throw (emu::exception::ConfigurationException)
{
  if(verbose_) std::cout << "==== DAQMB ====" << std::endl;

  int slot = getInt(conf, "SLOT");
  if (slot == 0)
  {
    XCEPT_RAISE(emu::exception::ConfigurationException, std::string("Failed to get TMB slot") );
  }
  TMB * tmb_ = new TMB(theCrate, theChamber, slot);

  if (conf->has("TMB_FIRMWARE_MONTH"))         tmb_->SetExpectedTmbFirmwareMonth( getInt(conf, "TMB_FIRMWARE_MONTH"));
  if (conf->has("TMB_FIRMWARE_DAY"))           tmb_->SetExpectedTmbFirmwareDay( getInt(conf, "TMB_FIRMWARE_DAY"));
  if (conf->has("TMB_FIRMWARE_YEAR"))          tmb_->SetExpectedTmbFirmwareYear( getInt(conf, "TMB_FIRMWARE_YEAR"));
  if (conf->has("TMB_FIRMWARE_VERSION"))       tmb_->SetExpectedTmbFirmwareVersion( getInt(conf, "TMB_FIRMWARE_VERSION"));
  if (conf->has("TMB_FIRMWARE_REVCODE"))       tmb_->SetExpectedTmbFirmwareRevcode( getInt(conf, "TMB_FIRMWARE_REVCODE"));
  if (conf->has("TMB_FIRMWARE_TYPE"))          tmb_->SetExpectedTmbFirmwareType( getInt(conf, "TMB_FIRMWARE_TYPE"));
  if (conf->has("TMB_FIRMWARE_COMPILE_TYPE"))  tmb_->SetTMBFirmwareCompileType( getHexStringAsInt(conf, "TMB_FIRMWARE_COMPILE_TYPE"));
  if (conf->has("RAT_FIRMWARE_MONTH"))         tmb_->SetExpectedRatFirmwareMonth( getInt(conf, "RAT_FIRMWARE_MONTH"));
  if (conf->has("RAT_FIRMWARE_DAY"))           tmb_->SetExpectedRatFirmwareDay( getInt(conf, "RAT_FIRMWARE_DAY"));
  if (conf->has("RAT_FIRMWARE_YEAR"))          tmb_->SetExpectedRatFirmwareYear( getInt(conf, "RAT_FIRMWARE_YEAR"));
  if (conf->has("ENABLE_ALCT_TX"))             tmb_->SetEnableAlctTx( getInt(conf, "ENABLE_ALCT_TX"));
  if (conf->has("ENABLE_ALCT_RX"))             tmb_->SetAlctInput( getInt(conf, "ENABLE_ALCT_RX"));
  if (conf->has("ALCT_CLEAR"))                 tmb_->SetAlctClear( getInt(conf, "ALCT_CLEAR"));
  if (conf->has("ENABLECLCTINPUTS_REG42"))     tmb_->SetEnableCLCTInputs( getInt(conf, "ENABLECLCTINPUTS_REG42"));
  if (conf->has("RPC_EXISTS"))                 tmb_->SetRpcExist( getInt(conf, "RPC_EXISTS"));
  if (conf->has("RPC_READ_ENABLE"))            tmb_->SetRpcReadEnable( getInt(conf, "RPC_READ_ENABLE"));
  if (conf->has("RPC_BXN_OFFSET"))             tmb_->SetRpcBxnOffset( getInt(conf, "RPC_BXN_OFFSET"));
  if (conf->has("RPC_MASK_ALL"))               tmb_->SetEnableRpcInput( getInt(conf, "RPC_MASK_ALL"));
  if (conf->has("MATCH_PRETRIG_WINDOW_SIZE"))  tmb_->SetAlctClctPretrigWidth( getInt(conf, "MATCH_PRETRIG_WINDOW_SIZE"));
  if (conf->has("MATCH_PRETRIG_ALCT_DELAY"))   tmb_->SetAlctPretrigDelay( getInt(conf, "MATCH_PRETRIG_ALCT_DELAY"));
  //if (conf->has("LAYER_TRIG_DELAY"))         tmb_->SetLayerTrigDelay( getInt(conf, "LAYER_TRIG_DELAY")); // obsolete
  if (conf->has("TMB_L1A_DELAY"))              tmb_->SetL1aDelay( getInt(conf, "TMB_L1A_DELAY"));
  if (conf->has("TMB_L1A_WINDOW_SIZE"))        tmb_->SetL1aWindowSize( getInt(conf, "TMB_L1A_WINDOW_SIZE"));
  if (conf->has("TMB_L1A_OFFSET"))             tmb_->SetL1aOffset( getInt(conf, "TMB_L1A_OFFSET"));
  if (conf->has("TMB_BXN_OFFSET"))             tmb_->SetBxnOffset( getInt(conf, "TMB_BXN_OFFSET"));
  if (conf->has("MATCH_TRIG_ALCT_DELAY"))      tmb_->SetAlctVpfDelay( getInt(conf, "MATCH_TRIG_ALCT_DELAY"));
  if (conf->has("MATCH_TRIG_WINDOW_SIZE"))     tmb_->SetAlctMatchWindowSize( getInt(conf, "MATCH_TRIG_WINDOW_SIZE"));
  if (conf->has("MPC_TX_DELAY"))               tmb_->SetMpcTxDelay( getInt(conf, "MPC_TX_DELAY"));
  if (conf->has("RPC0_RAW_DELAY"))             tmb_->SetRpc0RawDelay( getInt(conf, "RPC0_RAW_DELAY"));
  if (conf->has("RPC1_RAW_DELAY"))             tmb_->SetRpc1RawDelay( getInt(conf, "RPC1_RAW_DELAY"));
  if (conf->has("CLCT_PRETRIG_ENABLE"))        tmb_->SetClctPatternTrigEnable( getInt(conf, "CLCT_PRETRIG_ENABLE"));
  if (conf->has("ALCT_PRETRIG_ENABLE"))        tmb_->SetAlctPatternTrigEnable( getInt(conf, "ALCT_PRETRIG_ENABLE"));
  if (conf->has("MATCH_PRETRIG_ENABLE"))       tmb_->SetMatchPatternTrigEnable( getInt(conf, "MATCH_PRETRIG_ENABLE"));
  if (conf->has("CLCT_EXT_PRETRIG_ENABLE"))    tmb_->SetClctExtTrigEnable( getInt(conf, "CLCT_EXT_PRETRIG_ENABLE"));
  if (conf->has("ALL_CFEB_ACTIVE"))            tmb_->SetEnableAllCfebsActive( getInt(conf, "ALL_CFEB_ACTIVE"));
  if (conf->has("ENABLECLCTINPUTS_REG68"))     tmb_->SetCfebEnable( getInt(conf, "ENABLECLCTINPUTS_REG68"));
  if (conf->has("CFEB_ENABLE_SOURCE"))         tmb_->Set_cfeb_enable_source( getInt(conf, "CFEB_ENABLE_SOURCE"));
  if (conf->has("CLCT_HIT_PERSIST"))           tmb_->SetTriadPersistence( getInt(conf, "CLCT_HIT_PERSIST"));
  if (conf->has("CLCT_NPLANES_HIT_PRETRIG"))   tmb_->SetHsPretrigThresh( getInt(conf, "CLCT_NPLANES_HIT_PRETRIG"));
  if (conf->has("CLCT_NPLANES_HIT_PATTERN"))   tmb_->SetMinHitsPattern( getInt(conf, "CLCT_NPLANES_HIT_PATTERN"));
  if (conf->has("CLCT_DRIFT_DELAY"))           tmb_->SetDriftDelay( getInt(conf, "CLCT_DRIFT_DELAY"));
  if (conf->has("TMB_FIFO_MODE"))              tmb_->SetFifoMode( getInt(conf, "TMB_FIFO_MODE"));
  if (conf->has("CLCT_FIFO_TBINS"))            tmb_->SetFifoTbins( getInt(conf, "CLCT_FIFO_TBINS"));
  if (conf->has("CLCT_FIFO_PRETRIG"))          tmb_->SetFifoPreTrig( getInt(conf, "CLCT_FIFO_PRETRIG"));
  if (conf->has("MPC_SYNC_ERR_ENABLE"))        tmb_->SetTmbSyncErrEnable( getInt(conf, "MPC_SYNC_ERR_ENABLE"));
  if (conf->has("ALCT_TRIG_ENABLE"))           tmb_->SetTmbAllowAlct( getInt(conf, "ALCT_TRIG_ENABLE"));
  if (conf->has("CLCT_TRIG_ENABLE"))           tmb_->SetTmbAllowClct( getInt(conf, "CLCT_TRIG_ENABLE"));
  if (conf->has("MATCH_TRIG_ENABLE"))          tmb_->SetTmbAllowMatch( getInt(conf, "MATCH_TRIG_ENABLE"));
  if (conf->has("ALCT_READOUT_WITHOUT_TRIG"))  tmb_->SetAllowAlctNontrigReadout( getInt(conf, "ALCT_READOUT_WITHOUT_TRIG"));
  if (conf->has("CLCT_READOUT_WITHOUT_TRIG"))  tmb_->SetAllowClctNontrigReadout( getInt(conf, "CLCT_READOUT_WITHOUT_TRIG"));
  if (conf->has("MATCH_READOUT_WITHOUT_TRIG")) tmb_->SetAllowMatchNontrigReadout( getInt(conf, "MATCH_READOUT_WITHOUT_TRIG"));
  if (conf->has("MPC_RX_DELAY"))               tmb_->SetMpcRxDelay( getInt(conf, "MPC_RX_DELAY"));
  if (conf->has("MPC_IDLE_BLANK"))             tmb_->SetMpcIdleBlank( getInt(conf, "MPC_IDLE_BLANK"));
  if (conf->has("MPC_OUTPUT_ENABLE"))          tmb_->SetMpcOutputEnable( getInt(conf, "MPC_OUTPUT_ENABLE"));
  if (conf->has("MPC_BLOCK_ME1A"))             tmb_->SetBlockME1aToMPC( getInt(conf, "MPC_BLOCK_ME1A"));
  if (conf->has("WRITE_BUFFER_REQUIRED"))      tmb_->SetWriteBufferRequired( getInt(conf, "WRITE_BUFFER_REQUIRED"));
  if (conf->has("VALID_CLCT_REQUIRED"))        tmb_->SetRequireValidClct( getInt(conf, "VALID_CLCT_REQUIRED"));
  if (conf->has("L1A_ALLOW_MATCH"))            tmb_->SetL1aAllowMatch( getInt(conf, "L1A_ALLOW_MATCH"));
  if (conf->has("L1A_ALLOW_NOTMB"))            tmb_->SetL1aAllowNoTmb( getInt(conf, "L1A_ALLOW_NOTMB"));
  if (conf->has("L1A_ALLOW_NOL1A"))            tmb_->SetL1aAllowNoL1a( getInt(conf, "L1A_ALLOW_NOL1A"));
  if (conf->has("L1A_ALLOW_ALCT_ONLY"))        tmb_->SetL1aAllowAlctOnly( getInt(conf, "L1A_ALLOW_ALCT_ONLY"));
  if (conf->has("LAYER_TRIG_ENABLE"))          tmb_->SetEnableLayerTrigger( getInt(conf, "LAYER_TRIG_ENABLE"));
  if (conf->has("LAYER_TRIG_THRESH"))          tmb_->SetLayerTriggerThreshold( getInt(conf, "LAYER_TRIG_THRESH"));
  if (conf->has("CLCT_BLANKING"))              tmb_->SetClctBlanking( getInt(conf, "CLCT_BLANKING"));
  //if (conf->has("CLCT_STAGGER"))             tmb_->SetClctStagger( getInt(conf, "CLCT_STAGGER"));               }  // obsolete
  if (conf->has("CLCT_PID_THRESH_PRETRIG"))    tmb_->SetClctPatternIdThresh( getInt(conf, "CLCT_PID_THRESH_PRETRIG"));
  if (conf->has("AFF_THRESH"))                 tmb_->SetActiveFebFlagThresh( getInt(conf, "AFF_THRESH"));
  if (conf->has("CLCT_MIN_SEPARATION"))        tmb_->SetMinClctSeparation( getInt(conf, "CLCT_MIN_SEPARATION"));

  if (conf->has("LAYER0_DISTRIP_HOT_CHANN_MASK")) tmb_->SetDistripHotChannelMask(0, getStringAsLongLongInt(conf, "LAYER0_DISTRIP_HOT_CHANN_MASK"));
  if (conf->has("LAYER1_DISTRIP_HOT_CHANN_MASK")) tmb_->SetDistripHotChannelMask(1, getStringAsLongLongInt(conf, "LAYER1_DISTRIP_HOT_CHANN_MASK"));
  if (conf->has("LAYER2_DISTRIP_HOT_CHANN_MASK")) tmb_->SetDistripHotChannelMask(2, getStringAsLongLongInt(conf, "LAYER2_DISTRIP_HOT_CHANN_MASK"));
  if (conf->has("LAYER3_DISTRIP_HOT_CHANN_MASK")) tmb_->SetDistripHotChannelMask(3, getStringAsLongLongInt(conf, "LAYER3_DISTRIP_HOT_CHANN_MASK"));
  if (conf->has("LAYER4_DISTRIP_HOT_CHANN_MASK")) tmb_->SetDistripHotChannelMask(4, getStringAsLongLongInt(conf, "LAYER4_DISTRIP_HOT_CHANN_MASK"));
  if (conf->has("LAYER5_DISTRIP_HOT_CHANN_MASK")) tmb_->SetDistripHotChannelMask(5, getStringAsLongLongInt(conf, "LAYER5_DISTRIP_HOT_CHANN_MASK"));

  if (conf->has("REQUEST_L1A"))            tmb_->SetRequestL1a( getInt(conf, "REQUEST_L1A"));
  if (conf->has("IGNORE_CCB_STARTSTOP"))   tmb_->SetIgnoreCcbStartStop( getInt(conf, "IGNORE_CCB_STARTSTOP"));
  if (conf->has("ALCT_CLOCK_EN_USE_CCB"))  tmb_->SetEnableAlctUseCcbClock( getInt(conf, "ALCT_CLOCK_EN_USE_CCB"));
  if (conf->has("ALCT_POSNEG"))            tmb_->SetAlctPosNeg( getInt(conf, "ALCT_POSNEG"));
  if (conf->has("ALCT_TX_CLOCK_DELAY"))    tmb_->SetAlctTXclockDelay( getInt(conf, "ALCT_TX_CLOCK_DELAY"));
  if (conf->has("ALCT_RX_CLOCK_DELAY"))    tmb_->SetAlctRXclockDelay( getInt(conf, "ALCT_RX_CLOCK_DELAY"));
  if (conf->has("DMB_TX_DELAY"))           tmb_->SetDmbTxDelay( getInt(conf, "DMB_TX_DELAY"));
  if (conf->has("RAT_TMB_DELAY"))          tmb_->SetRatTmbDelay( getInt(conf, "RAT_TMB_DELAY"));
  if (conf->has("CFEB0DELAY"))             tmb_->SetCFEB0delay( getInt(conf, "CFEB0DELAY"));
  if (conf->has("CFEB1DELAY"))             tmb_->SetCFEB1delay( getInt(conf, "CFEB1DELAY"));
  if (conf->has("CFEB2DELAY"))             tmb_->SetCFEB2delay( getInt(conf, "CFEB2DELAY"));
  if (conf->has("CFEB3DELAY"))             tmb_->SetCFEB3delay( getInt(conf, "CFEB3DELAY"));
  if (conf->has("CFEB4DELAY"))             tmb_->SetCFEB4delay( getInt(conf, "CFEB4DELAY"));
  if (conf->has("RPC0_RAT_DELAY"))         tmb_->SetRpc0RatDelay( getInt(conf, "RPC0_RAT_DELAY"));
  if (conf->has("RPC1_RAT_DELAY"))         tmb_->SetRpc1RatDelay( getInt(conf, "RPC1_RAT_DELAY"));
  if (conf->has("ADJACENT_CFEB_DISTANCE")) tmb_->SetAdjacentCfebDistance( getInt(conf, "ADJACENT_CFEB_DISTANCE"));
  if (conf->has("ALCT_BX0_DELAY"))         tmb_->SetAlctBx0Delay( getInt(conf, "ALCT_BX0_DELAY"));
  if (conf->has("ALCT_BX0_ENABLE"))        tmb_->SetAlctBx0Enable( getInt(conf, "ALCT_BX0_ENABLE"));
  if (conf->has("CLCT_BX0_DELAY"))         tmb_->SetClctBx0Delay( getInt(conf, "CLCT_BX0_DELAY"));
  if (conf->has("CLCT_THROTTLE"))          tmb_->SetClctThrottle( getInt(conf, "CLCT_THROTTLE"));
  if (conf->has("RPC_FIFO_DECOUPLE"))      tmb_->SetRpcDecoupleTbins( getInt(conf, "RPC_FIFO_DECOUPLE"));
  if (conf->has("RPC_FIFO_PRETRIG"))       tmb_->SetFifoPretrigRpc( getInt(conf, "RPC_FIFO_PRETRIG"));
  if (conf->has("RPC_FIFO_TBINS"))         tmb_->SetFifoTbinsRpc( getInt(conf, "RPC_FIFO_TBINS"));
  if (conf->has("WRITE_BUFFER_AUTOCLEAR")) tmb_->SetWriteBufferAutoclear( getInt(conf, "WRITE_BUFFER_AUTOCLEAR"));
  if (conf->has("WRITE_BUFFER_CONTINOUS_ENABLE")) tmb_->SetClctWriteContinuousEnable( getInt(conf, "WRITE_BUFFER_CONTINOUS_ENABLE"));
  if (conf->has("TMB_FIFO_NO_RAW_HITS"))   tmb_->SetFifoNoRawHits( getInt(conf, "TMB_FIFO_NO_RAW_HITS"));

  if (conf->has("ALCT_TX_POSNEG"))         tmb_->SetAlctTxPosNeg( getInt(conf, "ALCT_TX_POSNEG"));
  if (conf->has("CFEB0POSNEG"))            tmb_->SetCfeb0RxPosNeg( getInt(conf, "CFEB0POSNEG"));
  if (conf->has("CFEB1POSNEG"))            tmb_->SetCfeb1RxPosNeg( getInt(conf, "CFEB1POSNEG"));
  if (conf->has("CFEB2POSNEG"))            tmb_->SetCfeb2RxPosNeg( getInt(conf, "CFEB2POSNEG"));
  if (conf->has("CFEB3POSNEG"))            tmb_->SetCfeb3RxPosNeg( getInt(conf, "CFEB3POSNEG"));
  if (conf->has("CFEB4POSNEG"))            tmb_->SetCfeb4RxPosNeg( getInt(conf, "CFEB4POSNEG"));
  if (conf->has("MPC_SEL_TTC_BX0"))        tmb_->SetSelectMpcTtcBx0( getInt(conf, "MPC_SEL_TTC_BX0"));
  if (conf->has("ALCT_TOF_DELAY"))         tmb_->SetAlctTOFDelay( getInt(conf, "ALCT_TOF_DELAY"));
  if (conf->has("TMB_TO_ALCT_DATA_DELAY")) tmb_->SetALCTTxDataDelay( getInt(conf, "TMB_TO_ALCT_DATA_DELAY"));
  if (conf->has("CFEB_TOF_DELAY"))         tmb_->SetCfebTOFDelay( getInt(conf, "CFEB_TOF_DELAY"));
  if (conf->has("CFEB0_TOF_DELAY"))        tmb_->SetCfeb0TOFDelay( getInt(conf, "CFEB0_TOF_DELAY"));
  if (conf->has("CFEB1_TOF_DELAY"))        tmb_->SetCfeb1TOFDelay( getInt(conf, "CFEB1_TOF_DELAY"));
  if (conf->has("CFEB2_TOF_DELAY"))        tmb_->SetCfeb2TOFDelay( getInt(conf, "CFEB2_TOF_DELAY"));
  if (conf->has("CFEB3_TOF_DELAY"))        tmb_->SetCfeb3TOFDelay( getInt(conf, "CFEB3_TOF_DELAY"));
  if (conf->has("CFEB4_TOF_DELAY"))        tmb_->SetCfeb4TOFDelay( getInt(conf, "CFEB4_TOF_DELAY"));
  if (conf->has("CFEB_BADBITS_BLOCK"))     tmb_->SetCFEBBadBitsBlock( getInt(conf, "CFEB_BADBITS_BLOCK"));
  if (conf->has("CFEB0_RXD_INT_DELAY"))    tmb_->SetCFEB0RxdIntDelay( getInt(conf, "CFEB0_RXD_INT_DELAY"));
  if (conf->has("CFEB1_RXD_INT_DELAY"))    tmb_->SetCFEB1RxdIntDelay( getInt(conf, "CFEB1_RXD_INT_DELAY"));
  if (conf->has("CFEB2_RXD_INT_DELAY"))    tmb_->SetCFEB2RxdIntDelay( getInt(conf, "CFEB2_RXD_INT_DELAY"));
  if (conf->has("CFEB3_RXD_INT_DELAY"))    tmb_->SetCFEB3RxdIntDelay( getInt(conf, "CFEB3_RXD_INT_DELAY"));
  if (conf->has("CFEB4_RXD_INT_DELAY"))    tmb_->SetCFEB4RxdIntDelay( getInt(conf, "CFEB4_RXD_INT_DELAY"));
  if (conf->has("CFEB_BADBITS_READOUT"))   tmb_->SetCFEBBadBitsReadout( getInt(conf, "CFEB_BADBITS_READOUT"));
  if (conf->has("L1A_PRIORITY_ENABLE"))    tmb_->SetL1APriorityEnable( getInt(conf, "L1A_PRIORITY_ENABLE"));
  if (conf->has("MINISCOPE_ENABLE"))       tmb_->SetMiniscopeEnable( getInt(conf, "MINISCOPE_ENABLE"));

  if(verbose_)
  {
    xdata::Table t = conf->row();
    t.writeTo(std::cout); std::cout<< std::endl;
  }

  std::vector<emu::db::ConfigRow *> kids = conf->children();
  for (std::vector<emu::db::ConfigRow *>::iterator kid = kids.begin(); kid != kids.end(); kid++)
  {
    std::string type = (*kid)->type();
    if (type == "ALCT")
    {
      readALCT(*kid, tmb_);
    }
    else
    {
      XCEPT_RAISE(emu::exception::ConfigurationException, "Unexpected child table type for TMB: " + type );
    }
  }

  if(verbose_) std::cout << "#### TMB added ####"<< std::endl;
}


void EmuEndcapConfigWrapper::readALCT(emu::db::ConfigRow *conf, TMB * theTmb)
throw (emu::exception::ConfigurationException)
{
  if(verbose_) std::cout << "==== ALCT ====" << std::endl;

  std::string chamberType = getString(conf, "CHAMBER_TYPE");

  ALCTController * alct_ = new ALCTController(theTmb, chamberType);
  RAT * rat_ = new RAT(theTmb);
  theTmb->SetAlct(alct_); //store alct_ pointer in theTmb
  theTmb->SetRat(rat_); //store rat_  in theTmb

  if (conf->has("ALCT_FIRMWARE_DAY"))              alct_->SetExpectedFastControlDay( getInt(conf, "ALCT_FIRMWARE_DAY"));
  if (conf->has("ALCT_FIRMWARE_MONTH"))            alct_->SetExpectedFastControlMonth( getInt(conf, "ALCT_FIRMWARE_MONTH"));
  if (conf->has("ALCT_FIRMWARE_YEAR"))             alct_->SetExpectedFastControlYear( getInt(conf, "ALCT_FIRMWARE_YEAR"));
  if (conf->has("ALCT_FIRMWARE_BACKWARD_FORWARD")) alct_->Set_fastcontrol_backward_forward_type( getString(conf, "ALCT_FIRMWARE_BACKWARD_FORWARD"));
  if (conf->has("ALCT_FIRMWARE_NEGAT_POSIT"))      alct_->Set_fastcontrol_negative_positive_type( getString(conf, "ALCT_FIRMWARE_NEGAT_POSIT"));
  if (conf->has("ALCT_TRIG_MODE"))                 alct_->SetTriggerMode( getInt(conf, "ALCT_TRIG_MODE"));
  if (conf->has("ALCT_EXT_TRIG_ENABLE"))           alct_->SetExtTrigEnable( getInt(conf, "ALCT_EXT_TRIG_ENABLE"));
  if (conf->has("ALCT_SEND_EMPTY"))                alct_->SetSendEmpty( getInt(conf, "ALCT_SEND_EMPTY"));
  if (conf->has("ALCT_INJECT_MODE"))               alct_->SetInjectMode( getInt(conf, "ALCT_INJECT_MODE"));
  if (conf->has("ALCT_BXN_OFFSET"))                alct_->SetBxcOffset( getInt(conf, "ALCT_BXN_OFFSET"));
  if (conf->has("ALCT_NPLANES_HIT_ACCEL_PRETRIG")) alct_->SetAcceleratorPretrigThresh( getInt(conf, "ALCT_NPLANES_HIT_ACCEL_PRETRIG"));
  if (conf->has("ALCT_NPLANES_HIT_ACCEL_PATTERN")) alct_->SetAcceleratorPatternThresh( getInt(conf, "ALCT_NPLANES_HIT_ACCEL_PATTERN"));
  if (conf->has("ALCT_NPLANES_HIT_PATTERN"))       alct_->SetPretrigNumberOfPattern( getInt(conf, "ALCT_NPLANES_HIT_PATTERN"));
  if (conf->has("ALCT_NPLANES_HIT_PRETRIG"))       alct_->SetPretrigNumberOfLayers( getInt(conf, "ALCT_NPLANES_HIT_PRETRIG"));
  if (conf->has("ALCT_DRIFT_DELAY"))               alct_->SetDriftDelay( getInt(conf, "ALCT_DRIFT_DELAY"));
  if (conf->has("ALCT_FIFO_TBINS"))                alct_->SetFifoTbins( getInt(conf, "ALCT_FIFO_TBINS"));
  if (conf->has("ALCT_FIFO_PRETRIG"))              alct_->SetFifoPretrig( getInt(conf, "ALCT_FIFO_PRETRIG"));
  if (conf->has("ALCT_FIFO_MODE"))                 alct_->SetFifoMode( getInt(conf, "ALCT_FIFO_MODE"));
  if (conf->has("ALCT_L1A_DELAY"))                 alct_->SetL1aDelay( getInt(conf, "ALCT_L1A_DELAY"));
  if (conf->has("ALCT_L1A_WINDOW_WIDTH"))          alct_->SetL1aWindowSize( getInt(conf, "ALCT_L1A_WINDOW_WIDTH"));
  if (conf->has("ALCT_L1A_OFFSET"))                alct_->SetL1aOffset( getInt(conf, "ALCT_L1A_OFFSET"));
  if (conf->has("ALCT_L1A_INTERNAL"))              alct_->SetL1aInternal( getInt(conf, "ALCT_L1A_INTERNAL"));
  if (conf->has("ALCT_CCB_ENABLE"))                alct_->SetCcbEnable( getInt(conf, "ALCT_CCB_ENABLE"));
  if (conf->has("ALCT_CONFIG_IN_READOUT"))         alct_->SetConfigInReadout( getInt(conf, "ALCT_CONFIG_IN_READOUT"));
  if (conf->has("ALCT_ACCEL_MODE"))                alct_->SetAlctAmode( getInt(conf, "ALCT_ACCEL_MODE"));
  if (conf->has("ALCT_TRIG_INFO_EN"))              alct_->SetTriggerInfoEnable( getInt(conf, "ALCT_TRIG_INFO_EN"));
  if (conf->has("ALCT_SN_SELECT"))                 alct_->SetSnSelect( getInt(conf, "ALCT_SN_SELECT"));
  if (conf->has("ALCT_TESTPULSE_AMPLITUDE"))       alct_->SetTestpulseAmplitude( getInt(conf, "ALCT_TESTPULSE_AMPLITUDE"));
  if (conf->has("ALCT_TESTPULSE_INVERT"))          alct_->Set_InvertPulse( getString(conf, "ALCT_TESTPULSE_INVERT"));
  if (conf->has("ALCT_TESTPULSE_DIRECTION"))       alct_->Set_PulseDirection( getString(conf, "ALCT_TESTPULSE_DIRECTION"));
  if (conf->has("ALCT_ZERO_SUPPRESS"))             alct_->SetAlctZeroSuppress( getInt(conf, "ALCT_ZERO_SUPPRESS"));

  if(verbose_)
  {
    xdata::Table t = conf->row();
    t.writeTo(std::cout); std::cout<< std::endl;
    std::cout << "#### ALCT added ####"<< std::endl;
  }

  std::vector<emu::db::ConfigRow *> kids = conf->children();
  for (std::vector<emu::db::ConfigRow *>::iterator kid = kids.begin(); kid != kids.end(); kid++)
  {
    std::string type = (*kid)->type();
    if (type == "AnodeChannel")
    {
      readAnodeChannel(*kid, alct_);
    }
    else
    {
      XCEPT_RAISE(emu::exception::ConfigurationException, "Unexpected child table type for ALCT: " + type );
    }
  }

  if(verbose_) std::cout << "#### ALCT added ####"<< std::endl;
}


void EmuEndcapConfigWrapper::readAnodeChannel(emu::db::ConfigRow *conf, ALCTController * theAlct)
throw (emu::exception::ConfigurationException)
{
  if(verbose_) std::cout << "==== AFEB ====" << std::endl;

  int afeb_number = getInt(conf, "AFEB_NUMBER");
  if(verbose_) std::cout << "AFEB(" << afeb_number << ")" << std::endl;
  theAlct->SetStandbyRegister_(afeb_number-1,ON);

  if (conf->has("AFEB_FINE_DELAY")) theAlct->SetAsicDelay(afeb_number-1, getInt(conf, "AFEB_FINE_DELAY"));
  if (conf->has("AFEB_THRESHOLD"))  theAlct->SetAfebThreshold(afeb_number-1, getInt(conf, "AFEB_THRESHOLD"));

  if(verbose_)
  {
    xdata::Table t = conf->row();
    t.writeTo(std::cout); std::cout<< std::endl;
    std::cout << "#### AFEB added ####"<< std::endl;
  }
}


std::string EmuEndcapConfigWrapper::getString(emu::db::ConfigRow *conf, std::string columnName)
throw (emu::exception::ConfigurationException)
{
  return conf->getCastValue<xdata::String>(columnName).toString();
}


int EmuEndcapConfigWrapper::getInt(emu::db::ConfigRow *conf, std::string columnName)
throw (emu::exception::ConfigurationException)
{
  // All integers in PC configuration are actually stored as unsigned shorts
  xdata::UnsignedShort i = conf->getCastValue<xdata::UnsignedShort>(columnName);
  if(i.isNaN()) return 0;
  return (int) i;
}


float EmuEndcapConfigWrapper::getFloat(emu::db::ConfigRow *conf, std::string columnName)
throw (emu::exception::ConfigurationException)
{
  return (float) conf->getCastValue<xdata::Float>(columnName);
}


int EmuEndcapConfigWrapper::getHexStringAsInt(emu::db::ConfigRow *conf, std::string columnName)
throw (emu::exception::ConfigurationException)
{
  int result;
  std::string str = getString(conf, columnName);
  sscanf(str.c_str(), "%x", &result);
  return result;
}


long int EmuEndcapConfigWrapper::getStringAsLongInt(emu::db::ConfigRow *conf, std::string columnName)
throw (emu::exception::ConfigurationException)
{
  long int result;
  std::string str = getString(conf, columnName);
  sscanf(str.c_str(), "%lx", &result);
  return result;
}


long long int EmuEndcapConfigWrapper::getStringAsLongLongInt(emu::db::ConfigRow *conf, std::string columnName)
throw (emu::exception::ConfigurationException)
{
  long long int result;
  std::string str = getString(conf, columnName);
  sscanf(str.c_str(), "%Lx", &result);
  return result;
}


}} // namespaces
