//-----------------------------------------------------------------------
// $Id: PeripheralCrateParser.h,v 2.1 2006/03/24 14:35:03 mey Exp $
// $Log: PeripheralCrateParser.h,v $
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
#include "DDUParser.h"
#include "CSCParser.h"


class PeripheralCrateParser {

public:
  PeripheralCrateParser() {}

  /** Parse the file
      @param name File Name
  */
  void parseFile(const char* name);

  DAQMBParser daqmbParser() const {return daqmbParser_;}
  TMBParser tmbParser()     const {return tmbParser_;}
  VMEParser vmeParser()     const {return vmeParser_;}
  CCBParser ccbParser()     const {return ccbParser_;}
  MPCParser mpcParser()     const {return mpcParser_;}
  DDUParser dduParser()     const {return dduParser_;}
  CSCParser cscParser()     const {return cscParser_;}

protected:
  DAQMBParser daqmbParser_;
  TMBParser tmbParser_;
  VMEParser vmeParser_;
  CCBParser ccbParser_;
  MPCParser mpcParser_;
  DDUParser dduParser_;
  CSCParser cscParser_;
  int crateNumber;
};

#endif






