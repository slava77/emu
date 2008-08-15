/*****************************************************************************\
* $Id: EmuParser.cc,v 3.2 2008/08/15 08:35:51 paste Exp $
*
* $Log: EmuParser.cc,v $
* Revision 3.2  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
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


