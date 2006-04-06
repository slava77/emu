//-----------------------------------------------------------------------
// $Id: DAQMBParser.cc,v 2.4 2006/04/06 22:23:08 mey Exp $
// $Log: DAQMBParser.cc,v $
// Revision 2.4  2006/04/06 22:23:08  mey
// Update
//
// Revision 2.3  2006/03/10 15:55:28  mey
// Update
//
// Revision 2.2  2005/09/13 14:46:40  mey
// Get DMB crate id; and DCS
//
// Revision 2.1  2005/09/07 13:54:45  mey
// Included new timing routines from Jianhui
//
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
  //
  parser_.parseNode(pNode);
  //
  int slot = 0;
  parser_.fillInt("slot", slot);
  if(slot == 0) {
    std::cerr << "No slot specified for DMB! " << std::endl;
  } else {
    daqmb_ = new DAQMB(crateNumber,slot);  
    //
    int delay;
    if ( parser_.fillInt("feb_dav_delay", delay)){
      daqmb_->SetFebDavDelay(delay);
    }
    if ( parser_.fillInt("tmb_dav_delay", delay) ){
      daqmb_->SetTmbDavDelay(delay);
    }
    if (parser_.fillInt("push_dav_delay", delay)){
      daqmb_->SetPushDavDelay(delay);
    }
    if(parser_.fillInt("l1acc_dav_delay", delay)){
      daqmb_->SetL1aDavDelay(delay);
    }
    if(parser_.fillInt("ALCT_dav_delay", delay)){
      daqmb_->SetAlctDavDelay(delay);
    }
    if(parser_.fillInt("calibration_LCT_delay", delay)){
      daqmb_->SetCalibrationLctDelay(delay);
    }
    if(parser_.fillInt("calibration_l1acc_delay", delay)){
      daqmb_->SetCalibrationL1aDelay(delay);
    }
    if(parser_.fillInt("pulse_delay", delay)){
      daqmb_->SetPulseDelay(delay);
    }
    if(parser_.fillInt("inject_delay", delay)){
      daqmb_->SetInjectDelay(delay);
    }
    float value;
    if(parser_.fillFloat("pul_dac_set", value)){
      daqmb_->SetPulseDac(value);
    }
    if(parser_.fillFloat("inj_dac_set", value)){
      daqmb_->SetInjectorDac(value);
    }
    if(parser_.fillFloat("set_comp_thresh", value)){
      daqmb_->SetCompThresh(value);
    }
    if(parser_.fillInt("feb_clock_delay", delay)){
      daqmb_->SetFebClockDelay(delay);
    }
    int mode;
    if(parser_.fillInt("comp_mode", mode)){
      daqmb_->SetCompMode(mode);
    }
    if(parser_.fillInt("comp_timing", delay)){
      daqmb_->SetCompTiming(delay);
    }
    if(parser_.fillInt("pre_block_end", delay)){
      daqmb_->SetPreBlockEnd(delay);
    }
    if(parser_.fillInt("cable_delay", delay)){
      daqmb_->SetCableDelay(delay);
    }
    int id;
    if(parser_.fillInt("crate_id",id)){
      daqmb_->SetCrateId(id);
    }
    //
    int number=0;   
    
    xercesc::DOMNode * daughterNode = pNode->getFirstChild();

    while (daughterNode) {
      if (daughterNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
	if (strcmp("CFEB",xercesc::XMLString::transcode(daughterNode->getNodeName()))==0){
	  parser_.parseNode(daughterNode);
	  if(parser_.fillInt("Number", number)){
	    std::cout <<"CFEB" <<std::endl;
	    if ( number <5 ){
	      CFEB cfeb(number);
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



























