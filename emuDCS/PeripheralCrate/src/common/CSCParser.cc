//-----------------------------------------------------------------------
// $Id: CSCParser.cc,v 3.3 2006/11/15 16:01:36 mey Exp $
// $Log: CSCParser.cc,v $
// Revision 3.3  2006/11/15 16:01:36  mey
// Cleaning up code
//
// Revision 3.2  2006/09/05 16:14:14  mey
// Update
//
// Revision 3.1  2006/08/17 15:01:31  mey
// Modified Parser to accept globals
//
// Revision 3.0  2006/07/20 21:15:47  geurts
// *** empty log message ***
//
// Revision 1.5  2006/07/13 15:46:37  mey
// New Parser strurture
//
// Revision 1.4  2006/07/11 15:00:39  mey
// Update
//
// Revision 1.3  2006/05/31 12:37:26  mey
// Update
//
// Revision 1.2  2006/05/16 15:54:38  mey
// UPdate
//
// Revision 1.1  2006/03/27 09:30:59  mey
// Update
//
// Revision 2.0  2005/04/12 08:07:05  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#include "CSCParser.h"
#include "CrateSetup.h"
#include "Chamber.h"
//
CSCParser::CSCParser(xercesc::DOMNode * pNode, Crate * theCrate, xercesc::DOMNode * pNodeGlobal)
{
  parser_.parseNode(pNode);
  parser_.fillString("chamber_type", chamberType);
  parser_.fillString("label", label);
  csc_ = new Chamber();
  csc_->SetCrate(theCrate);
  //
  theCrate->AddChamber(csc_);
  //
  std::cout << "Creating CSC Type=" << chamberType << std::endl;
  std::cout << "Done" << std::endl;
  //
  xercesc::DOMNode * pNode1 = pNode->getFirstChild();
  //
  while (pNode1) {
    if (pNode1->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
      std::cout << "PeripheralCrateParser: pNode1=" << xercesc::XMLString::transcode(pNode1->getNodeName()) << std::endl;
      //
      if (strcmp("DAQMB",xercesc::XMLString::transcode(pNode1->getNodeName()))==0) {  
	daqmbParser_ = DAQMBParser(pNode1, theCrate, csc_);
	//(daqmbParser_.daqmb())->SetCSC(csc_);
	//csc_->SetDMB(daqmbParser_.daqmb());
      }
      //
      if (strcmp("TMB",xercesc::XMLString::transcode(pNode1->getNodeName()))==0) {  
	std::cout << "TMBparser..." <<std::endl;
	tmbParser_ = TMBParser(pNode1, theCrate, csc_, pNodeGlobal);
	//(tmbParser_.tmb())->SetCSC(csc_);
	//csc_->SetTMB(tmbParser_.tmb());
      }       
    }
    pNode1 = pNode1->getNextSibling();
  }
  //
}
//

























