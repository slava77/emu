//-----------------------------------------------------------------------
// $Id: CFEB.cc,v 3.2 2009/03/25 10:19:41 liu Exp $
// $Log: CFEB.cc,v $
// Revision 3.2  2009/03/25 10:19:41  liu
// move header files to include/emu/pc
//
// Revision 3.1  2008/08/13 11:30:54  geurts
// introduce emu::pc:: namespaces
// remove any occurences of "using namespace" and make std:: references explicit
//
// Revision 3.0  2006/07/20 21:15:47  geurts
// *** empty log message ***
//
// Revision 2.2  2006/05/11 11:00:26  mey
// Update
//
// Revision 2.1  2005/10/04 16:00:46  mey
// Bug fix
//
// Revision 2.0  2005/04/12 08:07:05  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#include "emu/pc/CFEB.h"

namespace emu {
  namespace pc {


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

  } // namespace emu::pc
  } // namespace emu
