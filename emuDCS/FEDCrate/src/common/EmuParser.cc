//-----------------------------------------------------------------------
// $Id: EmuParser.cc,v 3.1 2008/08/14 14:14:33 paste Exp $
// $Log: EmuParser.cc,v $
// Revision 3.1  2008/08/14 14:14:33  paste
// Adding things to namespace emu::fed, condensing include files, renaming VMEParser.
//
// Revision 3.0  2006/07/20 21:16:11  geurts
// *** empty log message ***
//
// Revision 1.3  2006/05/09 19:20:03  gilmore
// Fix for DCC configure function.
//
// Revision 2.0  2005/04/12 08:07:05  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#include "EmuParser.h"

#include <stdio.h>
#include <iostream>

void emu::fed::EmuParser::parseNode(xercesc::DOMNode * pNode) {
  pAttributes_ = pNode->getAttributes();
  size_ = pAttributes_->getLength();
  #ifdef debugV
  std::cout << "   number of attributes = " << size_ << std::endl;
  #endif
}

void emu::fed::EmuParser::fillInt(std::string item, int & target) {
  int value; 
  XMLCh * name = xercesc::XMLString::transcode(item.c_str());
  xercesc::DOMAttr * pAttributeNode = (xercesc::DOMAttr*) pAttributes_->getNamedItem(name);
  if(pAttributeNode) {
    int err = sscanf(xercesc::XMLString::transcode(pAttributeNode->getNodeValue()), "%d", &value);
    if (err==0) std::cerr << "ERRORS in parsing!!!" << item << " code " << err << std::endl;
    target = value;
    #ifdef debugV
    std::cout << "  " << item << " = " << target << std::endl;
    #endif
  }
  
}


void emu::fed::EmuParser::fillHex(std::string item, int & target) {
  int value; 
  XMLCh * name = xercesc::XMLString::transcode(item.c_str());
  xercesc::DOMAttr * pAttributeNode = (xercesc::DOMAttr*) pAttributes_->getNamedItem(name);
  if(pAttributeNode) {
    int err = sscanf(xercesc::XMLString::transcode(pAttributeNode->getNodeValue()), "%x", &value);
    if (err==0) std::cerr << "ERRORS in parsing!!!" << item << " code " << err << std::endl;
    target = value;
    #ifdef debugV
    std::cout << "  " << item << " = " << target << std::endl;
    #endif
  }
  
}


void emu::fed::EmuParser::fillString(std::string item, std::string & target) {
  std::string value;
  XMLCh * name = xercesc::XMLString::transcode(item.c_str());
  xercesc::DOMAttr * pAttributeNode = (xercesc::DOMAttr*) pAttributes_->getNamedItem(name);
  if(pAttributeNode) {
    value = xercesc::XMLString::transcode(pAttributeNode->getNodeValue());
    target = value;
    #ifdef debugV
      std::cout << "  " << item << " = " << target << std::endl;
    #endif
  }
}


void emu::fed::EmuParser::fillFloat(std::string item, float & target) {
  float value;
  XMLCh * name = xercesc::XMLString::transcode(item.c_str());
  xercesc::DOMAttr * pAttributeNode = (xercesc::DOMAttr*) pAttributes_->getNamedItem(name);
  if(pAttributeNode) {
    int err = sscanf(xercesc::XMLString::transcode(pAttributeNode->getNodeValue()), "%f", &value);
    if (err==0) std::cerr << "ERRORS in parsing!!!" << item << " code " << err << std::endl;
    target = value;
    #ifdef debugV
      std::cout << "  " << item << " = " << target << std::endl;
    #endif
  }
}


