//-----------------------------------------------------------------------
// $Id: EmuParser.h,v 1.3 2006/05/09 19:20:02 gilmore Exp $
// $Log: EmuParser.h,v $
// Revision 1.3  2006/05/09 19:20:02  gilmore
// Fix for DCC configure function.
//
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
  void fillHex(std::string item, int & target);
  void fillFloat(std::string item, float & target);
  void fillString(std::string item, std::string & target);

protected:
  xercesc::DOMNamedNodeMap * pAttributes_;
  int size_;

};

#endif

