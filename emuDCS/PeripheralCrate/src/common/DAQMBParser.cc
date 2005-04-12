//-----------------------------------------------------------------------
// $Id: DAQMBParser.cc,v 2.0 2005/04/12 08:07:05 geurts Exp $
// $Log: DAQMBParser.cc,v $
// Revision 2.0  2005/04/12 08:07:05  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#include "DAQMBParser.h"
#include "DAQMB.h"
#include "CFEB.h"
#include <xercesc/dom/DOM.hpp>

#define debug false

DAQMBParser::DAQMBParser(xercesc::DOMNode * pNode, int crateNumber)
{

  parser_.parseNode(pNode);

  int slot = 0;
  parser_.fillInt("slot", slot);
  if(slot == 0) {
    std::cerr << "No slot specified for DMB! " << std::endl;
  } else {
    daqmb_ = new DAQMB(crateNumber,slot);  
    parser_.fillInt("feb_dav_delay", daqmb_->feb_dav_delay_);
    parser_.fillInt("tmb_dav_delay", daqmb_->tmb_dav_delay_);
    parser_.fillInt("push_dav_delay", daqmb_->push_dav_delay_);
    parser_.fillInt("l1acc_dav_delay", daqmb_->l1acc_dav_delay_);
    parser_.fillInt("ALCT_dav_delay", daqmb_->ALCT_dav_delay_);
    parser_.fillInt("calibration_LCT_delay", daqmb_->calibration_LCT_delay_);
    parser_.fillInt("calibration_l1acc_delay", daqmb_->calibration_l1acc_delay_);
    parser_.fillInt("pulse_delay", daqmb_->pulse_delay_);
    parser_.fillInt("inject_delay", daqmb_->inject_delay_);

    parser_.fillFloat("pul_dac_set", daqmb_->pul_dac_set_);
    parser_.fillFloat("inj_dac_set", daqmb_->inj_dac_set_);
    parser_.fillFloat("set_comp_thresh", daqmb_->set_comp_thresh_);
    parser_.fillInt("feb_clock_delay", daqmb_->feb_clock_delay_);
    parser_.fillInt("comp_mode", daqmb_->comp_mode_);
    parser_.fillInt("comp_timing", daqmb_->comp_timing_);
    parser_.fillInt("pre_block_end", daqmb_->pre_block_end_);
   
    int number=0;   

    xercesc::DOMNode * daughterNode = pNode->getFirstChild();

    while (daughterNode) {
      if (daughterNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
	std::string  nodeName = xercesc::XMLString::transcode(daughterNode->getNodeName());
        parser_.parseNode(daughterNode);
	parser_.fillInt("Number", number);
	CFEB cfeb(number);
	daqmb_->cfebs_.push_back(cfeb);
      }
      daughterNode = daughterNode->getNextSibling();
    }
  }
}



























