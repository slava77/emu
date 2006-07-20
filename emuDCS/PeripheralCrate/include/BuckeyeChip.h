//-----------------------------------------------------------------------
// $Id: BuckeyeChip.h,v 3.0 2006/07/20 21:15:47 geurts Exp $
// $Log: BuckeyeChip.h,v $
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

#endif

