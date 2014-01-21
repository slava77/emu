#ifndef _DMBHeader_h_
#define _DMBHeader_h_

class DMBHeader {

 public:

  uint32_t l1a() const { return ( ( l1a_hi_ << 12 ) | l1a_lo_ ); }

 private:
  uint32_t l1a_lo_  : 12;
  uint32_t ____9_   :  4;
  uint32_t l1a_hi_  : 12;
  uint32_t ___9__   :  4;

};
#endif
