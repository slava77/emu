//-----------------------------------------------------------------------
// $Id: BuckeyeChip.h,v 1.1 2009/03/25 10:07:42 liu Exp $
// $Log: BuckeyeChip.h,v $
// Revision 1.1  2009/03/25 10:07:42  liu
// move header files to include/emu/pc
//
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
#ifndef BuckeyeChip_h
#define BuckeyeChip_h

namespace emu {
  namespace pc {

/** preamp-shaper chip
 *  should be 6 per CFEB, each controlling
 *  one layer
 */

class BuckeyeChip {
public:
  explicit BuckeyeChip(int number) : number_(number) {}
  //enum Mode {NORM_RUN, SMALL_CAP, MEDIUM_CAP, LARGE_CAP, EXT_CAP
  //           NORM_RUN1, NORM_RUN2, KILL_CHAN};
  int number() const {return number_;}

  int shift_array(int i) const {return shift_array_[i];}
  /// returns 6 words to be sent to VME
  //void shift_bits(char * result);
private:
  int number_;
  int shift_array_[16];
};

  } // namespace emu::pc
  } // namespace emu
#endif

