#include "XMLParser.h"
#include <stdio.h>
#include <iostream>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/XMLPScanToken.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

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

namespace emu {
  namespace pc {

XMLParser::XMLParser(): pAttributes_(0), emuEndcap_(0) {}

void XMLParser::parseNode(xercesc::DOMNode * pNode) 
{
  pAttributes_ = pNode->getAttributes();

#ifdef debugV
  std::cout << "   number of attributes = " << pAttributes_->getLength() << std::endl;
#endif
}

bool XMLParser::fillInt(std::string item, int & target) 
{
  bool found=false;
  int value; 
  XMLCh * name = xercesc::XMLString::transcode(item.c_str());
  xercesc::DOMAttr * pAttributeNode = (xercesc::DOMAttr*) pAttributes_->getNamedItem(name);
  if(pAttributeNode) {
    int err = sscanf(xercesc::XMLString::transcode(pAttributeNode->getNodeValue()), "%d", &value);
    if (err==0) std::cerr << "ERRORS in parsing!!!" << item << " code " << err << std::endl;
    target = value;
    found = true;
#ifdef debugV
    std::cout << "  " << item << " = " << target << std::endl;
#endif
  }
  return found;  
}

bool XMLParser::fillIntX(std::string item, int & target) 
{
  bool found=false;
  int value; 
  XMLCh * name = xercesc::XMLString::transcode(item.c_str());
  xercesc::DOMAttr * pAttributeNode = (xercesc::DOMAttr*) pAttributes_->getNamedItem(name);
  if(pAttributeNode) {
    int err = sscanf(xercesc::XMLString::transcode(pAttributeNode->getNodeValue()), "%x", &value);
    if (err==0) std::cerr << "ERRORS in parsing!!!" << item << " code " << err << std::endl;
    target = value;
    found = true;
#ifdef debugV
    std::cout << "  " << item << " = " << target << std::endl;
#endif
  }
  return found;  
}

bool XMLParser::fillLongIntX(std::string item, long int & target) 
{
  bool found=false;
  long int value; 
  XMLCh * name = xercesc::XMLString::transcode(item.c_str());
  xercesc::DOMAttr * pAttributeNode = (xercesc::DOMAttr*) pAttributes_->getNamedItem(name);
  if(pAttributeNode) {
    int err = sscanf(xercesc::XMLString::transcode(pAttributeNode->getNodeValue()), "%lx", &value);
    if (err==0) std::cerr << "ERRORS in parsing!!!" << item << " code " << err << std::endl;
    target = value;
    found = true;
#ifdef debugV
    std::cout << "fillLongIntX: " << item << " = 0x" << std::hex << target << std::endl;
#endif
  }
  return found;  
}

bool XMLParser::fillLongLongIntX(std::string item, long long int & target) 
{
  bool found=false;
  long long int value; 
  XMLCh * name = xercesc::XMLString::transcode(item.c_str());
  xercesc::DOMAttr * pAttributeNode = (xercesc::DOMAttr*) pAttributes_->getNamedItem(name);
  if(pAttributeNode) {
    int err = sscanf(xercesc::XMLString::transcode(pAttributeNode->getNodeValue()), "%Lx", &value);
    if (err==0) std::cerr << "ERRORS in parsing!!!" << item << " code " << err << std::endl;
    target = value;
    found = true;
#ifdef debugV
    std::cout << "fillLongLongIntX: " << item << " = 0x" << std::hex << target << std::endl;
#endif
  }
  return found;  
}


bool XMLParser::fillString(std::string item, std::string & target) 
{
  bool found=false;
  std::string value;
  XMLCh * name = xercesc::XMLString::transcode(item.c_str());
  xercesc::DOMAttr * pAttributeNode = (xercesc::DOMAttr*) pAttributes_->getNamedItem(name);
  if(pAttributeNode) {
    value = xercesc::XMLString::transcode(pAttributeNode->getNodeValue());
    target = value;
    found = true;
#ifdef debugV
      std::cout << "  " << item << " = " << target << std::endl;
#endif
  }
  return found;
}


bool XMLParser::fillFloat(std::string item, float & target) 
{
  float value;
  bool found=false;
  XMLCh * name = xercesc::XMLString::transcode(item.c_str());
  xercesc::DOMAttr * pAttributeNode = (xercesc::DOMAttr*) pAttributes_->getNamedItem(name);
  if(pAttributeNode) {
    int err = sscanf(xercesc::XMLString::transcode(pAttributeNode->getNodeValue()), "%f", &value);
    if (err==0) std::cerr << "ERRORS in parsing!!!" << item << " code " << err << std::endl;
    target = value;
    found = true;
#ifdef debugV
      std::cout << "  " << item << " = " << target << std::endl;
#endif
  }
  return found;
}

bool XMLParser::VCCParser(xercesc::DOMNode *pNode, Crate * theCrate)
{
  int         Value;
  std::string StrgValue;
  bool found_MAC_and_port;

  parseNode(pNode);

  std::cout << "Found VCC" << std::endl;
  found_MAC_and_port = true;

  // add VMECC(VMEModule in slot 1) to crate
  VMECC * vcc = new VMECC(theCrate, 1);
  std::cout << "VMECC in slot " << vcc->slot() << std::endl;

  if(fillString("MAC_addr", StrgValue)){
    theCrate->vmeController()->SetMAC(0,StrgValue);
    size_t pos=StrgValue.find('-');
    while(pos!=std::string::npos){
      StrgValue.replace(pos,1,1,':');
      pos=StrgValue.find('-');
    }
    theCrate->vmeController()->SetVMEAddress(StrgValue);
  }
  else {
    found_MAC_and_port = false;
  }
  if(fillInt("eth_port",Value)){
    theCrate->vmeController()->SetPort(Value);
  }
  else {
    found_MAC_and_port = false;
  }
  if(fillString("MCAST_1", StrgValue))theCrate->vmeController()->SetMAC(1,StrgValue);
  if(fillString("MCAST_2", StrgValue))theCrate->vmeController()->SetMAC(2,StrgValue);
  if(fillString("MCAST_3", StrgValue))theCrate->vmeController()->SetMAC(3,StrgValue);
  if(fillString("Dflt_Srv_MAC", StrgValue))theCrate->vmeController()->SetMAC(4,StrgValue);
  if(fillString("VCC_frmw_ver", StrgValue));vcc->SetVCC_frmw_ver(StrgValue);
  if(fillString("Ethernet_CR", StrgValue))theCrate->vmeController()->SetCR(0,StrgValue);
  if(fillString("Ext_FIFO_CR", StrgValue))theCrate->vmeController()->SetCR(1,StrgValue);
  if(fillString("Rst_Misc_CR", StrgValue))theCrate->vmeController()->SetCR(2,StrgValue);
  if(fillString("VME_CR", StrgValue))theCrate->vmeController()->SetCR(3,StrgValue);
  if(fillString("BTO", StrgValue))theCrate->vmeController()->SetCR(4,StrgValue);
  if(fillString("BGTO", StrgValue))theCrate->vmeController()->SetCR(5,StrgValue);
  if(fillInt("Msg_Lvl", Value))theCrate->vmeController()->SetMsg_Lvl(Value);
  if(fillString("Warn_On_Shtdwn", StrgValue)){
    if(StrgValue == "true"){
      theCrate->vmeController()->SetWarn_On_Shtdwn(true);
    }else{
      theCrate->vmeController()->SetWarn_On_Shtdwn(false);
    }
  }
  if(fillString("Pkt_On_Startup", StrgValue)){
    if(StrgValue == "true"){
      theCrate->vmeController()->SetPkt_On_Startup(true);
    }else{
      theCrate->vmeController()->SetPkt_On_Startup(false);
    }
  }

  return found_MAC_and_port;
}

void XMLParser::CCBParser(xercesc::DOMNode *pNode, Crate * theCrate)
{
  int slot, mode, BxOrbit, SPS25ns, delay, ID;

  parseNode(pNode);

  if(!fillInt("slot", slot)) slot = 13;;
  CCB * ccb_ = new CCB(theCrate, slot);
  if ( fillInt("TTCmode", mode) ) {
    ccb_->SetTTCmode(mode);
  }
  if ( fillInt("CCBmode", mode) ) {
    ccb_->SetCCBmode(mode);
  }
  if ( fillInt("BX_Orbit" , BxOrbit) ) {
    ccb_->SetBxOrbit(BxOrbit);
  }
  if(fillInt("SPS25ns", SPS25ns)) {
    ccb_->SetSPS25ns(SPS25ns);
  }
  if (fillInt("l1aDelay", delay)){
    ccb_->Setl1adelay(delay);
  }
  if (fillInt("TTCrxID", ID)){
    ccb_->SetTTCrxID(ID);
  }
  if (fillInt("TTCrxCoarseDelay", delay)){
    ccb_->SetTTCrxCoarseDelay(delay);
  }
  if (fillInt("TTCrxFineDelay", delay)){
    ccb_->SetTTCrxFineDelay(delay);
  }
  if ( fillInt("ccb_firmware_year",delay) ) {
    ccb_->SetExpectedFirmwareYear(delay);
  }
  if ( fillInt("ccb_firmware_month",delay) ) {
    ccb_->SetExpectedFirmwareMonth(delay);
  }
  if ( fillInt("ccb_firmware_day",delay) ) {
    ccb_->SetExpectedFirmwareDay(delay);
  }
}

void XMLParser::MPCParser(xercesc::DOMNode * pNode, Crate * theCrate)
{
  int slot, value;

  parseNode(pNode);

  if(!fillInt("slot", slot)) slot = 12;
  MPC * mpc_ = new MPC(theCrate, slot);
  
  if(fillInt("serializermode",value)){
    mpc_->SetTLK2501TxMode(value);
  }

  if ( fillInt("TransparentMode",value)){
    mpc_->SetTransparentMode(value);
  }

  if(fillInt("TMBdelays",value)){
    mpc_->SetTMBDelays(value);
  }

  if ( fillInt("mpc_firmware_year",value) ) {
    mpc_->SetExpectedFirmwareYear(value);
  }
  if ( fillInt("mpc_firmware_month",value) ) {
    mpc_->SetExpectedFirmwareMonth(value);
  }
  if ( fillInt("mpc_firmware_day",value) ) {
    mpc_->SetExpectedFirmwareDay(value);
  }

//  if(fillInt("BoardID",value)){
//    mpc_->SetBoardID(value);
//  }
    mpc_->SetBoardID(theCrate->CrateID());
 
}

void XMLParser::TMBParser(xercesc::DOMNode * pNode, Crate * theCrate, Chamber * theChamber, xercesc::DOMNode * pNodeGlobal)
{
  // dealing with global values
  int Global_alct_pretrig_thresh;
  bool got_alct_pretrig_thresh=false;

  if (pNodeGlobal) 
  {
     parseNode(pNodeGlobal);

     // add all globals here
     got_alct_pretrig_thresh = fillInt("alct_pretrig_thresh" , Global_alct_pretrig_thresh);
  }

  parseNode(pNode);

  int slot = 0;
  fillInt("slot", slot);
  std::cout << "Inside TMBParser..."<<std::endl;
  if(slot == 0) {
    std::cerr << "No slot specified for TMB! " << std::endl;
  } else {
    //
    TMB * tmb_ = new TMB(theCrate, theChamber, slot);
    //
    // need still to put in 
    //   . ddd_oe mask
    int value;
    //////////////////////////////
    // Expected Firmware tags:
    //////////////////////////////
    if (fillInt("tmb_firmware_month"  ,value)) { tmb_->SetExpectedTmbFirmwareMonth(value);   }
    if (fillInt("tmb_firmware_day"    ,value)) { tmb_->SetExpectedTmbFirmwareDay(value);     }
    if (fillInt("tmb_firmware_year"   ,value)) { tmb_->SetExpectedTmbFirmwareYear(value);    }
    if (fillInt("tmb_firmware_version",value)) { tmb_->SetExpectedTmbFirmwareVersion(value); }
    if (fillInt("tmb_firmware_revcode",value)) { tmb_->SetExpectedTmbFirmwareRevcode(value); }
    if (fillInt("tmb_firmware_type"   ,value)) { tmb_->SetExpectedTmbFirmwareType(value);    }
    ///
    if (fillInt("rat_firmware_month"  ,value)) { tmb_->SetExpectedRatFirmwareMonth(value); }
    if (fillInt("rat_firmware_day"    ,value)) { tmb_->SetExpectedRatFirmwareDay(value);   }
    if (fillInt("rat_firmware_year"   ,value)) { tmb_->SetExpectedRatFirmwareYear(value);  }
    //
    //0x0E:
    if (fillInt("enable_alct_tx",value)) { tmb_->SetEnableAlctTx(value); }
    if (fillInt("enable_alct_rx",value)) { tmb_->SetAlctInput(value);    }
    //    
    //0X16
    if (fillInt("alct_tx_clock_delay",value)) { tmb_->SetAlctTXclockDelay(value); }
    if (fillInt("alct_rx_clock_delay",value)) { tmb_->SetAlctRXclockDelay(value); }
    if (fillInt("dmb_tx_delay"       ,value)) { tmb_->SetDmbTxDelay(value);       }
    if (fillInt("rat_tmb_delay"      ,value)) { tmb_->SetRatTmbDelay(value);      }
    //
    //0X18
    //if (fillInt("tmb1_phase",value)) { tmb_->SetTmb1Phase(value);  }
    //if (fillInt("mpc_phase" ,value)) { tmb_->SetMpcPhase(value);   }
    //if (fillInt("dcc_phase" ,value)) { tmb_->SetDccPhase(value);   }
    if (fillInt("cfeb0delay",value)) { tmb_->SetCFEB0delay(value); }
    //
    //0X1A
    if (fillInt("cfeb1delay",value)) { tmb_->SetCFEB1delay(value); }
    if (fillInt("cfeb2delay",value)) { tmb_->SetCFEB2delay(value); }
    if (fillInt("cfeb3delay",value)) { tmb_->SetCFEB3delay(value); }
    if (fillInt("cfeb4delay",value)) { tmb_->SetCFEB4delay(value); }
    //
    //0X1C
    // put int ddd_oe...
    //
    //0X2C
    //if (fillInt("alct_ext_trig_l1aen"   ,value)) { tmb_->SetEnableL1aRequestOnAlctExtTrig(value); }
    //if (fillInt("clct_ext_trig_l1aen"   ,value)) { tmb_->SetEnableL1aRequestOnClctExtTrig(value); }
    if (fillInt("request_l1a"           ,value)) { tmb_->SetRequestL1a(value);                    }
    //if (fillInt("alct_ext_trig_vme"     ,value)) { tmb_->SetAlctExtTrigVme(value);                }
    //if (fillInt("clct_ext_trig_vme"     ,value)) { tmb_->SetClctExtTrigVme(value);                }
    //if (fillInt("ext_trig_both"         ,value)) { tmb_->SetExtTrigBoth(value);                   }
    //if (fillInt("ccb_allow_bypass"      ,value)) { tmb_->SetCcbAllowExternalBypass(value);        }
    if (fillInt("ignore_ccb_startstop"    ,value)) { tmb_->SetIgnoreCcbStartStop(value);          }
    //if (fillInt("internal_l1a_delay_vme",value)) { tmb_->SetInternalL1aDelay(value);              }
    //
    //0X30
    //if (fillInt("cfg_alct_ext_trig_en"  ,value)) { tmb_->SetEnableAlctExtTrig(value);     }
    //if (fillInt("cfg_alct_ext_inject_en",value)) { tmb_->SetEnableAlctExtInject(value);   }
    //if (fillInt("cfg_alct_ext_trig"     ,value)) { tmb_->SetAlctExtTrig(value);           }
    //if (fillInt("cfg_alct_ext_inject"   ,value)) { tmb_->SetAlctExtInject(value);         }
    //if (fillInt("alct_seq_cmd"          ,value)) { tmb_->SetAlctSequencerCommand(value);  }
    if (fillInt("alct_clock_en_use_ccb" ,value)) { tmb_->SetEnableAlctUseCcbClock(value); }
    //if (fillInt("alct_clock_en_use_vme" ,value)) { tmb_->SetAlctClockVme(value);          }
    //
    //0X32:
    if (fillInt("alct_clear"     ,value)) { tmb_->SetAlctClear(value);                  }
    //if (fillInt("alct_inject_mux",value)) { tmb_->SetAlctInject(value);                 }
    //if (fillInt("alct_sync_clct" ,value)) { tmb_->SetSyncAlctInjectToClctInject(value); }
    //if (fillInt("alct_inj_delay" ,value)) { tmb_->SetAlctInjectorDelay(value);          }
    //
    //0X42:
    if (fillInt("enableCLCTInputs_reg42",value)) { tmb_->SetEnableCLCTInputs(value);     }
    //if (fillInt("cfeb_ram_sel"          ,value)) { tmb_->SetSelectCLCTRAM(value);        }
    //if (fillInt("cfeb_inj_en_sel"       ,value)) { tmb_->SetEnableCLCTInject(value);     }
    //if (fillInt("start_pattern_inj"     ,value)) { tmb_->SetStartPatternInjector(value); }
    //
    ///////////////////////////////////
    //0X4A,4C,4E = ADR_HCM001,HCM023,HCM045 = CFEB0 Hot Channel Masks
    //0X50,52,54 = ADR_HCM101,HCM123,HCM145 = CFEB1 Hot Channel Masks
    //0X56,58,5A = ADR_HCM201,HCM223,HCM245 = CFEB2 Hot Channel Masks
    //0X5C,5E,60 = ADR_HCM301,HCM323,HCM345 = CFEB3 Hot Channel Masks
    //0X62,64,66 = ADR_HCM401,HCM423,HCM445 = CFEB4 Hot Channel Masks
    ///////////////////////////////////
    long long int hot_channel_mask;
    if (fillLongLongIntX("layer0_distrip_hot_chann_mask", hot_channel_mask) ) 
      tmb_->SetDistripHotChannelMask(0,hot_channel_mask);
    //
    if (fillLongLongIntX("layer1_distrip_hot_chann_mask", hot_channel_mask) ) 
      tmb_->SetDistripHotChannelMask(1,hot_channel_mask);
    //
    if (fillLongLongIntX("layer2_distrip_hot_chann_mask", hot_channel_mask) ) 
      tmb_->SetDistripHotChannelMask(2,hot_channel_mask);
    //
    if (fillLongLongIntX("layer3_distrip_hot_chann_mask", hot_channel_mask) ) 
      tmb_->SetDistripHotChannelMask(3,hot_channel_mask);
    //
    if (fillLongLongIntX("layer4_distrip_hot_chann_mask", hot_channel_mask) ) 
      tmb_->SetDistripHotChannelMask(4,hot_channel_mask);
    //
    if (fillLongLongIntX("layer5_distrip_hot_chann_mask", hot_channel_mask) ) 
      tmb_->SetDistripHotChannelMask(5,hot_channel_mask);
    //
    //0X68:
    if (fillInt("clct_pretrig_enable"    ,value)) { tmb_->SetClctPatternTrigEnable(value);  }
    if (fillInt("alct_pretrig_enable"    ,value)) { tmb_->SetAlctPatternTrigEnable(value);  }
    if (fillInt("match_pretrig_enable"   ,value)) { tmb_->SetMatchPatternTrigEnable(value); }
    //if (fillInt("adb_ext_pretrig_enable" ,value)) { tmb_->SetAdbExtTrigEnable(value);       }
    //if (fillInt("dmb_ext_pretrig_enable" ,value)) { tmb_->SetDmbExtTrigEnable(value);       }
    if (fillInt("clct_ext_pretrig_enable",value)) { tmb_->SetClctExtTrigEnable(value);      }
    //if (fillInt("alct_ext_pretrig_enable",value)) { tmb_->SetAlctExtTrigEnable(value);      }
    //if (fillInt("ext_trig_inject"        ,value)) { tmb_->SetExtTrigInject(value);          }
    if (fillInt("all_cfeb_active"        ,value)) { tmb_->SetEnableAllCfebsActive(value);   }
    if (fillInt("enableCLCTInputs_reg68" ,value)) { tmb_->SetCfebEnable(value);             }
    if (fillInt("cfeb_enable_source"     ,value)) { tmb_->Set_cfeb_enable_source(value);    }
    //
    //0X6A:
    if (fillInt("match_pretrig_window_size",value)) { tmb_->SetAlctClctPretrigWidth(value); }
    if (fillInt("match_pretrig_alct_delay" ,value)) { tmb_->SetAlctPretrigDelay(value);     }
    //if (fillInt("alct_pat_trig_delay"      ,value)) { tmb_->SetAlctPatternDelay(value);     }
    //if (fillInt("adb_ext_trig_delay"       ,value)) { tmb_->SetAdbExternalTrigDelay(value); }
    //
    //0X6C:
    //if (fillInt("dmb_ext_trig_delay" ,value)) { tmb_->SetDmbExternalTrigDelay(value);  }
    //if (fillInt("clct_ext_trig_delay",value)) { tmb_->SetClctExternalTrigDelay(value); }
    //if (fillInt("alct_ext_trig_delay",value)) { tmb_->SetAlctExternalTrigDelay(value); }
    //
    //0X6E
    //if (fillInt("slot"  ,value)) { tmb_->SetTmbSlot(value); }
    //if (fillInt("csc_id",value)) { tmb_->SetCscId(value);   }
    //if (fillInt("run_id",value)) { tmb_->SetRunId(value);   }
    //
    //0X70:
    if (fillInt("clct_hit_persist"        ,value)) { tmb_->SetTriadPersistence(value); }
    if (fillInt("clct_nplanes_hit_pretrig",value)) { tmb_->SetHsPretrigThresh(value);  }
    if (fillInt("clct_nplanes_hit_pattern",value)) { tmb_->SetMinHitsPattern(value);   }
    if (fillInt("clct_drift_delay"        ,value)) { tmb_->SetDriftDelay(value);       }
    //if (fillInt("pretrigger_halt"              ,value)) { tmb_->SetPretriggerHalt(value);   }
    //
    //0X72
    if (fillInt("tmb_fifo_mode"       ,value)) { tmb_->SetFifoMode(value);      }
    if (fillInt("clct_fifo_tbins"     ,value)) { tmb_->SetFifoTbins(value);     }
    if (fillInt("clct_fifo_pretrig"   ,value)) { tmb_->SetFifoPreTrig(value);   }
    if (fillInt("tmb_fifo_no_raw_hits",value)) { tmb_->SetFifoNoRawHits(value); }
    //
    //0X74:
    if (fillInt("tmb_l1a_delay"       ,value)) { tmb_->SetL1aDelay(value);      }
    if (fillInt("tmb_l1a_window_size" ,value)) { tmb_->SetL1aWindowSize(value); }
    //if (fillInt("tmb_l1a_internal"    ,value)) { tmb_->SetInternalL1a(value);   }
    //
    //0X76:
    if (fillInt("tmb_l1a_offset",value)) { tmb_->SetL1aOffset(value); }
    if (fillInt("tmb_bxn_offset",value)) { tmb_->SetBxnOffset(value); }
    //
    //0X86
    if (fillInt("mpc_sync_err_enable",value)) { tmb_->SetTmbSyncErrEnable(value); }
    if (fillInt("alct_trig_enable"   ,value)) { tmb_->SetTmbAllowAlct(value);     }
    if (fillInt("clct_trig_enable"   ,value)) { tmb_->SetTmbAllowClct(value);     }
    if (fillInt("match_trig_enable"  ,value)) { tmb_->SetTmbAllowMatch(value);    }
    if (fillInt("mpc_rx_delay"       ,value)) { tmb_->SetMpcRxDelay(value);       }
    //if (fillInt("mpc_sel_ttc_bx0"    ,value)) { tmb_->SetSelectMpcTtcBx0(value);  }
    if (fillInt("mpc_idle_blank"     ,value)) { tmb_->SetMpcIdleBlank(value);     }
    if (fillInt("mpc_output_enable"  ,value)) { tmb_->SetMpcOutputEnable(value);  }
    //
    //0XAC
    //if (fillInt("clct_flush_delay"       ,value)) { tmb_->SetClctFlushDelay(value);            }
    if (fillInt("write_buffer_autoclear" ,value)) { tmb_->SetWriteBufferAutoclear(value);      }
    if (fillInt("write_continuous_enable",value)) { tmb_->SetClctWriteContinuousEnable(value); }
    if (fillInt("write_buffer_required"  ,value)) { tmb_->SetWriteBufferRequired(value);       }
    if (fillInt("valid_clct_required"    ,value)) { tmb_->SetRequireValidClct(value);          }
    if (fillInt("l1a_allow_match"        ,value)) { tmb_->SetL1aAllowMatch(value);             }
    if (fillInt("l1a_allow_notmb"        ,value)) { tmb_->SetL1aAllowNoTmb(value);             }
    if (fillInt("l1a_allow_nol1a"        ,value)) { tmb_->SetL1aAllowNoL1a(value);             }
    if (fillInt("l1a_allow_alct_only"    ,value)) { tmb_->SetL1aAllowAlctOnly(value);          }
    //if (fillInt("scint_veto_clr"         ,value)) { tmb_->SetScintillatorVetoClear(value);     }
    //
    //0XB2:
    if (fillInt("match_trig_alct_delay" ,value)) { tmb_->SetAlctVpfDelay(value);        }
    if (fillInt("match_trig_window_size",value)) { tmb_->SetAlctMatchWindowSize(value); }
    if (fillInt("mpc_tx_delay"          ,value)) { tmb_->SetMpcTxDelay(value);          }
    //
    //0XB6:
    if (fillInt("rpc_exists"     ,value)) { tmb_->SetRpcExist(value);           }
    if (fillInt("rpc_read_enable",value)) { tmb_->SetRpcReadEnable(value);      }
    if (fillInt("rpc_bxn_offset" ,value)) { tmb_->SetRpcBxnOffset(value);       }    
    //if (fillInt("rpc_bank_addr"  ,value)) { tmb_->SetRpcSyncBankAddress(value); }
    //
    //0XBA:
    if (fillInt("rpc0_raw_delay",value)) { tmb_->SetRpc0RawDelay(value); }
    if (fillInt("rpc1_raw_delay",value)) { tmb_->SetRpc1RawDelay(value); }
    //if (fillInt("rpc2_raw_delay",value)) { tmb_->SetRpc2RawDelay(value); }
    //if (fillInt("rpc3_raw_delay",value)) { tmb_->SetRpc3RawDelay(value); }
    //
    //0xBC:
    if (fillInt("rpc_mask_all" ,value)) { tmb_->SetEnableRpcInput(value);           }
    //if (fillInt("inj_mask_rat" ,value)) { tmb_->SetInjectorMaskRat(value);          }
    //if (fillInt("inj_mask_rpc" ,value)) { tmb_->SetInjectorMaskRpc(value);          }
    //if (fillInt("inj_delay_rat",value)) { tmb_->SetInjectorDelayRat(value);         }
    //if (fillInt("rpc_inj_sel"  ,value)) { tmb_->SetRpcInjector(value);              }
    //if (fillInt("rpc_inj_wdata",value)) { tmb_->SetRpcInjectorWriteDataMSBs(value); }
    //
    //0XC4
    if (fillInt("rpc_fifo_tbins"   ,value)) { tmb_->SetFifoTbinsRpc(value);     }
    if (fillInt("rpc_fifo_pretrig" ,value)) { tmb_->SetFifoPretrigRpc(value);   }
    if (fillInt("rpc_fifo_decouple",value)) { tmb_->SetRpcDecoupleTbins(value); }
    //
    //0XCA
    if (fillInt("alct_bx0_delay" ,value)) { tmb_->SetAlctBx0Delay(value);  }
    if (fillInt("clct_bx0_delay" ,value)) { tmb_->SetClctBx0Delay(value);  }
    if (fillInt("alct_bx0_enable",value)) { tmb_->SetAlctBx0Enable(value); }
    //
    //0XCC
    if (fillInt("alct_readout_without_trig" ,value)) { tmb_->SetAllowAlctNontrigReadout(value);   }
    if (fillInt("clct_readout_without_trig" ,value)) { tmb_->SetAllowClctNontrigReadout(value);   }
    if (fillInt("match_readout_without_trig",value)) { tmb_->SetAllowMatchNontrigReadout(value);  }
    if (fillInt("mpc_block_me1a"            ,value)) { tmb_->SetBlockME1aToMPC(value);            }
    //if (fillInt("counter_clct_non_me11"     ,value)) { tmb_->SetClctPretriggerCounterME11(value); }
    if (fillIntX("tmb_firmware_compile_type" ,value)) { tmb_->SetTMBFirmwareCompileType(value);    }
    //0XE6
    if (fillInt("rpc0_rat_delay",value)) { tmb_->SetRpc0RatDelay(value); }
    if (fillInt("rpc1_rat_delay",value)) { tmb_->SetRpc1RatDelay(value); }
    //if (fillInt("rpc2_rat_delay",value)) { tmb_->SetRpc2RatDelay(value); }
    //if (fillInt("rpc3_rat_delay",value)) { tmb_->SetRpc3RatDelay(value); }
    //
    //0XF0
    if (fillInt("layer_trig_enable",value)) { tmb_->SetEnableLayerTrigger(value);    }
    if (fillInt("layer_trig_thresh",value)) { tmb_->SetLayerTriggerThreshold(value); }
    if (fillInt("clct_throttle"    ,value)) { tmb_->SetClctThrottle(value);          }
    //
    //0XF4
    if (fillInt("clct_blanking"          ,value)) { tmb_->SetClctBlanking(value);        }
    if (fillInt("clct_pid_thresh_pretrig",value)) { tmb_->SetClctPatternIdThresh(value); }
    if (fillInt("aff_thresh"             ,value)) { tmb_->SetActiveFebFlagThresh(value); }
    if (fillInt("adjacent_cfeb_distance" ,value)) { tmb_->SetAdjacentCfebDistance(value);}
    //
    //0XF6
    if (fillInt("clct_min_separation",value)) { tmb_->SetMinClctSeparation(value); }
    //
    //
    xercesc::DOMNode * daughterNode = pNode->getFirstChild();
    while(daughterNode) {
      if (daughterNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE and 
	  strcmp("ALCT",xercesc::XMLString::transcode(daughterNode->getNodeName()))==0){
	//
	std::string  nodeName = xercesc::XMLString::transcode(daughterNode->getNodeName());
#ifdef debugV  
	std::cout << nodeName << std::endl;
#endif
	//
	parseNode(daughterNode);
	
        std::string chamberType;
        fillString("chamber_type", chamberType);
        //alct_ memory area is deleted in ~TMB()
        ALCTController * alct_ = new ALCTController(tmb_, chamberType);
	RAT * rat_ = new RAT(tmb_);
	tmb_->SetAlct(alct_); //store alct_ pointer in tmb_
	tmb_->SetRat(rat_); //store rat_  in tmb_
	//
	//RAT
       	//if(fillInt("rpc0_rat_delay",value)) { rat_->SetRpcRatDelay(0,value); }
	//
	//ALCT
	// need to put in 
	//   . hot channel mask
	//   . collision mask
	//
	////////////////////////////////////////////////////////
	// Expected firmware tags
	////////////////////////////////////////////////////////
	if (fillInt("alct_firmware_day"  ,value)) { alct_->SetExpectedFastControlDay(value);   }
	if (fillInt("alct_firmware_month",value)) { alct_->SetExpectedFastControlMonth(value); }
	if (fillInt("alct_firmware_year" ,value)) { alct_->SetExpectedFastControlYear(value);  }
	//
	std::string alct_firmware_backwardForward;
        if (fillString("alct_firmware_backward_forward", alct_firmware_backwardForward) ){
	  alct_->Set_fastcontrol_backward_forward_type(alct_firmware_backwardForward);
	}
	std::string alct_firmware_negativePositive;
        if (fillString("alct_firmware_negat_posit", alct_firmware_negativePositive) ){
	  alct_->Set_fastcontrol_negative_positive_type(alct_firmware_negativePositive);
	}
	//
	////////////////////////////////////////////////////////
	// configuration register
	////////////////////////////////////////////////////////
	if (fillInt("alct_trig_mode"      ,value)) { alct_->SetTriggerMode(value);            }
	if (fillInt("alct_ext_trig_enable",value)) { alct_->SetExtTrigEnable(value);          }
	if (fillInt("alct_send_empty"     ,value)) { alct_->SetSendEmpty(value);              }
	if (fillInt("alct_inject_mode"    ,value)) { alct_->SetInjectMode(value);             }
	if (fillInt("alct_bxn_offset"     ,value)) { alct_->SetBxcOffset(value);              }
	if (fillInt("alct_nplanes_hit_pretrig" ,value)) { alct_->SetPretrigNumberOfLayers(value);  }
	if (pNodeGlobal){
	   if(got_alct_pretrig_thresh) alct_->SetPretrigNumberOfLayers(Global_alct_pretrig_thresh);
	}
	if (fillInt("alct_nplanes_hit_pattern"      ,value)) { alct_->SetPretrigNumberOfPattern(value);   }
	if (fillInt("alct_nplanes_hit_accel_pretrig",value)) { alct_->SetAcceleratorPretrigThresh(value); }
	if (fillInt("alct_nplanes_hit_accel_pattern",value)) { alct_->SetAcceleratorPatternThresh(value); }
	if (fillInt("alct_drift_delay"         ,value)) { alct_->SetDriftDelay(value);               }
	if (fillInt("alct_fifo_tbins"          ,value)) { alct_->SetFifoTbins(value);                }
	if (fillInt("alct_fifo_pretrig"        ,value)) { alct_->SetFifoPretrig(value);              }
	if (fillInt("alct_fifo_mode"           ,value)) { alct_->SetFifoMode(value);                 }
	if (fillInt("alct_l1a_delay"           ,value)) { alct_->SetL1aDelay(value);                 }
	if (fillInt("alct_l1a_window_width"     ,value)) { alct_->SetL1aWindowSize(value);            }
	if (fillInt("alct_l1a_offset"          ,value)) { alct_->SetL1aOffset(value);                }
	if (fillInt("alct_l1a_internal"        ,value)) { alct_->SetL1aInternal(value);	         }
	if (fillInt("alct_ccb_enable"          ,value)) { alct_->SetCcbEnable(value);                }
	if (fillInt("alct_config_in_readout"   ,value)) { alct_->SetConfigInReadout(value);          }
	if (fillInt("alct_accel_mode"          ,value)) { alct_->SetAlctAmode(value);                }
	if (fillInt("alct_trig_info_en"        ,value)) { alct_->SetTriggerInfoEnable(value);        }
	if (fillInt("alct_sn_select"           ,value)) { alct_->SetSnSelect(value);                 }
	if (fillInt("alct_testpulse_amplitude" ,value)) { alct_->SetTestpulseAmplitude(value);       }
	std::string on_or_off;
	if (fillString("alct_testpulse_invert",on_or_off)) { alct_->Set_InvertPulse(on_or_off); }
	std::string afebs_or_strips;
	if (fillString("alct_testpulse_direction",afebs_or_strips)) { alct_->Set_PulseDirection(afebs_or_strips); }
	//	std::string file;
	//	if ( fillString("alct_pattern_file", file)) {
	//	  alct_->SetPatternFile(file);
	//	}
	//	if (fillString("alct_hotchannel_file", file)){
	//	  alct_->SetHotChannelFile(file);
	//	}
	
	int number, delay, threshold;
	
	xercesc::DOMNode * grandDaughterNode = daughterNode->getFirstChild();

	while (grandDaughterNode) {
	  if (grandDaughterNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE and
	      strcmp("AnodeChannel",xercesc::XMLString::transcode(grandDaughterNode->getNodeName()))==0){
	    //
	    std::string  nodeName = xercesc::XMLString::transcode(grandDaughterNode->getNodeName());
	    //
	    parseNode(grandDaughterNode);
	    if(fillInt("afeb_number", number)){
	      fillInt("afeb_fine_delay", delay);
	      fillInt("afeb_threshold", threshold);
	      //
              alct_->SetStandbyRegister_(number-1,ON);     //setting the threshold and delay in the xml file turns on this AFEB
	      alct_->SetAsicDelay(number-1,delay);
	      alct_->SetAfebThreshold(number-1,threshold);
	      //
	    }
	  }
	  grandDaughterNode = grandDaughterNode->getNextSibling();
	}
      }
      daughterNode = daughterNode->getNextSibling();
    }
  }
#ifdef debugV  
  std::cout << "finished TMBParsing" << std::endl;
#endif
}

void XMLParser::DAQMBParser(xercesc::DOMNode * pNode, Crate * theCrate, Chamber * theChamber)
{
  int slot = 0;

  parseNode(pNode);

  fillInt("slot", slot);
  if(slot == 0) {
    std::cerr << "No slot specified for DMB! " << std::endl;
  } else {
    //
    DAQMB * daqmb_ = new DAQMB(theCrate,theChamber,slot);  
    //
    int delay;
//     if ( fillInt("feb_dav_delay", delay)){
//       daqmb_->SetFebDavDelay(delay);
//     }
//     if ( fillInt("tmb_dav_delay", delay) ){
//       daqmb_->SetTmbDavDelay(delay);
//     }
//     if (fillInt("push_dav_delay", delay)){
//       daqmb_->SetPushDavDelay(delay);
//     }
//     if(fillInt("l1acc_dav_delay", delay)){
//       daqmb_->SetL1aDavDelay(delay);
//     }
//     if(fillInt("ALCT_dav_delay", delay)){
//       daqmb_->SetAlctDavDelay(delay);
//     }
    if(fillInt("calibration_LCT_delay", delay)){
      daqmb_->SetCalibrationLctDelay(delay);
    }
    if(fillInt("calibration_l1acc_delay", delay)){
      daqmb_->SetCalibrationL1aDelay(delay);
    }
    if(fillInt("pulse_delay", delay)){
      daqmb_->SetPulseDelay(delay);
    }
    if(fillInt("inject_delay", delay)){
      daqmb_->SetInjectDelay(delay);
    }
    float value;
    if(fillFloat("pul_dac_set", value)){
      daqmb_->SetPulseDac(value);
    }
    if(fillFloat("inj_dac_set", value)){
      daqmb_->SetInjectorDac(value);
    }
    if(fillFloat("set_comp_thresh", value)){
      daqmb_->SetCompThresh(value);
      for(int cfeb=0; cfeb<5; cfeb++) daqmb_->SetCompThresholdsCfeb(cfeb,value);
    }
    int mode;
    if(fillInt("comp_mode", mode)){
      daqmb_->SetCompMode(mode);
      for(int cfeb=0; cfeb<5; cfeb++) daqmb_->SetCompModeCfeb(cfeb,mode);
    }
    if(fillInt("comp_timing", delay)){
      daqmb_->SetCompTiming(delay);
      for(int cfeb=0; cfeb<5; cfeb++) daqmb_->SetCompTimingCfeb(cfeb,delay);
    }
    if(fillInt("pre_block_end", delay)){
      daqmb_->SetPreBlockEnd(delay);
      for(int cfeb=0; cfeb<5; cfeb++) daqmb_->SetPreBlockEndCfeb(cfeb,delay);
    }
    if (fillInt("cfeb_cable_delay", delay)) {
      daqmb_->SetCfebCableDelay(delay); }
    if (fillInt("tmb_lct_cable_delay", delay)) {
      daqmb_->SetTmbLctCableDelay(delay); }
    if (fillInt("cfeb_dav_cable_delay", delay)) {
      daqmb_->SetCfebDavCableDelay(delay); }
    if (fillInt("alct_dav_cable_delay", delay)) {
      daqmb_->SetAlctDavCableDelay(delay); }

//    int id;
//    if(fillInt("crate_id",id)){
//      daqmb_->SetCrateId(id);
//    }
      daqmb_->SetCrateId(theCrate->CrateID());
    if ( fillInt("feb_clock_delay",delay)){
      daqmb_->SetCfebClkDelay(delay);
    }
    if ( fillInt("xLatency",delay)){
      daqmb_->SetxLatency(delay);
      for (int cfeb=0; cfeb<5; cfeb++) daqmb_->SetL1aExtraCfeb(cfeb,delay);
    }
    if (fillInt("xFineLatency",delay)) {
      daqmb_->SetxFineLatency(delay);
    }
    // just use the variable 'delay' for killinput
    if (fillInt("kill_input",delay)) {
      daqmb_->SetKillInput(delay);
    }
    //
    long int long_value;
    if (fillLongIntX("dmb_cntl_firmware_tag", long_value) ) 
      daqmb_->SetExpectedControlFirmwareTag(long_value);
    //
    if (fillInt("dmb_vme_firmware_tag", delay) ) 
      daqmb_->SetExpectedVMEFirmwareTag(delay);
    //
    int number=0;   
    int kill_chip[6]={0x0000,0x0000,0x0000,0x0000,0x0000,0x0000};
    
    xercesc::DOMNode * daughterNode = pNode->getFirstChild();

    while (daughterNode) {
      if (daughterNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
	if (strcmp("CFEB",xercesc::XMLString::transcode(daughterNode->getNodeName()))==0){
	  parseNode(daughterNode);
	  if(fillInt("cfeb_number", number)){
	    //daqmb_->SendOutput("CFEB");
	    if ( number <5 ){
	      CFEB cfeb(number);
//fg explicitly prevend the parser from interpreting the following 5 options ...
//	      int ivalue;
//	      if ( fillInt("comp_mode",ivalue)){
//		daqmb_->SetCompModeCfeb(number,ivalue);
//	      }
//	      if ( fillInt("comp_timing",ivalue)){
//		daqmb_->SetCompTimingCfeb(number,ivalue);
//	      }
//	      int fvalue;
//	      if ( fillInt("comp_threshold",fvalue)){
//		daqmb_->SetCompThresholdsCfeb(number,fvalue);
//	      }
//	      if ( fillInt("pre_block_end",ivalue)){
//		daqmb_->SetPreBlockEndCfeb(number,ivalue);
//	      }
//	      if ( fillInt("L1A_extra",ivalue)){
//		daqmb_->SetL1aExtraCfeb(number,ivalue);
//	      }
//fg
	      if (fillLongIntX("cfeb_firmware_tag", long_value) ) 
		daqmb_->SetExpectedCFEBFirmwareTag(number,long_value);
	      //
	      fillIntX("kill_chip0",kill_chip[0]);
	      fillIntX("kill_chip1",kill_chip[1]);
	      fillIntX("kill_chip2",kill_chip[2]);
	      fillIntX("kill_chip3",kill_chip[3]);
	      fillIntX("kill_chip4",kill_chip[4]);
	      fillIntX("kill_chip5",kill_chip[5]);
	      //
	      for(int chip=0;chip<6;chip++){
		//
		daqmb_->SetKillChip(number,chip,kill_chip[chip]);
		//
		for(int chan=0;chan<16;chan++){
		  unsigned short int mask=(1<<chan);
		  if((mask&kill_chip[chip])==0x0000){
		    //daqmb_->shift_array_[number][5-chip][chan]=NORM_RUN;
		    daqmb_->SetShiftArray(number,5-chip,chan,NORM_RUN);
		  }else{
		    //daqmb_->shift_array_[number][5-chip][chan]=KILL_CHAN;
		    daqmb_->SetShiftArray(number,5-chip,chan,KILL_CHAN);
		  }
		}
	      } 	      
	      //
	      daqmb_->cfebs_.push_back(cfeb);
	    } else {
	      std::cout << "ERROR: CFEB range 0-4" <<std::endl;
	    }
	  }
	}
      }
      daughterNode = daughterNode->getNextSibling();
    }
  }
}

void XMLParser::CSCParser(xercesc::DOMNode * pNode, Crate * theCrate, xercesc::DOMNode * pNodeGlobal)
{
  parseNode(pNode);

  Chamber * csc_ = new Chamber(theCrate);

  std::string label, chamberType, problem;
  int problem_mask;

  if(fillString("label", label)) csc_->SetLabel(label);
  fillString("chamber_type", chamberType);
  if(fillString("known_problem", problem)) csc_->SetProblemDescription(problem);
  if(fillIntX("problem_location_mask", problem_mask)) csc_->SetProblemMask(problem_mask); 
  //
  std::cout << "Creating CSC Type=" << chamberType 
	    << " Label= " << label << std::endl;
  //
  xercesc::DOMNode * pNode1 = pNode->getFirstChild();
  //
  while (pNode1) {
    if (pNode1->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
      // std::cout << "PeripheralCrateParser: pNode1=" << xercesc::XMLString::transcode(pNode1->getNodeName()) << std::endl;
      //
      if (strcmp("DAQMB",xercesc::XMLString::transcode(pNode1->getNodeName()))==0) {  
	DAQMBParser(pNode1, theCrate, csc_);
      }
      //
      if (strcmp("TMB",xercesc::XMLString::transcode(pNode1->getNodeName()))==0) {  
	TMBParser(pNode1, theCrate, csc_, pNodeGlobal);
      }       
    }
    pNode1 = pNode1->getNextSibling();
  }
  //
}

  /******************************************************************************/
  /*                                                                            */
  /*  BGB 5-Jun-2008 moving crate controller parameters to a separate VCC tag   */
  /*  Attempting to maintain backward compatability with original xml elements  */
  /*  But old parameters will be depricated.                                    */
  /******************************************************************************/
// Crate * XMLParser::VMEParser(xercesc::DOMNode * pNode)
// {
//   int crateid=0, port; 
//   std::string  label, VMEaddress;

//   parseNode(pNode);
  
//   fillInt("crateID",crateid);
//   if(!fillString("VMEaddress",VMEaddress) )
//   {
//      std::cerr << "No valid VMEaddress" << std::endl;
//      return (Crate *) 0x0;
//   }

//   VMEController * controller = new VMEController(); 

//   controller->SetVMEAddress(VMEaddress);
//   if(fillInt("port",port)) controller->SetPort(port);
  
//   Crate * crate = new Crate(crateid,controller);

//   // if crate "label" missing, use "crateID" instead
//   if(!fillString("label",label)) fillString("crateID",label);  

//   crate->SetLabel(label);
//   std::cout << "Crate ID=" << crateid << " Label=" << label << std::endl;
  
//   // add VMECC(VMEModule in slot 1) to crate
//   VMECC * vcc = new VMECC(crate, 1);
//   std::cout << "VMECC in slot " << vcc->slot() << std::endl;
//   return crate;
// }

void XMLParser::PeripheralCrateParser(xercesc::DOMNode *pNode,EmuEndcap * endcap,xercesc::DOMNode *pNodeGlobal)
{ 
  int crateid=0, dep_port; 
  std::string  label, dep_VMEaddress;
  bool  found_dep_VMEaddress = false;
  bool  found_dep_port = false;
  bool  found_MAC_and_port = false;

  parseNode(pNode);
  
  if(!fillInt("crateID",crateid))
  {
     std::cerr << "No valid crateID" << std::endl;
     std::cerr<< "Failed to instantiate the crate, crate info discarded!" << std::endl;
     return;
  }

  if(fillString("VMEaddress",dep_VMEaddress) ) found_dep_VMEaddress = true;
  if(fillInt("port",dep_port))  found_dep_port = true;

  VMEController * controller = new VMEController(); 
  Crate * crate = new Crate(crateid,controller);

  // if crate "label" missing, use "crateID" instead
  if(!fillString("label",label)) fillString("crateID",label);  

  crate->SetLabel(label);
  std::cout << "Crate ID=" << crateid << " Label=" << label << std::endl;
  
//  Crate * crate = VMEParser(pNode);
  
//   if(crate==NULL) 
//   {
//      std::cerr<< "Failed to instantiate the crate, crate info discarded!" << std::endl;
//      return;
//   }

  endcap->addCrate(crate);
  
  xercesc::DOMNode * pNode3 = pNode->getFirstChild(); 
  if (pNode3==0) std::cout << " Bad element "<< std::endl;
  
  while(pNode3) {
    // the node was really a board of the peripheral crate like DMB, TMB, etc.
    if (pNode3->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {

#ifdef debugV
       std::cout <<"  "<< xercesc::XMLString::transcode(pNode3->getNodeName()) << std::endl;
#endif
    }
    
    if (strcmp("VCC",xercesc::XMLString::transcode(pNode3->getNodeName()))==0) {  
       found_MAC_and_port = VCCParser(pNode3, crate);
    }

    if (strcmp("CSC",xercesc::XMLString::transcode(pNode3->getNodeName()))==0) {  
       CSCParser(pNode3, crate,pNodeGlobal);
    }
    
    if (strcmp("CCB",xercesc::XMLString::transcode(pNode3->getNodeName()))==0) {  
       CCBParser(pNode3, crate);
    }
    
    if (strcmp("MPC",xercesc::XMLString::transcode(pNode3->getNodeName()))==0) {  
       MPCParser(pNode3, crate);
    }
    
    pNode3 = pNode3->getNextSibling();
    
  } // end of looping over boards (pNode3)
  if(!found_MAC_and_port){
    /* The new VCC parameters for MAC address and ethernet port were not found */
    /* so using deprecated values from PeripheralCrate element */
  // add VMECC(VMEModule in slot 1) to crate

    std::cout << "\nNo valid VCC MAC address information found" << std::endl;
    std::cout << "  Using depricated VMEaddress and port" << std::endl;
    VMECC * vcc = new VMECC(crate, 1);
    std::cout << "VMECC in slot " << vcc->slot() << std::endl;

    crate->vmeController()->SetVMEAddress(dep_VMEaddress);
    size_t pos=dep_VMEaddress.find(':');
    while(pos!=std::string::npos){
      dep_VMEaddress.replace(pos,1,1,'-');
      pos=dep_VMEaddress.find(':');
    }
    crate->vmeController()->SetMAC(0,dep_VMEaddress);
    crate->vmeController()->SetPort(dep_port);
  }
  std::cout <<"PeripheralCrateParser: finished looping over boards for crateID "<< crate->CrateID() << std::endl; 
}


EmuEndcap * XMLParser::EmuEndcapParser(xercesc::DOMNode *pNode)
{
  std::cout << "EmuEndcapParser" << std::endl;
  //
  EmuEndcap * endcap = new EmuEndcap();
  xercesc::DOMNode * pNodeGlobal = 0x0;
  //
  xercesc::DOMNode * pNode1 = pNode->getFirstChild();
  while (pNode1) {
    if (pNode1->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
      if (strcmp("Global",xercesc::XMLString::transcode(pNode1->getNodeName()))==0)
      {
	std::cout << "Found Global" << std::endl;
	//
	pNodeGlobal = pNode1->cloneNode(true);
	//
      }
      //
      if (strcmp("PeripheralCrate",xercesc::XMLString::transcode(pNode1->getNodeName()))==0)
      {
	 std::cout << "Found PeripheralCrate" << std::endl;
	 PeripheralCrateParser(pNode1,endcap,pNodeGlobal);
      }
    }
    pNode1 = pNode1->getNextSibling();
    //
  } // end of Top Element Loop, <EMU> only (pNode1)
  return endcap;
}

void XMLParser::parseFile(const std::string name)
{ 
  //
  /// Initialize XML4C system
  try{
    xercesc::XMLPlatformUtils::Initialize();
  }
  catch(const xercesc::XMLException& toCatch){
    std::cerr << "Error during Xerces-c Initialization.\n"
	      << "  Exception message:"
	      << xercesc::XMLString::transcode(toCatch.getMessage()) << std::endl;
    return ;
  }
 
  //  Create our parser, then attach an error handler to the parser.
  //  The parser will call back to methods of the ErrorHandler if it
  //  discovers errors during the course of parsing the XML document.
  //
  xercesc::XercesDOMParser *parser = new xercesc::XercesDOMParser;
  parser->setValidationScheme(xercesc::XercesDOMParser::Val_Auto);
  parser->setDoNamespaces(false);
  parser->setCreateEntityReferenceNodes(false);
  //parser->setToCreateXMLDeclTypeNode(true);
  
 
  //  Parse the XML file, catching any XML exceptions that might propogate
  //  out of it.
  //
  bool errorsOccured = false;
  try{
    parser->parse(name.c_str());
  }
  
  catch (const xercesc::XMLException& e){
    std::cerr << "An error occured during parsing\n   Message: "
	 << xercesc::XMLString::transcode(e.getMessage()) << std::endl;
    errorsOccured = true;
  }

 
  catch (const xercesc::DOMException& e){
    std::cerr << "An error occured during parsing\n   Message: "
	      << xercesc::XMLString::transcode(e.msg) << std::endl;
    errorsOccured = true;
  }

  catch (...){
    std::cerr << "An error occured during parsing" << std::endl;
    errorsOccured = true;
  }

  // If the parse was successful, output the document data from the DOM tree
  if (!errorsOccured){
    xercesc::DOMNode * pDoc = parser->getDocument();
    xercesc::DOMNode * pNode1 = pDoc->getFirstChild();
    while (pNode1) {
      if (pNode1->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
	if (strcmp("EmuSystem",xercesc::XMLString::transcode(pNode1->getNodeName()))==0
         || strcmp("EmuEndcap",xercesc::XMLString::transcode(pNode1->getNodeName()))==0)
        {
	  emuEndcap_ = EmuEndcapParser(pNode1);
	} else 
        {
	  std::cerr << "XMLParser.ERROR: First tag must be EmuEndcap or EmuSystem" << std::endl;
	}
      }
      pNode1 = pNode1->getNextSibling();
    } // end of Top Element Loop, <EMU> only (pNode1)
    //
  } //end of parsing config file
  
  //
  //  Clean up the error handler. The parser does not adopt handlers
  //  since they could be many objects or one object installed for multiple
  //  handlers.
  //

  //  Delete the parser itself.  Must be done prior to calling Terminate, below.
  delete parser;
  
  // And call the termination method
  xercesc::XMLPlatformUtils::Terminate();
  //
  //  The DOM document and its contents are reference counted, and need
  //  no explicit deletion.
  //
}
  } // namespace emu::pc
  } // namespace emu
