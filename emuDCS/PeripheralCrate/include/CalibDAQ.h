//-----------------------------------------------------------------------
// $Id: CalibDAQ.h,v 2.11 2006/05/23 14:11:11 mey Exp $
// $Log: CalibDAQ.h,v $
// Revision 2.11  2006/05/23 14:11:11  mey
// Update
//
// Revision 2.10  2006/05/23 13:17:49  mey
// Update
//
// Revision 2.9  2006/05/17 14:16:44  mey
// Update
//
// Revision 2.8  2006/04/28 13:41:16  mey
// Update
//
// Revision 2.7  2006/04/25 13:25:18  mey
// Update
//
// Revision 2.6  2006/04/24 14:57:20  mey
// Update
//
// Revision 2.5  2006/04/11 15:27:42  mey
// Update
//
// Revision 2.4  2006/03/24 16:40:36  mey
// Update
//
// Revision 2.3  2006/03/07 09:22:57  mey
// Update
//
// Revision 2.2  2006/02/20 13:31:13  mey
// Update
//
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
  void CFEBSaturation();
  void loadConstants(Crate * crate);   
  void pulseAllDMBs(int ntim, int nstrip, float dac, int nsleep, int calType=0);
  void injectComparator(int ntim, int nstrip, float dac, int nsleep, float thresh=0.03);
  void pedestalCFEB();
  void timeCFEB();
  void pulseAllWires();
  void pulseComparatorPulse();
  void FindL1aDelayComparator();
  void FindL1aDelayALCT();
  void ALCTThresholdScan();

  CrateSelector & selector() {return theSelector;}



private:
  CrateSelector theSelector;

};

#endif
