//-----------------------------------------------------------------------
// $Id: EmuParser.h,v 2.0 2005/04/12 08:07:03 geurts Exp $
// $Log: EmuParser.h,v $
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#ifndef EmuParser_h
#define EmuParser_h

#include <xercesc/dom/DOM.hpp>
#include <string>

class EmuParser 
{
public:
  EmuParser() {}
  virtual ~EmuParser() {}

  void parseNode(xercesc::DOMNode * pNode);
  void fillInt(std::string item, int & target);
  void fillFloat(std::string item, float & target);
  void fillString(std::string item, std::string & target);
     
  //void fillHex(std::string item, int & target);


protected:
  xercesc::DOMNamedNodeMap * pAttributes_;
  int size_;

};

#endif

