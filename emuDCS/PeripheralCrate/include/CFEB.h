//-----------------------------------------------------------------------
// $Id: CFEB.h,v 2.0 2005/04/12 08:07:03 geurts Exp $
// $Log: CFEB.h,v $
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#ifndef CFEB_h
#define CFEB_h
#include "JTAG_constants.h"
#include "BuckeyeChip.h"
#include <vector>

class CFEB {
public:
  CFEB(int number, std::vector<BuckeyeChip> buckeyes);
  /// assumes all default Buckeyes are present
  explicit CFEB(int number);

  int number() const {return number_;}
  DEVTYPE scamDevice() const {return (DEVTYPE) (F1SCAM+number_);}
  DEVTYPE promDevice() const {return (DEVTYPE) (F1PROM+number_);}
  
  std::vector<BuckeyeChip> buckeyeChips() const {return buckeyeChips_;}
  char chipMask() const;

private:
  int number_;
  std::vector<BuckeyeChip> buckeyeChips_;
};

#endif

