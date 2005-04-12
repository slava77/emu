//-----------------------------------------------------------------------
// $Id: CFEB.cc,v 2.0 2005/04/12 08:07:05 geurts Exp $
// $Log: CFEB.cc,v $
// Revision 2.0  2005/04/12 08:07:05  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#include "CFEB.h"

CFEB::CFEB(int number, std::vector<BuckeyeChip> buckeyes) :
  number_(number),
  buckeyeChips_(buckeyes)
{
}


CFEB::CFEB(int number) :
  number_(number)
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

