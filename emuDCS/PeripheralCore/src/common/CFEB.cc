//-----------------------------------------------------------------------
// $Id: CFEB.cc,v 2.1 2005/10/04 16:00:46 mey Exp $
// $Log: CFEB.cc,v $
// Revision 2.1  2005/10/04 16:00:46  mey
// Bug fix
//
// Revision 2.0  2005/04/12 08:07:05  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#include "CFEB.h"

CFEB::CFEB(int number, std::vector<BuckeyeChip> buckeyes) :
  number_(4-number),
  buckeyeChips_(buckeyes)
{
}


CFEB::CFEB(int number) :
  number_(4-number)
{
  for(unsigned i = 0; i < 6; ++i) {
    buckeyeChips_.push_back(BuckeyeChip(i));
  }
}


char CFEB::chipMask() const {
  char chip_mask=0;
  for(unsigned ichip=0;ichip<buckeyeChips_.size();ichip++){
    int i = buckeyeChips_[ichip].number();
    chip_mask |= (1<<(5-i));
  }
  return chip_mask;
}

