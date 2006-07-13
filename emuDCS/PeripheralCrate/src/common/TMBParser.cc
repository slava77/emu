//-----------------------------------------------------------------------
// $Id: TMBParser.cc,v 2.16 2006/07/13 15:46:37 mey Exp $
// $Log: TMBParser.cc,v $
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

TMBParser::TMBParser(xercesc::DOMNode * pNode, Crate * theCrate)
{
  parser_.parseNode(pNode);

  int slot = 0;
  parser_.fillInt("slot", slot);
  if(slot == 0) {
    std::cerr << "No slot specified for TMB! " << std::endl;
  } else {
    tmb_ = new TMB(theCrate, slot);
    //
    tmb_->SendOutput("New TMB");
    //
    int delay;
    if ( parser_.fillInt("cfeb0delay",delay) ) {
      tmb_->SetCFEB0delay(delay);
    }
    if ( parser_.fillInt("cfeb1delay",delay) ) {
      tmb_->SetCFEB1delay(delay);
    }
    if ( parser_.fillInt("cfeb2delay",delay) ) {
      tmb_->SetCFEB2delay(delay);
    }
    if ( parser_.fillInt("cfeb3delay",delay) ) {
      tmb_->SetCFEB3delay(delay);
    }
    if ( parser_.fillInt("cfeb4delay",delay) ) {
      tmb_->SetCFEB4delay(delay);
    }
    //
    if ( parser_.fillInt("alct_tx_clock_delay", delay)) {
      tmb_->SetAlctTXclockDelay(delay);
    }
    if ( parser_.fillInt("alct_rx_clock_delay", delay) ) {
      tmb_->SetAlctRXclockDelay(delay);
    }
    //
    if ( parser_.fillInt("l1adelay", delay) ) {
      tmb_->SetL1aDelay(delay);
    }
    if ( parser_.fillInt("l1a_window_size", delay) ) {
      tmb_->SetL1aWindowSize(delay);
    }
    int size;
    if ( parser_.fillInt("alct_match_window_size", size) ) {
      tmb_->SetAlctMatchWindowSize(size);
    }
    if ( parser_.fillInt("alct_vpf_delay", delay) ) {
      tmb_->SetAlctVpfDelay(delay);
    }
    std::string version;
    if ( parser_.fillString("version", version) ) {
      tmb_->SetVersion(version);
    }
    if ( parser_.fillInt("mpc_delay", delay) ) {
      tmb_->SetMpcDelay(delay);
    }
    int input;
    if ( parser_.fillInt("ALCT_input",input) ) {
      tmb_->SetAlctInput(input);
    }
    //
    int exist;
    if ( parser_.fillInt("rpc_exists",exist) ) {
      tmb_->SetRpcExist(exist);
    }
    //
    int mode;
    if ( parser_.fillInt("fifo_mode",mode) ) {
      tmb_->SetFifoMode(mode);
    }
    //
    int tbins;
    if ( parser_.fillInt("fifo_tbins",tbins)) {
      tmb_->SetFifoTbins(tbins);
    }
    //
    int pretrig;
    if ( parser_.fillInt("fifo_pretrig",pretrig)) {
      tmb_->SetFifoPreTrig(pretrig);
    }
    //
    int clear;
    if ( parser_.fillInt("alct_clear",clear)) {
      tmb_->SetAlctClear(clear);
    }
    //
    if ( parser_.fillInt("mpc_tx_delay",delay)) {
      tmb_->SetMpcTXdelay(delay);
    }
    //
    int offset;
    if ( parser_.fillInt("l1a_offset",offset) ) {
      tmb_->SetL1aOffset(offset);
    }
    //
    if ( parser_.fillInt("bxn_offset",offset)) {
      tmb_->SetBxnOffset(offset);
    }
    //
    int disable;
    if ( parser_.fillInt("disableCLCTInputs",disable)) {
      tmb_->SetDisableCLCTInputs(disable);
    }
    //
    int enable;
    if ( parser_.fillInt("enableCLCTInputs",enable)) {
      tmb_->SetEnableCLCTInputs(enable);
    }
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
	//RAT
       	//alctParser_.fillInt("rpc_rat_delay", rat_->rat_tmb_delay_);
	//ALCT
	//
	int mode;
       	if ( alctParser_.fillInt("trig_mode", mode) ) {
	  alct_->SetTrigMode(mode);
	}
	//
	if ( alctParser_.fillInt("ext_trig_en", enable)) {
	  alct_->SetExtTrigEnable(enable);
	}
	//
	if ( alctParser_.fillInt("trig_info_en", enable)) {
	  alct_->SetTrigInfoEnable(enable);
	}
	if ( alctParser_.fillInt("l1a_internal", enable)) {
	  alct_->SetL1aInternal(enable);
	}
	if ( alctParser_.fillInt("fifo_tbins", tbins) ) {
	  alct_->SetFifoTbins(tbins);
	}
	if ( alctParser_.fillInt("fifo_pretrig", pretrig)) {
	  alct_->SetFifoPretrig(pretrig);
	}
	if (alctParser_.fillInt("l1a_delay", delay)) {
	  alct_->SetL1aDelay(delay);
	}
	if ( alctParser_.fillInt("l1a_offset",offset)) {
	  alct_->SetL1aOffset(offset);
	}
	if (alctParser_.fillInt("l1a_window", size)) {
	  alct_->SetL1aWindowSize(size);
	}
	int nph;
	if ( alctParser_.fillInt("nph_thresh", nph)) {
	  alct_->SetPretrigNumberOfLayers(nph);
	}
	if ( alctParser_.fillInt("nph_pattern", nph)){
	  alct_->SetPretrigNumberOfPattern(nph);
	}
	if ( alctParser_.fillInt("ccb_enable", enable) ) {
	  alct_->SetCCBEnable(enable);
	}
	if ( alctParser_.fillInt("inject_mode", mode)) {
	  alct_->SetAlctInjectMode(mode);
	}
	if ( alctParser_.fillInt("send_empty", enable)) {
	  alct_->SetSendEmpty(enable);
	}
	if (alctParser_.fillInt("drift_delay", delay)) {
	  alct_->SetDriftDelay(delay);
	}
	std::string file;
	if ( alctParser_.fillString("alct_pattern_file", file)) {
	  alct_->SetPatternFile(file);
	}
	if (alctParser_.fillString("alct_hotchannel_file", file)){
	  alct_->SetHotChannelFile(file);
	}
	
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
	      anodeParser_.fillInt("delay", delay);
	      anodeParser_.fillInt("threshold", threshold);
	      //
	      alct_->SetDelay(number-1,delay);
	      //alct_->delays_[number-1] = delay;
	      //alct_->thresholds_[number-1] = threshold;
	      alct_->SetThreshold(number-1,threshold);
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
TMBParser::~TMBParser(){
  //  delete tmb_;
}
