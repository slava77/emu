//-----------------------------------------------------------------------
#ifndef EmuSystemParser_h
#define EmuSystemParser_h
#include "EmuParser.h"
#include <xercesc/dom/DOM.hpp>

#include "DAQMBParser.h"
#include "TMBParser.h"
#include "VMEParser.h"
#include "CCBParser.h"
#include "MPCParser.h"
#include "CSCParser.h"
#include "EmuParser.h"



class EmuSystemParser{
 public:
  EmuSystemParser(){}
  explicit EmuSystemParser(xercesc::DOMNode * pNode);
  //
 private:
  EmuParser parser_;
  DAQMBParser daqmbParser_;
  TMBParser tmbParser_;
  VMEParser vmeParser_;
  CCBParser ccbParser_;
  MPCParser mpcParser_;
  CSCParser cscParser_;

  int crateNumber;
};

#endif






