//-----------------------------------------------------------------------
// $Id: PeripheralCrateParser.h,v 3.1 2006/08/17 15:01:31 mey Exp $
// $Log: PeripheralCrateParser.h,v $
// Revision 3.1  2006/08/17 15:01:31  mey
// Modified Parser to accept globals
//
// Revision 3.0  2006/07/20 21:15:47  geurts
// *** empty log message ***
//
// Revision 2.4  2006/07/13 15:46:37  mey
// New Parser strurture
//
// Revision 2.3  2006/07/11 14:49:28  mey
// New Parser sturcture ready to go
//
// Revision 2.2  2006/03/30 13:55:38  mey
// Update
//
// Revision 2.1  2006/03/24 14:35:03  mey
// Update
//
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
// Revision 1.16  2004/06/11 09:38:17  tfcvs
// improved checks on top elements. print out warnings in case they do not
// comply. (FG)
//
//-----------------------------------------------------------------------
#ifndef PeripheralCrateParser_h
#define PeripheralCrateParser_h

#include "DAQMBParser.h"
#include "TMBParser.h"
#include "VMEParser.h"
#include "CCBParser.h"
#include "MPCParser.h"
#include "CSCParser.h"
#include "EmuParser.h"
#include "EmuSystem.h"

class PeripheralCrateParser {

public:
  //
  PeripheralCrateParser() {}
  explicit PeripheralCrateParser(xercesc::DOMNode * , EmuSystem *, xercesc::DOMNode * );
  //
  DAQMBParser daqmbParser() const {return daqmbParser_;}
  TMBParser tmbParser()     const {return tmbParser_;}
  VMEParser vmeParser()     const {return vmeParser_;}
  CCBParser ccbParser()     const {return ccbParser_;}
  MPCParser mpcParser()     const {return mpcParser_;}
  CSCParser cscParser()     const {return cscParser_;}
  //
protected:
  DAQMBParser daqmbParser_;
  TMBParser tmbParser_;
  VMEParser vmeParser_;
  CCBParser ccbParser_;
  MPCParser mpcParser_;
  CSCParser cscParser_;
  int crateNumber;
};

#endif






