//-----------------------------------------------------------------------
// $Id: CalibDAQ.h,v 3.4 2006/10/18 15:51:07 mey Exp $
// $Log: CalibDAQ.h,v $
// Revision 3.4  2006/10/18 15:51:07  mey
// UPdate
//
// Revision 3.3  2006/10/04 14:24:03  mey
// UPdate
//
// Revision 3.2  2006/09/13 14:13:32  mey
// Update
//
// Revision 3.1  2006/09/04 16:14:45  mey
// UPdate
//
// Revision 3.0  2006/07/20 21:15:47  geurts
// *** empty log message ***
//
// Revision 2.14  2006/07/17 11:56:05  mey
// Included CFEB coonectivity calibration
//
// Revision 2.13  2006/07/14 12:33:26  mey
// New XML structure
//
// Revision 2.12  2006/07/12 12:07:11  mey
// ALCT connectivity
//
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
  CalibDAQ(EmuSystem * emuSystem);
  ~CalibDAQ() {}
  
  void loadConstants();
  void rateTest();
  void gainCFEBtest();
  void CFEBSaturation();
  void loadConstants(Crate * crate);   
  void pulseAllDMBs(int ntim, int nstrip, float dac, int nsleep, int calType=0);
  void pulseAllDMBsPre(int ntim, int nstrip, float dac, int nsleep, int calType=0);
  void pulseAllDMBsInit();
  void pulseAllDMBsPost(int ntim, int nstrip, float dac, int nsleep, int calType=0);
  void injectComparator(int ntim, int nstrip, float dac, int nsleep, float thresh=0.03);
  void pedestalCFEB();
  void pedestalCFEBtest();
  void timeCFEB();
  void timeCFEBtest();
  void pulseAllWires();
  void pulseComparatorPulse();
  void FindL1aDelayComparator();
  void FindL1aDelayALCT();
  void ALCTThresholdScan();
  void ALCTConnectivity();
  void CFEBConnectivity();
  //
  CrateSelector & selector() {return theSelector;}



private:
  CrateSelector theSelector;

};

#endif
