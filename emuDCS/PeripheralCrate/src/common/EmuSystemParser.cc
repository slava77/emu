//-----------------------------------------------------------------------
#include "EmuSystemParser.h"
#include "EmuSystem.h"
#include "PeripheralCrateParser.h"

#define debug false

EmuSystemParser::EmuSystemParser(xercesc::DOMNode *pNode)
{
  parser_.parseNode(pNode);
  //
  std::cout << "emuSystemParser" << std::endl;
  //
  emuSystem = new EmuSystem();
  xercesc::DOMNode * pNodeGlobal = 0x0;
  //
  xercesc::DOMNode * pNode1 = pNode->getFirstChild();
  while (pNode1) {
    if (pNode1->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
      if (strcmp("Global",xercesc::XMLString::transcode(pNode1->getNodeName()))==0){
	std::cout << "Found Global" << std::endl;
	//
	pNodeGlobal = pNode1->cloneNode(true);
	//
      }
      //
      if (strcmp("PeripheralCrate",xercesc::XMLString::transcode(pNode1->getNodeName()))==0){
	std::cout << "Found PeripheralCrate" << std::endl;
	PeripheralCrateParser parser = PeripheralCrateParser(pNode1,emuSystem,pNodeGlobal);
      }
    }
    pNode1 = pNode1->getNextSibling();
    //
  } // end of Top Element Loop, <EMU> only (pNode1)
  
} //end of parsing config file


