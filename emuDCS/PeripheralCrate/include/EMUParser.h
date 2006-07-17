//-----------------------------------------------------------------------
#ifndef EMUParser_h
#define EMUParser_h

#include <xercesc/parsers/XercesDOMParser.hpp>

#include "DAQMBParser.h"
#include "TMBParser.h"
#include "VMEParser.h"
#include "CCBParser.h"
#include "MPCParser.h"
#include "CSCParser.h"
#include "EmuParser.h"

class EMUParser {

public:
  EMUParser();
  ~EMUParser();

  /** Parse the file
      @param name File Name
  */
  void parseFile(const char* name);

  inline EmuSystem * GetEmuSystem(){return emuSystem;}

  DAQMBParser daqmbParser() const {return daqmbParser_;}
  TMBParser   tmbParser()     const {return tmbParser_;}
  VMEParser   vmeParser()     const {return vmeParser_;}
  CCBParser   ccbParser()     const {return ccbParser_;}
  MPCParser   mpcParser()     const {return mpcParser_;}
  CSCParser   cscParser()     const {return cscParser_;}

 protected:
  xercesc::XercesDOMParser *parser;
  DAQMBParser daqmbParser_;
  TMBParser tmbParser_;
  VMEParser vmeParser_;
  CCBParser ccbParser_;
  MPCParser mpcParser_;
  CSCParser cscParser_;
  int crateNumber;
  EmuSystem * emuSystem;
};

#endif






