//-----------------------------------------------------------------------
// $Id: CSCParser.h,v 3.0 2006/07/20 21:15:47 geurts Exp $
// $Log: CSCParser.h,v $
// Revision 3.0  2006/07/20 21:15:47  geurts
// *** empty log message ***
//
// Revision 1.3  2006/07/13 15:46:37  mey
// New Parser strurture
//
// Revision 1.2  2006/05/31 12:37:26  mey
// Update
//
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
#include "DAQMBParser.h"
#include "TMBParser.h"
#include "Crate.h"

class CSCParser
{
public:
  CSCParser(){}
  explicit CSCParser(xercesc::DOMNode * , Crate * );
  
protected:
  std::string chamberType;
  std::string label;
  EmuParser parser_;
  Chamber * csc_;
  DAQMBParser daqmbParser_;
  TMBParser tmbParser_;
  //
  Crate * theCrate;
  //
};

#endif






