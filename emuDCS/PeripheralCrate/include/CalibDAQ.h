//-----------------------------------------------------------------------
// $Id: CalibDAQ.h,v 2.1 2006/01/11 17:00:14 mey Exp $
// $Log: CalibDAQ.h,v $
// Revision 2.1  2006/01/11 17:00:14  mey
// Update
//
// Revision 2.0  2005/06/06 10:01:03  geurts
// calibration routines by Alex Tumanov and Jason Gilmore
//
//
//-----------------------------------------------------------------------
#ifndef CalibDAQ_h
#define CalibDAQ_h
#include "CrateSelector.h"
#include <string>

class DAQMB;
class Crate;

class CalibDAQ {
public:
  CalibDAQ() {}
  ~CalibDAQ() {}
  
  void loadConstants();
  
  void rateTest();

  void loadConstants(Crate * crate);   

  void pulseAllDMBs(int ntim, int nstrip, float dac, int nsleep);

  void injectAllDMBs(int ntim);

  void pulseAllWires();

  CrateSelector & selector() {return theSelector;}



private:
  CrateSelector theSelector;

};

#endif
