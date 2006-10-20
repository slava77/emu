//-----------------------------------------------------------------------
// $Id: TMBParser.cc,v 3.9 2006/10/20 13:09:54 mey Exp $
// $Log: TMBParser.cc,v $
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

TMBParser::TMBParser(xercesc::DOMNode * pNode, Crate * theCrate,xercesc::DOMNode * pNodeGlobal)
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
    std::cout << "Creating TMB" <<std::endl;
    tmb_ = new TMB(theCrate, slot);
    std::cout << "Leaving  TMB" <<std::endl;
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
    int trgmode;
    if ( parser_.fillInt("trgmode",trgmode)) {               //add
      tmb_->SetTrgMode(trgmode);
    }
    //
    int rpc_bxn_offset;
    if ( parser_.fillInt("rpc_bxn_offset",rpc_bxn_offset)) { //add
      tmb_->SetRpcBxnOffset(rpc_bxn_offset);
    }
    //
    int shift_rpc;
    if ( parser_.fillInt("shift_rpc",shift_rpc)) {           //add
      tmb_->SetShiftRpc(shift_rpc);
    }
    //
    int request_l1a;
    if ( parser_.fillInt("request_l1a",request_l1a)) {       //add
      tmb_->SetRequestL1a(request_l1a);
    }
    //
    int hs_pretrig_thresh;
    if ( parser_.fillInt("hs_pretrig_thresh",hs_pretrig_thresh)) { //add
      tmb_->SetHsPretrigThresh(hs_pretrig_thresh);
    }
    //
    int ds_pretrig_thresh;
    if ( parser_.fillInt("ds_pretrig_thresh",ds_pretrig_thresh)) { //add
      tmb_->SetDsPretrigThresh(ds_pretrig_thresh);
    }
    //
    int min_hits_pattern;
    if ( parser_.fillInt("min_hits_pattern",min_hits_pattern)) { //add
      tmb_->SetMinHitsPattern(min_hits_pattern);
    }
    //
    int dmb_tx_delay;
    if ( parser_.fillInt("dmb_tx_delay",dmb_tx_delay)) {  //add
      tmb_->SetDmbTxDelay(dmb_tx_delay);
    }
    //
    int rat_tmb_delay;
    if ( parser_.fillInt("rat_tmb_delay",rat_tmb_delay)) {  //add
      tmb_->SetRatTmbDelay(rat_tmb_delay);
    }
    //
    int rpc0_rat_delay;
    if ( parser_.fillInt("rpc0_rat_delay",rpc0_rat_delay)) {  //add
      tmb_->SetRpc0RatDelay(rpc0_rat_delay);
    }
    //
    int mpc_phase;
    if ( parser_.fillInt("mpc_phase",mpc_phase)) {  //add
      tmb_->SetMpcPhase(mpc_phase);
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
	//
	//RAT
       	if(alctParser_.fillInt("rpc0_rat_delay",delay)){
	  rat_->SetRpcRatDelay(0,delay);
	}
	//
	//ALCT
	//
	//
	int mode;
	if ( alctParser_.fillInt("trig_mode", mode) ) {
	  alct_->SetTriggerMode(mode);
	}
	//
	if ( alctParser_.fillInt("ext_trig_en", enable)) {
	  alct_->SetExtTrigEnable(enable);
	}
	//
	if ( alctParser_.fillInt("trig_info_en", enable)) {
	  alct_->SetTriggerInfoEnable(enable);
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
	if ( alctParser_.fillInt("nph_thresh", nph) ){
	  alct_->SetPretrigNumberOfLayers(nph);
	}
	if (pNodeGlobal){
	  if ( parserGlobal_.fillInt("nph_thresh", nph) ){
	    alct_->SetPretrigNumberOfLayers(nph);
	  }
	}
	if ( alctParser_.fillInt("nph_pattern", nph)){
	  alct_->SetPretrigNumberOfPattern(nph);
	}
	if ( alctParser_.fillInt("ccb_enable", enable) ) {
	  alct_->SetCcbEnable(enable);
	}
	if ( alctParser_.fillInt("inject_mode", mode)) {
	  alct_->SetInjectMode(mode);
	}
	if ( alctParser_.fillInt("send_empty", enable)) {
	  alct_->SetSendEmpty(enable);
	}
	if (alctParser_.fillInt("drift_delay", delay)) {
	  alct_->SetDriftDelay(delay);
	}
	if (alctParser_.fillInt("amode", mode)) {    //add
	  alct_->SetAlctAmode(mode);
	}
	if (alctParser_.fillInt("bxc_offset", offset)) {    //add
	  alct_->SetBxcOffset(offset);
	}
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
	      anodeParser_.fillInt("delay", delay);
	      anodeParser_.fillInt("threshold", threshold);
	      //
	      alct_->SetAsicDelay(number-1,delay);
	      //alct_->delays_[number-1] = delay;
	      //alct_->thresholds_[number-1] = threshold;
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
