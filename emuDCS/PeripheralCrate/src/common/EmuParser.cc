//-----------------------------------------------------------------------
// $Id: EmuParser.cc,v 3.0 2006/07/20 21:15:48 geurts Exp $
// $Log: EmuParser.cc,v $
// Revision 3.0  2006/07/20 21:15:48  geurts
// *** empty log message ***
//
// Revision 2.3  2006/04/06 22:23:08  mey
// Update
//
// Revision 2.2  2006/04/06 08:54:32  mey
// Got rif of friend TMBParser
//
// Revision 2.1  2006/02/25 11:25:11  mey
// UPdate
//
// Revision 2.0  2005/04/12 08:07:05  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#include "EmuParser.h"
#include <stdio.h>
#include <iostream>

void EmuParser::parseNode(xercesc::DOMNode * pNode) {
  pAttributes_ = pNode->getAttributes();
  size_ = pAttributes_->getLength();
  #ifdef debugV
  std::cout << "   number of attributes = " << size_ << std::endl;
  #endif
}

bool EmuParser::fillInt(std::string item, int & target) {
  bool found=false;
  int value; 
  XMLCh * name = xercesc::XMLString::transcode(item.c_str());
  xercesc::DOMAttr * pAttributeNode = (xercesc::DOMAttr*) pAttributes_->getNamedItem(name);
  if(pAttributeNode) {
    int err = sscanf(xercesc::XMLString::transcode(pAttributeNode->getNodeValue()), "%d", &value);
    if (err==0) std::cerr << "ERRORS in parsing!!!" << item << " code " << err << std::endl;
    target = value;
    found = true;
#ifdef debugV
    std::cout << "  " << item << " = " << target << std::endl;
#endif
  }
  return found;  
}


bool EmuParser::fillString(std::string item, std::string & target) {
  bool found=false;
  std::string value;
  XMLCh * name = xercesc::XMLString::transcode(item.c_str());
  xercesc::DOMAttr * pAttributeNode = (xercesc::DOMAttr*) pAttributes_->getNamedItem(name);
  if(pAttributeNode) {
    value = xercesc::XMLString::transcode(pAttributeNode->getNodeValue());
    target = value;
    found = true;
    #ifdef debugV
      std::cout << "  " << item << " = " << target << std::endl;
    #endif
  }
  return found;
}


bool EmuParser::fillFloat(std::string item, float & target) {
  float value;
  bool found=false;
  XMLCh * name = xercesc::XMLString::transcode(item.c_str());
  xercesc::DOMAttr * pAttributeNode = (xercesc::DOMAttr*) pAttributes_->getNamedItem(name);
  if(pAttributeNode) {
    int err = sscanf(xercesc::XMLString::transcode(pAttributeNode->getNodeValue()), "%f", &value);
    if (err==0) std::cerr << "ERRORS in parsing!!!" << item << " code " << err << std::endl;
    target = value;
    found = true;
    #ifdef debugV
      std::cout << "  " << item << " = " << target << std::endl;
    #endif
  }
  return found;
}


