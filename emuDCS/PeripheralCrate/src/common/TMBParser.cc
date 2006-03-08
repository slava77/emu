//-----------------------------------------------------------------------
// $Id: TMBParser.cc,v 2.7 2006/03/08 22:53:12 mey Exp $
// $Log: TMBParser.cc,v $
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

TMBParser::TMBParser(xercesc::DOMNode * pNode, int crateNumber)
{
  parser_.parseNode(pNode);

  int slot = 0;
  parser_.fillInt("slot", slot);
  if(slot == 0) {
    std::cerr << "No slot specified for TMB! " << std::endl;
  } else {
    tmb_ = new TMB(crateNumber, slot);
    std::cout << "New TMB" << std::endl ;
    parser_.fillInt("cfeb0delay", tmb_->cfeb0delay_);
    parser_.fillInt("cfeb1delay", tmb_->cfeb1delay_);
    parser_.fillInt("cfeb2delay", tmb_->cfeb2delay_);
    parser_.fillInt("cfeb3delay", tmb_->cfeb3delay_);
    parser_.fillInt("cfeb4delay", tmb_->cfeb4delay_);
    parser_.fillInt("alct_tx_clock_delay", tmb_->alct_tx_clock_delay_);
    parser_.fillInt("alct_rx_clock_delay", tmb_->alct_rx_clock_delay_);
    parser_.fillInt("l1adelay", tmb_->l1adelay_);
    parser_.fillInt("l1a_window_size", tmb_->l1a_window_size_);
    parser_.fillInt("alct_match_window_size", tmb_->alct_match_window_size_);
    parser_.fillInt("alct_vpf_delay", tmb_->alct_vpf_delay_);
    parser_.fillString("version", tmb_->version_); 
    parser_.fillInt("mpc_delay", tmb_->mpc_delay_);
    parser_.fillInt("ALCT_input",tmb_->ALCT_input_);
    parser_.fillInt("rpc_exists",tmb_->rpc_exists_);
    parser_.fillInt("fifo_mode",tmb_->fifo_mode_);
    parser_.fillInt("fifo_tbins",tmb_->fifo_tbins_);
    parser_.fillInt("fifo_pretrig",tmb_->fifo_pretrig_);
    parser_.fillInt("alct_clear",tmb_->alct_clear_);
    parser_.fillInt("mpc_tx_delay",tmb_->mpc_tx_delay_);
    parser_.fillInt("l1a_offset",tmb_->l1a_offset_);
    parser_.fillInt("bxn_offset",tmb_->bxn_offset_);
    parser_.fillInt("disableCLCTInputs",tmb_->disableCLCTInputs_);
    xercesc::DOMNode * daughterNode = pNode->getFirstChild();
    while(daughterNode) {
      if (daughterNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
	std::string  nodeName = xercesc::XMLString::transcode(daughterNode->getNodeName());
#ifdef debugV  
	std::cout << nodeName << std::endl;
#endif
	parser_.parseNode(daughterNode);

        std::string chamberType;
        parser_.fillString("chamber_type", chamberType);
        //alct_ memory area is deleted in ~TMB()
        ALCTController * alct_ = new ALCTController(tmb_, chamberType);
	RAT * rat_ = new RAT(tmb_);
	tmb_->alctController_ = alct_; //store alct_ in tmb_
	tmb_->rat_            = rat_ ; //store rat_  in tmb_
	//RAT
       	parser_.fillInt("rpc_rat_delay", rat_->rat_tmb_delay_);
	//ALCT
       	parser_.fillInt("trig_mode", alct_->alct_trig_mode_);
	parser_.fillInt("ext_trig_en", alct_->alct_ext_trig_en_);
	parser_.fillInt("trig_info_en", alct_->alct_trig_info_en_);
	parser_.fillInt("l1a_internal", alct_->alct_l1a_internal_);
	parser_.fillInt("fifo_tbins", alct_->alct_fifo_tbins_);
	parser_.fillInt("fifo_pretrig", alct_->alct_fifo_pretrig_);
	parser_.fillInt("l1a_delay", alct_->alct_l1a_delay_);
	parser_.fillInt("l1a_offset",alct_->alct_l1a_offset_);
	parser_.fillInt("l1a_window", alct_->alct_l1a_window_);
	parser_.fillInt("nph_thresh", alct_->alct_nph_thresh_);
	parser_.fillInt("nph_pattern", alct_->alct_nph_pattern_);
	parser_.fillInt("ccb_enable", alct_->alct_ccb_enable_);
	parser_.fillInt("inject_mode", alct_->alct_inject_mode_);
	parser_.fillInt("send_empty", alct_->alct_send_empty_);
	parser_.fillInt("drift_delay", alct_->alct_drift_delay_);
	parser_.fillString("alct_pattern_file", alct_->alctPatternFile);
	parser_.fillString("alct_hotchannel_file", alct_->alctHotChannelFile);
                   
	int number, delay, threshold;

	xercesc::DOMNode * grandDaughterNode = daughterNode->getFirstChild();

	while (grandDaughterNode) {
	  if (grandDaughterNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
	    std::string  nodeName = xercesc::XMLString::transcode(grandDaughterNode->getNodeName());
	    parser_.parseNode(grandDaughterNode);
	    parser_.fillInt("Number", number);
	    parser_.fillInt("delay", delay);
	    parser_.fillInt("threshold", threshold);
            alct_->delays_[number-1] = delay;
            alct_->thresholds_[number-1] = threshold;
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
