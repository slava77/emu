//-----------------------------------------------------------------------
// $Id: TMBParser.cc,v 3.17 2007/04/19 16:09:18 rakness Exp $
// $Log: TMBParser.cc,v $
// Revision 3.17  2007/04/19 16:09:18  rakness
// add accel pretrig/pattern to ALCT config reg
//
// Revision 3.16  2007/04/10 13:31:02  rakness
// add mpc_output_enable, remove rpc2/3
//
// Revision 3.15  2007/03/28 17:46:22  rakness
// xml changes:  add ALCT testpulse, remove TTCrxID
//
// Revision 3.14  2007/03/14 08:59:03  rakness
// make parser dumb
//
// Revision 3.13  2007/03/01 18:07:26  rakness
// changes to xml: add dmb_tx_delay, change match_pretrig(trig)_match_window_size->match_pretrig(trig)_window_size
//
// Revision 3.12  2007/01/31 16:50:07  rakness
// complete set of TMB/ALCT/RAT xml parameters
//
// Revision 3.11  2006/11/15 16:01:37  mey
// Cleaning up code
//
// Revision 3.10  2006/11/09 08:47:51  rakness
// add rpc0_raw_delay to xml file
//
// Revision 3.9  2006/10/20 13:09:54  mey
// UPdate
//
// Revision 3.8  2006/10/19 09:42:03  rakness
// remove old ALCTController
//
// Revision 3.7  2006/10/13 15:34:39  rakness
// add mpc_phase
//
// Revision 3.6  2006/10/06 12:15:40  rakness
// expand xml file
//
// Revision 3.5  2006/10/03 07:39:16  mey
// UPdate
//
// Revision 3.4  2006/09/19 14:32:45  mey
// Update
//
// Revision 3.3  2006/09/13 14:13:33  mey
// Update
//
// Revision 3.2  2006/08/17 15:03:16  mey
// Modified Parser to accept globals
//
// Revision 3.1  2006/08/09 11:57:04  mey
// Got rid of version
//
// Revision 3.0  2006/07/20 21:15:48  geurts
// *** empty log message ***
//
// Revision 2.18  2006/07/18 14:13:20  mey
// Update
//
// Revision 2.17  2006/07/14 11:46:31  rakness
// compiler switch possible for ALCTNEW
//
// Revision 2.16  2006/07/13 15:46:37  mey
// New Parser strurture
//
// Revision 2.15  2006/06/21 08:30:48  mey
// Update
//
// Revision 2.14  2006/06/20 13:18:17  mey
// Update
//
// Revision 2.13  2006/06/20 12:34:10  mey
// Get Output
//
// Revision 2.12  2006/05/19 14:06:46  mey
// Fixed ALCT parser
//
// Revision 2.11  2006/04/06 08:54:32  mey
// Got rif of friend TMBParser
//
// Revision 2.10  2006/03/30 13:55:38  mey
// Update
//
// Revision 2.9  2006/03/20 09:10:43  mey
// Update
//
// Revision 2.8  2006/03/10 15:55:28  mey
// Update
//
// Revision 2.7  2006/03/08 22:53:12  mey
// Update
//
// Revision 2.6  2006/03/05 18:45:08  mey
// Update
//
// Revision 2.5  2006/03/03 07:59:20  mey
// Update
//
// Revision 2.4  2006/02/25 11:25:11  mey
// UPdate
//
// Revision 2.3  2005/08/31 15:12:58  mey
// Bug fixes, updates and new routine for timing in DMB
//
// Revision 2.2  2005/08/15 15:38:02  mey
// Include alct_hotchannel_file
//
// Revision 2.1  2005/06/06 15:17:18  geurts
// TMB/ALCT timing updates (Martin vd Mey)
//
// Revision 2.0  2005/04/12 08:07:06  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#include <iostream>
#include "TMBParser.h"
#include "TMB.h"
#include "AnodeChannel.h"
#include <xercesc/dom/DOM.hpp>
#include "ALCTController.h"
#include "RAT.h"

TMBParser::TMBParser(xercesc::DOMNode * pNode, Crate * theCrate, Chamber * theChamber, xercesc::DOMNode * pNodeGlobal)
{

  if (pNodeGlobal) {
    parserGlobal_.parseNode(pNodeGlobal);
  }

  parser_.parseNode(pNode);

  int slot = 0;
  parser_.fillInt("slot", slot);
  std::cout << "Inside TMBParser..."<<std::endl;
  if(slot == 0) {
    std::cerr << "No slot specified for TMB! " << std::endl;
  } else {
    //
    tmb_ = new TMB(theCrate, theChamber, slot);
    //
    // need still to put in 
    //   . hot channel mask
    //   . ddd_oe mask
    int value;
    //////////////////////////////
    // Expected Firmware tags:
    //////////////////////////////
    if (parser_.fillInt("tmb_firmware_month"  ,value)) { tmb_->SetExpectedTmbFirmwareMonth(value);   }
    if (parser_.fillInt("tmb_firmware_day"    ,value)) { tmb_->SetExpectedTmbFirmwareDay(value);     }
    if (parser_.fillInt("tmb_firmware_year"   ,value)) { tmb_->SetExpectedTmbFirmwareYear(value);    }
    if (parser_.fillInt("tmb_firmware_version",value)) { tmb_->SetExpectedTmbFirmwareVersion(value); }
    if (parser_.fillInt("tmb_firmware_revcode",value)) { tmb_->SetExpectedTmbFirmwareRevcode(value); }
    if (parser_.fillInt("tmb_firmware_type"   ,value)) { tmb_->SetExpectedTmbFirmwareType(value);    }
    //
    if (parser_.fillInt("rat_firmware_month"  ,value)) { tmb_->SetExpectedRatFirmwareMonth(value); }
    if (parser_.fillInt("rat_firmware_day"    ,value)) { tmb_->SetExpectedRatFirmwareDay(value);   }
    if (parser_.fillInt("rat_firmware_year"   ,value)) { tmb_->SetExpectedRatFirmwareYear(value);  }
    //
    //////////////////////////////
    // Enable inputs
    //////////////////////////////
    //0x0E:
    if (parser_.fillInt("enable_alct_tx",value)) { tmb_->SetEnableAlctTx(value); }
    if (parser_.fillInt("enable_alct_rx",value)) { tmb_->SetAlctInput(value);    }
    //    
    //0X32:
    if (parser_.fillInt("alct_clear"     ,value)) { tmb_->SetAlctClear(value);                  }
    //if (parser_.fillInt("alct_inject_mux",value)) { tmb_->SetAlctInject(value);                 }
    //if (parser_.fillInt("alct_sync_clct" ,value)) { tmb_->SetSyncAlctInjectToClctInject(value); }
    //if (parser_.fillInt("alct_inj_delay" ,value)) { tmb_->SetAlctInjectorDelay(value);          }
    //
    //0X42:
    if (parser_.fillInt("enableCLCTInputs_reg42",value)) { tmb_->SetEnableCLCTInputs(value);     }
    //if (parser_.fillInt("cfeb_ram_sel"          ,value)) { tmb_->SetSelectCLCTRAM(value);        }
    //if (parser_.fillInt("cfeb_inj_en_sel"       ,value)) { tmb_->SetEnableCLCTInject(value);     }
    //if (parser_.fillInt("start_pattern_inj"     ,value)) { tmb_->SetStartPatternInjector(value); }
    //
    //0XB6:
    if (parser_.fillInt("rpc_exists"     ,value)) { tmb_->SetRpcExist(value);           }
    if (parser_.fillInt("rpc_read_enable",value)) { tmb_->SetRpcReadEnable(value);      }
    if (parser_.fillInt("rpc_bxn_offset" ,value)) { tmb_->SetRpcBxnOffset(value);       }    
    //if (parser_.fillInt("rpc_bank_addr"  ,value)) { tmb_->SetRpcSyncBankAddress(value); }
    //
    //0xBC:
    if (parser_.fillInt("rpc_mask_all" ,value)) { tmb_->SetEnableRpcInput(value);           }
    //if (parser_.fillInt("inj_mask_rat" ,value)) { tmb_->SetInjectorMaskRat(value);          }
    //if (parser_.fillInt("inj_mask_rpc" ,value)) { tmb_->SetInjectorMaskRpc(value);          }
    //if (parser_.fillInt("inj_delay_rat",value)) { tmb_->SetInjectorDelayRat(value);         }
    //if (parser_.fillInt("rpc_inj_sel"  ,value)) { tmb_->SetRpcInjector(value);              }
    //if (parser_.fillInt("rpc_inj_wdata",value)) { tmb_->SetRpcInjectorWriteDataMSBs(value); }
    //
    //////////////////////////////
    // trigger and signal delays
    //////////////////////////////
    //0X6A:
    if (parser_.fillInt("match_pretrig_window_size",value)) { tmb_->SetAlctClctPretrigWidth(value); }
    if (parser_.fillInt("match_pretrig_alct_delay" ,value)) { tmb_->SetAlctPretrigDelay(value);     }
    //if (parser_.fillInt("alct_pat_trig_delay"      ,value)) { tmb_->SetAlctPatternDelay(value);     }
    //if (parser_.fillInt("adb_ext_trig_delay"       ,value)) { tmb_->SetAdbExternalTrigDelay(value); }
    //
    //0X6C:
    //if (parser_.fillInt("dmb_ext_trig_delay" ,value)) { tmb_->SetDmbExternalTrigDelay(value);  }
    //if (parser_.fillInt("clct_ext_trig_delay",value)) { tmb_->SetClctExternalTrigDelay(value); }
    //if (parser_.fillInt("alct_ext_trig_delay",value)) { tmb_->SetAlctExternalTrigDelay(value); }
    if (parser_.fillInt("layer_trig_delay"   ,value)) { tmb_->SetLayerTrigDelay(value);        }
    //
    //0X74:
    if (parser_.fillInt("tmb_l1a_delay"       ,value)) { tmb_->SetL1aDelay(value);      }
    if (parser_.fillInt("tmb_l1a_window_size" ,value)) { tmb_->SetL1aWindowSize(value); }
    //if (parser_.fillInt("tmb_l1a_internal"    ,value)) { tmb_->SetInternalL1a(value);   }
    //
    //0X76:
    if (parser_.fillInt("tmb_l1a_offset",value)) { tmb_->SetL1aOffset(value); }
    if (parser_.fillInt("tmb_bxn_offset",value)) { tmb_->SetBxnOffset(value); }
    //
    //0XB2:
    if (parser_.fillInt("match_trig_alct_delay" ,value)) { tmb_->SetAlctVpfDelay(value);        }
    if (parser_.fillInt("match_trig_window_size",value)) { tmb_->SetAlctMatchWindowSize(value); }
    if (parser_.fillInt("mpc_tx_delay"          ,value)) { tmb_->SetMpcTXdelay(value);          }
    //
    //0XBA:
    if (parser_.fillInt("rpc0_raw_delay",value)) { tmb_->SetRpc0RawDelay(value); }
    if (parser_.fillInt("rpc1_raw_delay",value)) { tmb_->SetRpc1RawDelay(value); }
    //if (parser_.fillInt("rpc2_raw_delay",value)) { tmb_->SetRpc2RawDelay(value); }
    //if (parser_.fillInt("rpc3_raw_delay",value)) { tmb_->SetRpc3RawDelay(value); }
    //
    //
    //////////////////////////////
    // trigger configuration
    //////////////////////////////
    //if (parser_.fillInt("trgmode",value)) { tmb_->SetTrgMode(value); }
    //
    //0X68:
    if (parser_.fillInt("clct_pretrig_enable"    ,value)) { tmb_->SetClctPatternTrigEnable(value);  }
    if (parser_.fillInt("alct_pretrig_enable"    ,value)) { tmb_->SetAlctPatternTrigEnable(value);  }
    if (parser_.fillInt("match_pretrig_enable"   ,value)) { tmb_->SetMatchPatternTrigEnable(value); }
    //if (parser_.fillInt("adb_ext_pretrig_enable" ,value)) { tmb_->SetAdbExtTrigEnable(value);       }
    //if (parser_.fillInt("dmb_ext_pretrig_enable" ,value)) { tmb_->SetDmbExtTrigEnable(value);       }
    //if (parser_.fillInt("clct_ext_pretrig_enable",value)) { tmb_->SetClctExtTrigEnable(value);      }
    //if (parser_.fillInt("alct_ext_pretrig_enable",value)) { tmb_->SetAlctExtTrigEnable(value);      }
    //if (parser_.fillInt("ext_trig_inject"        ,value)) { tmb_->SetExtTrigInject(value);          }
    if (parser_.fillInt("all_cfeb_active"        ,value)) { tmb_->SetEnableAllCfebsActive(value);   }
    if (parser_.fillInt("enableCLCTInputs_reg68" ,value)) { tmb_->SetCfebEnable(value);             }
    if (parser_.fillInt("cfeb_enable_source"     ,value)) { tmb_->Set_cfeb_enable_source(value);    }
    //
    //0X70:
    if (parser_.fillInt("triad_persistence"            ,value)) { tmb_->SetTriadPersistence(value); }
    if (parser_.fillInt("clct_halfstrip_pretrig_thresh",value)) { tmb_->SetHsPretrigThresh(value);  }
    if (parser_.fillInt("clct_distrip_pretrig_thresh"  ,value)) { tmb_->SetDsPretrigThresh(value);  }
    if (parser_.fillInt("clct_pattern_thresh"          ,value)) { tmb_->SetMinHitsPattern(value);   }
    if (parser_.fillInt("clct_drift_delay"             ,value)) { tmb_->SetDriftDelay(value);       }
    //if (parser_.fillInt("pretrigger_halt"              ,value)) { tmb_->SetPretriggerHalt(value);   }
    //
    //0X72
    if (parser_.fillInt("tmb_fifo_mode"   ,value)) { tmb_->SetFifoMode(value);    }
    if (parser_.fillInt("tmb_fifo_tbins"  ,value)) { tmb_->SetFifoTbins(value);   }
    if (parser_.fillInt("tmb_fifo_pretrig",value)) { tmb_->SetFifoPreTrig(value); }
    //
    //0X86
    if (parser_.fillInt("mpc_sync_err_enable",value)) { tmb_->SetTmbSyncErrEnable(value); }
    if (parser_.fillInt("alct_trig_enable"   ,value)) { tmb_->SetTmbAllowAlct(value);     }
    if (parser_.fillInt("clct_trig_enable"   ,value)) { tmb_->SetTmbAllowClct(value);     }
    if (parser_.fillInt("match_trig_enable"  ,value)) { tmb_->SetTmbAllowMatch(value);    }
    if (parser_.fillInt("mpc_rx_delay"       ,value)) { tmb_->SetMpcDelay(value);         }
    //if (parser_.fillInt("mpc_sel_ttc_bx0"    ,value)) { tmb_->SetSelectMpcTtcBx0(value);  }
    if (parser_.fillInt("mpc_idle_blank"     ,value)) { tmb_->SetMpcIdleBlank(value);     }
    if (parser_.fillInt("mpc_output_enable"  ,value)) { tmb_->SetMpcOutputEnable(value);  }
    //
    //0XAC
    //if (parser_.fillInt("clct_flush_delay"   ,value)) { tmb_->SetClctFlushDelay(value);        }
    //if (parser_.fillInt("clct_turbo"         ,value)) { tmb_->SetClctTurbo(value);             }
    //if (parser_.fillInt("ranlct_enable"      ,value)) { tmb_->SetRandomLctEnable(value);       }
    if (parser_.fillInt("write_buffer_required",value)) { tmb_->SetWriteBufferRequired(value);   }
    if (parser_.fillInt("valid_clct_required"  ,value)) { tmb_->SetRequireValidClct(value);      }
    if (parser_.fillInt("l1a_allow_match"      ,value)) { tmb_->SetL1aAllowMatch(value);         }
    if (parser_.fillInt("l1a_allow_notmb"      ,value)) { tmb_->SetL1aAllowNoTmb(value);         }
    if (parser_.fillInt("l1a_allow_nol1a"      ,value)) { tmb_->SetL1aAllowNoL1a(value);         }
    if (parser_.fillInt("l1a_allow_alct_only"  ,value)) { tmb_->SetL1aAllowAlctOnly(value);      }
    //if (parser_.fillInt("scint_veto_clr"     ,value)) { tmb_->SetScintillatorVetoClear(value); }
    //
    //0XF0
    if (parser_.fillInt("layer_trig_enable",value)) { tmb_->SetEnableLayerTrigger(value);    }
    if (parser_.fillInt("layer_trig_thresh",value)) { tmb_->SetLayerTriggerThreshold(value); }
    //
    //
    ///////////////////////////////////
    // special configuration registers
    ///////////////////////////////////
    //0X1C
    // put int ddd_oe...
    //0X2C
    //if (parser_.fillInt("alct_ext_trig_l1aen"   ,value)) { tmb_->SetEnableL1aRequestOnAlctExtTrig(value); }
    //if (parser_.fillInt("clct_ext_trig_l1aen"   ,value)) { tmb_->SetEnableL1aRequestOnClctExtTrig(value); }
    if (parser_.fillInt("request_l1a"           ,value)) { tmb_->SetRequestL1a(value);                    }
    //if (parser_.fillInt("alct_ext_trig_vme"     ,value)) { tmb_->SetAlctExtTrigVme(value);                }
    //if (parser_.fillInt("clct_ext_trig_vme"     ,value)) { tmb_->SetClctExtTrigVme(value);                }
    //if (parser_.fillInt("ext_trig_both"         ,value)) { tmb_->SetExtTrigBoth(value);                   }
    //if (parser_.fillInt("ccb_allow_bypass"      ,value)) { tmb_->SetCcbAllowExternalBypass(value);        }
    //if (parser_.fillInt("internal_l1a_delay_vme",value)) { tmb_->SetInternalL1aDelay(value);              }
    //
    //0X30
    //if (parser_.fillInt("cfg_alct_ext_trig_en"  ,value)) { tmb_->SetEnableAlctExtTrig(value);     }
    //if (parser_.fillInt("cfg_alct_ext_inject_en",value)) { tmb_->SetEnableAlctExtInject(value);   }
    //if (parser_.fillInt("cfg_alct_ext_trig"     ,value)) { tmb_->SetAlctExtTrig(value);           }
    //if (parser_.fillInt("cfg_alct_ext_inject"   ,value)) { tmb_->SetAlctExtInject(value);         }
    //if (parser_.fillInt("alct_seq_cmd"          ,value)) { tmb_->SetAlctSequencerCommand(value);  }
    if (parser_.fillInt("alct_clock_en_use_ccb" ,value)) { tmb_->SetEnableAlctUseCcbClock(value); }
    //if (parser_.fillInt("alct_clock_en_use_vme" ,value)) { tmb_->SetAlctClockVme(value);          }
    //
    //0X6E
    //if (parser_.fillInt("slot"  ,value)) { tmb_->SetTmbSlot(value); }
    //if (parser_.fillInt("csc_id",value)) { tmb_->SetCscId(value);   }
    //if (parser_.fillInt("run_id",value)) { tmb_->SetRunId(value);   }
    //
    //if (parser_.fillInt("disableCLCTInputs",value)) { tmb_->SetDisableCLCTInputs(value); }
    //if (parser_.fillInt("shift_rpc",value)) { tmb_->SetShiftRpc(value); }
    //
    ///////////////////////////////////
    // clock phases
    ///////////////////////////////////
    //0X16
    if (parser_.fillInt("alct_tx_clock_delay",value)) { tmb_->SetAlctTXclockDelay(value); }
    if (parser_.fillInt("alct_rx_clock_delay",value)) { tmb_->SetAlctRXclockDelay(value); }
    if (parser_.fillInt("dmb_tx_delay"       ,value)) { tmb_->SetDmbTxDelay(value);       }
    if (parser_.fillInt("rat_tmb_delay"      ,value)) { tmb_->SetRatTmbDelay(value);      }
    //
    //0X18
    //if (parser_.fillInt("tmb1_phase",value)) { tmb_->SetTmb1Phase(value);  }
    //if (parser_.fillInt("mpc_phase" ,value)) { tmb_->SetMpcPhase(value);   }
    //if (parser_.fillInt("dcc_phase" ,value)) { tmb_->SetDccPhase(value);   }
    if (parser_.fillInt("cfeb0delay",value)) { tmb_->SetCFEB0delay(value); }
    //
    //0X1A
    if (parser_.fillInt("cfeb1delay",value)) { tmb_->SetCFEB1delay(value); }
    if (parser_.fillInt("cfeb2delay",value)) { tmb_->SetCFEB2delay(value); }
    if (parser_.fillInt("cfeb3delay",value)) { tmb_->SetCFEB3delay(value); }
    if (parser_.fillInt("cfeb4delay",value)) { tmb_->SetCFEB4delay(value); }
    //
    //0XE6
    if (parser_.fillInt("rpc0_rat_delay",value)) { tmb_->SetRpc0RatDelay(value); }
    if (parser_.fillInt("rpc1_rat_delay",value)) { tmb_->SetRpc1RatDelay(value); }
    //if (parser_.fillInt("rpc2_rat_delay",value)) { tmb_->SetRpc2RatDelay(value); }
    //if (parser_.fillInt("rpc3_rat_delay",value)) { tmb_->SetRpc3RatDelay(value); }
    //
    //
    ////////
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
	EmuParser alctParser_;
	//
	alctParser_.parseNode(daughterNode);
	
        std::string chamberType;
        alctParser_.fillString("chamber_type", chamberType);
        //alct_ memory area is deleted in ~TMB()
        ALCTController * alct_ = new ALCTController(tmb_, chamberType);
	RAT * rat_ = new RAT(tmb_);
	tmb_->SetAlct(alct_); //store alct_ pointer in tmb_
	tmb_->SetRat(rat_); //store rat_  in tmb_
	//
	//RAT
       	//if(alctParser_.fillInt("rpc0_rat_delay",value)) { rat_->SetRpcRatDelay(0,value); }
	//
	//ALCT
	// need to put in 
	//   . standby register
	//   . hot channel mask
	//   . collision mask
	//
	////////////////////////////////////////////////////////
	// Expected firmware tags
	////////////////////////////////////////////////////////
	if (alctParser_.fillInt("alct_firmware_day"  ,value)) { alct_->SetExpectedFastControlDay(value);   }
	if (alctParser_.fillInt("alct_firmware_month",value)) { alct_->SetExpectedFastControlMonth(value); }
	if (alctParser_.fillInt("alct_firmware_year" ,value)) { alct_->SetExpectedFastControlYear(value);  }
	//
	std::string alct_firmware_backwardForward;
        if (alctParser_.fillString("alct_firmware_backward_forward", alct_firmware_backwardForward) ){
	  alct_->Set_fastcontrol_backward_forward_type(alct_firmware_backwardForward);
	}
	std::string alct_firmware_negativePositive;
        if (alctParser_.fillString("alct_firmware_negative_positive", alct_firmware_negativePositive) ){
	  alct_->Set_fastcontrol_negative_positive_type(alct_firmware_negativePositive);
	}
	//
	////////////////////////////////////////////////////////
	// configuration register
	////////////////////////////////////////////////////////
	if (alctParser_.fillInt("alct_trig_mode"      ,value)) { alct_->SetTriggerMode(value);            }
	if (alctParser_.fillInt("alct_ext_trig_enable",value)) { alct_->SetExtTrigEnable(value);          }
	if (alctParser_.fillInt("alct_send_empty"     ,value)) { alct_->SetSendEmpty(value);              }
	if (alctParser_.fillInt("alct_inject_mode"    ,value)) { alct_->SetInjectMode(value);             }
	if (alctParser_.fillInt("alct_bxn_offset"     ,value)) { alct_->SetBxcOffset(value);              }
	if (alctParser_.fillInt("alct_pretrig_thresh" ,value)) { alct_->SetPretrigNumberOfLayers(value);  }
	if (pNodeGlobal){
	  if (parserGlobal_.fillInt("alct_pretrig_thresh",value)) { alct_->SetPretrigNumberOfLayers(value); }
	}
	if (alctParser_.fillInt("alct_pattern_thresh"      ,value)) { alct_->SetPretrigNumberOfPattern(value);   }
	if (alctParser_.fillInt("alct_accel_pretrig_thresh",value)) { alct_->SetAcceleratorPretrigThresh(value); }
	if (alctParser_.fillInt("alct_accel_pattern_thresh",value)) { alct_->SetAcceleratorPatternThresh(value); }
	if (alctParser_.fillInt("alct_drift_delay"         ,value)) { alct_->SetDriftDelay(value);               }
	if (alctParser_.fillInt("alct_fifo_tbins"          ,value)) { alct_->SetFifoTbins(value);                }
	if (alctParser_.fillInt("alct_fifo_pretrig"        ,value)) { alct_->SetFifoPretrig(value);              }
	if (alctParser_.fillInt("alct_fifo_mode"           ,value)) { alct_->SetFifoMode(value);                 }
	if (alctParser_.fillInt("alct_l1a_delay"           ,value)) { alct_->SetL1aDelay(value);                 }
	if (alctParser_.fillInt("alct_l1a_window_size"     ,value)) { alct_->SetL1aWindowSize(value);            }
	if (alctParser_.fillInt("alct_l1a_offset"          ,value)) { alct_->SetL1aOffset(value);                }
	if (alctParser_.fillInt("alct_l1a_internal"        ,value)) { alct_->SetL1aInternal(value);	         }
	if (alctParser_.fillInt("alct_ccb_enable"          ,value)) { alct_->SetCcbEnable(value);                }
	if (alctParser_.fillInt("alct_config_in_readout"   ,value)) { alct_->SetConfigInReadout(value);          }
	if (alctParser_.fillInt("alct_accel_mode"          ,value)) { alct_->SetAlctAmode(value);                }
	if (alctParser_.fillInt("alct_trig_info_en"        ,value)) { alct_->SetTriggerInfoEnable(value);        }
	if (alctParser_.fillInt("alct_sn_select"           ,value)) { alct_->SetSnSelect(value);                 }
	if (alctParser_.fillInt("alct_testpulse_amplitude" ,value)) { alct_->SetTestpulseAmplitude(value);       }
	std::string on_or_off;
	if (alctParser_.fillString("alct_testpulse_invert",on_or_off)) { alct_->Set_InvertPulse(on_or_off); }
	std::string afebs_or_strips;
	if (alctParser_.fillString("alct_testpulse_direction",afebs_or_strips)) { alct_->Set_PulseDirection(afebs_or_strips); }
	//	std::string file;
	//	if ( alctParser_.fillString("alct_pattern_file", file)) {
	//	  alct_->SetPatternFile(file);
	//	}
	//	if (alctParser_.fillString("alct_hotchannel_file", file)){
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
	    EmuParser anodeParser_;
	    //
	    anodeParser_.parseNode(grandDaughterNode);
	    if(anodeParser_.fillInt("Number", number)){
	      anodeParser_.fillInt("afeb_fine_delay", delay);
	      anodeParser_.fillInt("afeb_threshold", threshold);
	      //
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
  std::cout << "Done TMBparser" << std::endl;
#ifdef debugV  
  std::cout << "finished TMBParsing" << std::endl;
#endif
}
TMBParser::~TMBParser(){
  //  delete tmb_;
}
