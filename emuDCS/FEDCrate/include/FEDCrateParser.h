//-----------------------------------------------------------------------
// $Id: FEDCrateParser.h,v 3.1 2007/04/27 19:29:44 gilmore Exp $
// $Log: FEDCrateParser.h,v $
// Revision 3.1  2007/04/27 19:29:44  gilmore
// Improved multiple-FED Crate functions, added DDU firmware broadcast and management features
//
// Revision 3.0  2006/07/20 21:16:10  geurts
// *** empty log message ***
//
// Revision 1.2  2006/01/21 20:20:14  gilmore
// *** empty log message ***
//
// Revision 1.16  2004/06/11 09:38:17  tfcvs
// improved checks on top elements. print out warnings in case they do not
// comply. (FG)
//
//-----------------------------------------------------------------------
#ifndef FEDCrateParser_h
#define FEDCrateParser_h
/*
 *  class FEDCrateParser
 *  author Alex Tumanov 6/12/03
 *     
 */


#include "DDUParser.h"
#include "DCCParser.h"
#include "VMEParser.h"


class FEDCrateParser {

public:
  FEDCrateParser() {}

  /** Parse the file
      @param name File Name
  */
  void parseFile(const char* name);

  DDUParser dduParser() const {return dduParser_;}
  DCCParser dccParser()     const {return dccParser_;}
  VMEParser vmeParser()     const {return vmeParser_;}
  /* There has to be a way to get the crates in the XML
  back to the user, so that selectCrates will work. */
  std::vector<int> crateVector() { return crateVector_; }

protected:
  DDUParser dduParser_;
  DCCParser dccParser_;
  VMEParser vmeParser_;
  int crateNumber;
  std::vector<int> crateVector_;
};

#endif






