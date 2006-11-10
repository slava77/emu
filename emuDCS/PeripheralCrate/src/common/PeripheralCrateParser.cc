//-----------------------------------------------------------------------
// $Id: PeripheralCrateParser.cc,v 3.2 2006/11/10 16:51:45 mey Exp $
// $Log: PeripheralCrateParser.cc,v $
// Revision 3.2  2006/11/10 16:51:45  mey
// Update
//
// Revision 3.1  2006/08/17 15:02:31  mey
// Modified Parser to accept globals
//
// Revision 3.0  2006/07/20 21:15:48  geurts
// *** empty log message ***
//
// Revision 2.11  2006/07/20 14:03:12  mey
// Update
//
// Revision 2.10  2006/07/14 12:33:26  mey
// New XML structure
//
// Revision 2.9  2006/07/13 15:46:37  mey
// New Parser strurture
//
// Revision 2.8  2006/07/11 14:49:29  mey
// New Parser sturcture ready to go
//
// Revision 2.7  2006/05/17 14:16:45  mey
// Update
//
// Revision 2.6  2006/03/31 09:22:05  mey
// Update
//
// Revision 2.5  2006/03/30 13:55:38  mey
// Update
//
// Revision 2.4  2006/03/24 14:35:04  mey
// Update
//
// Revision 2.3  2006/03/08 22:53:12  mey
// Update
//
// Revision 2.2  2006/02/25 11:25:11  mey
// UPdate
//
// Revision 2.1  2006/01/19 10:03:46  mey
// Update
//
// Revision 2.0  2005/04/12 08:07:05  geurts
// *** empty log message ***
//
// Revision 1.21  2004/07/19 19:37:57  tfcvs
// Removed unused variables and unsigned some variables in order to prevent compiler warnings (-Wall flag) (FG)
//
// Revision 1.20  2004/06/11 09:38:17  tfcvs
// improved checks on top elements. print out warnings in case they do not
// comply. (FG)
//
//-----------------------------------------------------------------------
#include <iostream>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/XMLPScanToken.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include "PeripheralCrateParser.h"

#include "Crate.h"
#include "VMEController.h"

#define debug false

//xercesc::XercesDOMParser *parser;

PeripheralCrateParser::PeripheralCrateParser(xercesc::DOMNode *pNode,EmuSystem * emuSystem,xercesc::DOMNode *pNodeGlobal){ 
  //
  //xercesc::DOMNode * pNode2 = pNode->getFirstChild();
  //while (pNode2) {
  //if (pNode2->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
  //
  std::cout << "Periph...........!!!!!" <<std::endl;
  std::cout << pNodeGlobal << std::endl;
  //
  if(pNodeGlobal) {
    std::cout << "Peri...........!!!!!" <<std::endl;
  }
  //
  xercesc::DOMNamedNodeMap * pAttributes = pNode->getAttributes();
  xercesc::DOMNode * e = pAttributes->getNamedItem(xercesc::XMLString::transcode("crateID"));
  crateNumber = atoi(xercesc::XMLString::transcode(e->getNodeValue()));

  Crate * crate = 0x0;
      
  vmeParser_ = VMEParser(pNode, crateNumber,emuSystem);                       
  crate = vmeParser_.crate();
	  
  xercesc::DOMNode * pNode3 = pNode->getFirstChild(); 
  if (pNode3==0) std::cout << " Bad element "<< std::endl;
  
  while(pNode3) {
    // the node was really a board of the peripheral crate like DMB, TMB, etc.
    if (pNode3->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
      if (debug) std::cout <<"  "<< xercesc::XMLString::transcode(pNode3->getNodeName()) << std::endl;
    }
    //
    //if (strcmp("VME",xercesc::XMLString::transcode(pNode3->getNodeName()))==0) {  
    //vmeParser_ = VMEParser(pNode3, crateNumber,emuSystem);                       
    //crate = vmeParser_.crate();
    //
    //}
    
    if (strcmp("CSC",xercesc::XMLString::transcode(pNode3->getNodeName()))==0) {  
      cscParser_ = CSCParser(pNode3, crate,pNodeGlobal);
    }
    
    //if (strcmp("DAQMB",xercesc::XMLString::transcode(pNode3->getNodeName()))==0) {  
    //daqmbParser_ = DAQMBParser(pNode3, crate);
    //}
    
    //if (strcmp("TMB",xercesc::XMLString::transcode(pNode3->getNodeName()))==0) {  
    //tmbParser_ = TMBParser(pNode3, crate);
    //}
    
    if (strcmp("CCB",xercesc::XMLString::transcode(pNode3->getNodeName()))==0) {  
      ccbParser_ = CCBParser(pNode3, crate);
    }
    
    if (strcmp("MPC",xercesc::XMLString::transcode(pNode3->getNodeName()))==0) {  
      mpcParser_ = MPCParser(pNode3, crate);
    }
    
    pNode3 = pNode3->getNextSibling();
    
  } // end of looping over boards (pNode3)
  std::cout <<"PeripheralCrateParser: finished looping over boards for crate #"<< crateNumber << std::endl; 
  //Here init controller()
  //if(crate) vmeParser_.controller()->init();
}

