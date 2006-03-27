//-----------------------------------------------------------------------
// $Id: CSCParser.h,v 1.1 2006/03/27 09:31:14 mey Exp $
// $Log: CSCParser.h,v $
// Revision 1.1  2006/03/27 09:31:14  mey
// Update
//
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#ifndef CSCParser_h
#define CSCParser_h

#include "EmuParser.h"
#include <xercesc/dom/DOM.hpp>
#include "Chamber.h"
#include "DAQMBParser.h"
#include "TMBParser.h"
#include "Crate.h"

class CSCParser
{
public:
  CSCParser(){}
  explicit CSCParser(xercesc::DOMNode * pNode, int crateNumber);
  
protected:
  std::string chamberType;
  EmuParser parser_;
  Chamber * csc_;
  DAQMBParser daqmbParser_;
  TMBParser tmbParser_;
  //
  Crate * theCrate;
  //
};

#endif






