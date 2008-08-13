//-----------------------------------------------------------------------
// $Id: CFEB.h,v 3.1 2008/08/13 11:30:53 geurts Exp $
// $Log: CFEB.h,v $
// Revision 3.1  2008/08/13 11:30:53  geurts
// introduce emu::pc:: namespaces
// remove any occurences of "using namespace" and make std:: references explicit
//
// Revision 3.0  2006/07/20 21:15:47  geurts
// *** empty log message ***
//
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

namespace emu {
  namespace pc {


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


  } // namespace emu::pc
  } // namespace emu
#endif

