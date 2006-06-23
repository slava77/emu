#ifndef EMUjtag_h
#define EMUjtag_h

#include <string>
#include "EMU_JTAG_constants.h"

class TMB;

class EMUjtag 
{
public:
  //
  //EMUjtag();
  EMUjtag(TMB * );
  ~EMUjtag();
  //
  inline void RedirectOutput(std::ostream * Output) { MyOutput_ = Output ; }
  //
  //--------------------------------------------------------
  //See EMU_JTAG_constants.h for the values of
  //    - JTAGchain 
  //    - selected_chip
  //    - opcode
  //    - length_of_register
  // ... which are supported for following methods...:
  void setup_jtag(int JTAGchain);
  void ShfIR_ShfDR(const int selected_chip,
		   const int opcode,
		   const int length_of_register,
		   const int * tdi_to_shift_in);  //1 bit per index
  void ShfIR_ShfDR(const int selected_chip,
		   const int opcode,
		   const int length_of_register); //drop tdi argument for read-only registers
  //--------------------------------------------------------
  //
  inline int * GetDRtdo() { return shfDR_tdo_ ; }
  inline int GetRegLength() { return register_length_ ; }
  //
  void packCharBuffer(int * bitVector, 
		      int Nbits, 
		      char * charVector);
  void unpackCharBuffer(char * buffer, 
			int length, 
			int firstBit, 
			int * bitVector);
  int bits_to_int(int * vector_of_bits,
		  int length_of_vector,
		  int start_from_MsbOrLsb);   // 0 => LSB of return integer = vector_of_bits[0]
  void int_to_bits(int value_to_expand,
		   int number_of_bits,
		   int * vector_of_bits,
		   int start_from_MsbOrLsb);   // 0 => vector_of_bits[0] = LSB of "value_to_expand"
                                               //      vector_of_bits[number_of_bits] = MSB of "value_to_expand"
  //
protected:
  //
  //
private:
  //
  std::ostream * MyOutput_ ;
  TMB * tmb_ ;
  //
  int jtag_chain_;
  int devices_in_chain_;
  int bits_in_opcode_[MAX_NUM_DEVICES];
  //
  int chip_id_;
  int register_length_;
  int shfDR_tdo_[MAX_NUM_FRAMES];
  //
};

#endif

