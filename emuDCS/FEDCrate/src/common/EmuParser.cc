#include "EmuParser.h"
#include <stdio.h>
#include <iostream>

using namespace std;

void EmuParser::parseNode(DOMNode * pNode) {
  pAttributes_ = pNode->getAttributes();
  size_ = pAttributes_->getLength();
  #ifdef debugV
  cout << "   number of attributes = " << size_ << endl;
  #endif
}

void EmuParser::fillInt(string item, int & target) {
  int value; 
  XMLCh * name = XMLString::transcode(item.c_str());
  DOMAttr * pAttributeNode = (DOMAttr*) pAttributes_->getNamedItem(name);
  if(pAttributeNode) {
    int err = sscanf(XMLString::transcode(pAttributeNode->getNodeValue()), "%d", &value);
    if (err==0) cerr << "ERRORS in parsing!!!" << item << " code " << err << endl;
    target = value;
    #ifdef debugV
    cout << "  " << item << " = " << target << endl;
    #endif
  }
  
}


void EmuParser::fillString(string item, string & target) {
  string value;
  XMLCh * name = XMLString::transcode(item.c_str());
  DOMAttr * pAttributeNode = (DOMAttr*) pAttributes_->getNamedItem(name);
  if(pAttributeNode) {
    value = XMLString::transcode(pAttributeNode->getNodeValue());
    target = value;
    #ifdef debugV
      cout << "  " << item << " = " << target << endl;
    #endif
  }
}


void EmuParser::fillFloat(string item, float & target) {
  float value;
  XMLCh * name = XMLString::transcode(item.c_str());
  DOMAttr * pAttributeNode = (DOMAttr*) pAttributes_->getNamedItem(name);
  if(pAttributeNode) {
    int err = sscanf(XMLString::transcode(pAttributeNode->getNodeValue()), "%f", &value);
    if (err==0) cerr << "ERRORS in parsing!!!" << item << " code " << err << endl;
    target = value;
    #ifdef debugV
      cout << "  " << item << " = " << target << endl;
    #endif
  }
}


