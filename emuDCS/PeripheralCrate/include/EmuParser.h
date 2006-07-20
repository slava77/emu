//-----------------------------------------------------------------------
// $Id: EmuParser.h,v 3.0 2006/07/20 21:15:47 geurts Exp $
// $Log: EmuParser.h,v $
// Revision 3.0  2006/07/20 21:15:47  geurts
// *** empty log message ***
//
// Revision 2.2  2006/04/06 22:23:07  mey
// Update
//
// Revision 2.1  2006/04/06 08:54:32  mey
// Got rif of friend TMBParser
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
  bool fillInt(std::string item, int & target);
  bool fillFloat(std::string item, float & target);
  bool fillString(std::string item, std::string & target);
     
  //void fillHex(std::string item, int & target);


protected:
  xercesc::DOMNamedNodeMap * pAttributes_;
  int size_;

};

#endif

