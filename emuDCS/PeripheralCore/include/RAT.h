//-----------------------------------------------------------------------
// $Id: RAT.h,v 1.1 2006/02/01 18:33:13 mey Exp $
// $Log: RAT.h,v $
// Revision 1.1  2006/02/01 18:33:13  mey
// UPdate
//
// Revision 2.15  2006/01/13 10:06:44  mey
// Got rid of virtuals
//
// Revision 2.14  2006/01/12 23:43:39  mey
// Update
//
// Revision 2.13  2006/01/12 12:27:38  mey
// UPdate
//
// Revision 2.12  2006/01/12 11:32:02  mey
// Update
//
// Revision 2.11  2006/01/11 13:25:17  mey
// Update
//
// Revision 2.10  2005/12/16 17:50:08  mey
// Update
//
// Revision 2.9  2005/11/25 23:41:39  mey
// Update
//
// Revision 2.8  2005/11/15 15:37:22  mey
// Update
//
// Revision 2.7  2005/11/07 16:23:24  mey
// Update
//
// Revision 2.6  2005/11/04 10:26:13  mey
// Update
//
// Revision 2.5  2005/11/03 18:24:38  mey
// I2C routines
//
// Revision 2.4  2005/09/28 17:00:05  mey
// Update
//
// Revision 2.3  2005/08/11 08:13:59  mey
// Update
//
// Revision 2.2  2005/07/08 10:29:34  geurts
// introduce debug switch to hide debugging messages
//
// Revision 2.1  2005/06/06 11:10:53  geurts
// default power-up mode DLOG. updated for calibration code.
// direct read/write access to registers
//
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//-----------------------------------------------------------------------
#ifndef RAT_h
#define RAT_h

#include "TMB.h"
#include <string>
#include <bitset>

class RAT
{
public:
  //
  RAT(TMB * );
  virtual ~RAT();
  //
  inline void RedirectOutput(std::ostream * Output) { MyOutput_ = Output ; }
  void ReadRatUser1();
  void decodeRATUser1(int*);
  //
private:
  //
  std::ostream * MyOutput_ ;
  TMB * tmb_ ;
  //
};

#endif

