//-----------------------------------------------------------------------
// $Id: ALCTClasses.h,v 2.0 2005/04/12 08:07:03 geurts Exp $
// $Log: ALCTClasses.h,v $
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#ifndef ALCTClasses_h
#define ALCTClasses_h

#include <iostream>

class ALCTIDRegister {
public:
  unsigned    chip;
  unsigned    version;
  unsigned    day;
  unsigned    month;
  short int        year;
  friend std::ostream & operator<<(std::ostream & os, const ALCTIDRegister & id) {
    os << std::hex << "Chip " << id.chip << " version " << id.version
       << " day " << id.day << " month " << id.month
       << " year " << id.year << std::dec;
   return os;
  }
  bool operator==(const ALCTIDRegister & b) const {
    return ( (chip==b.chip) && (version==b.version) && (day==b.day)
          && (month==b.month) && (year==b.year) );
  }
  bool operator!=(const ALCTIDRegister & b) const {
    return ( !((chip==b.chip) && (version==b.version) && (day==b.day)
	  && (month==b.month) && (year==b.year)) );

  }

  /// reads from an integer buffer
  void unpack_slow_id(int * buf) {
    chip    = buf[0];
    version = buf[1];
    year    = buf[2];
    day     = buf[3];
    month   = buf[4];
  }

  void unpack_fast_id(unsigned long * ID) {
    month = ID[1] & 0xff;
    day = (ID[0] >> 24) & 0xff;
    year = (ID[0] >> 8) & 0xffff;
    version = (ID[0] >> 4) & 0xf;
    chip = ID[0] & 0xf;
  }
};


class ALCT {
  ALCTIDRegister           idreg;
  unsigned char            lct_jtag_disable;
  unsigned char            test_pat_mode;
  unsigned char            accel_mu_mode;
  unsigned char            mask_all;
  unsigned char            unassigned;
  short int                hot_chan_mask[6*4]; //myc++
  short int                test_pattern[6*4];  //myc++
};


struct Rfield  // this structure describes one field of the JTAG register
{
        unsigned mask; // defines the OR mask for the field (for 2-bit field it is 3)
        unsigned bit;  // number of bit within the register where the field starts
        unsigned deflt; // default value
        char name[20];  // parameter name
        //char* pparam; // pointer to the corresponding parameter in alct_params_type structure
};



struct chamtypefield
{
        char* chamber_name; // name of the chamber
        int WG_number; // number of wiregroups total in all layers
        unsigned * RegSz;
        // array with the lengths of the jtag registers for this type of the firmware
        int * OSfield;
        // array of type Rfield containing the field
        // descriptions of output FIFO word
};

// from alct_params.h:

#define MXBITSTREAM    1000       /* Same as Jonathan's Fortran version */
#define NUM_POSTBITS    3
#define MAX_NUM_CHIPS     2
#define MAX_NUM_VALUES   50

// end from alct_params.h

// from jtag_b.h

#define TCK  0x02    /* Bit 1 is TCK output */
#define TMS  0x04    /* Bit 2 is TMS output */
#define TDI  0x01    /* Bit 0 is TDI output */
#define TDO  0x10    //#define TDO  0x08    /* Bit 3 is inverted TDO output */
#define notTCKTMS 0xF9    /* not of TCK,TMS */
#define TDITMS 0x05
#define TCKTMS 0x06
#define TRST 0x04    /* Bit 2 is TRST */
#define STRB 0x01    /* Bit 0 is Strobe */


// end from jtag_b.h

#define MAXBUFSIZE 8192

/************************************************************************/
#define ALCTDEBUG 1
#define ALCT_V_IRsize 5         // = VB = ALCT Virtex IR size
#define ALCT_SC_IRsize 6        // = VB = ALCT Slow Control IR size
#define IRsize 5                // Instruction Register size
#define IRPreBits 4
#define DRPreBits 3
#define PostBits 3

#endif
