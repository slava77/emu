#include "EMUjtag.h"
//
#include <iostream>
#include <iomanip>
#include <unistd.h> // for sleep
#include <vector>
#include <string>
//
#include "TMB.h"

//EMUjtag::EMUjtag(){
//}
//
EMUjtag::EMUjtag(TMB * tmb) :
  tmb_(tmb)
{
  //
  std::cout << "Creating EMUjtag" << std::endl ;
  //
  MyOutput_ = &std::cout ;
  //
  jtag_chain_ = -1;
  //
};
//
EMUjtag::~EMUjtag() {
  //
  //
}
//
void EMUjtag::ShfIR_ShfDR(const int selected_chip, 
			  const int opcode, 
			  const int size_of_register, 
			  const int * write_data) {

  //This member assumes you have run setup_jtag(int)...
  //This member sets up the tdi for shifting in "opcode" for ShfIR and then ShfDR.  
  //  
  //1) The "opcode" is written to the Instruction Register for "selected_chip"
  //   on jtag_chain_ (setup_jtag()).  
  //2) Shift in "write_data" on tdi into the Data Register (one bit per index).
  //   Clearly one bit per index is not very memory efficient, but it 
  //   allows for easy manipulation by the user.  For read-only JTAG, 
  //   this array should be all zeros.  The number of bits shifted in on tdi or
  //   out on tdo is "size_of_register".
  //3) The data which is shifted out of the data register is put into 
  //   the int array "shfDR_tdo_" (also one bit per index).   

  // N.B. JTAG instructions and data are loaded and readout in order
  //      starting with chipN, chipN-1, ... ,chip1, and ending with chip0

  if (jtag_chain_ < 0) return;

  chip_id_ = selected_chip;
  register_length_ = size_of_register;

  (*MyOutput_) << "EMUjtag: Use " << std::dec << bits_in_opcode_[chip_id_] 
	       << " bits to write opcode " << std::hex << opcode 
	       << " to chip " << std::hex << chip_id_ 
	       << " on chain " << std::hex << jtag_chain_
	       << " -> expect " << std::dec << register_length_ 
	       << " bits back" << std::hex << std::endl;


  //** Clear the read data which was previously there:
  for (int i=0; i<MAX_NUM_FRAMES; i++) 
    shfDR_tdo_[i] = 0;

  int tdi[MAX_NUM_FRAMES];
  char sndBuffer[MAX_BUFFER_SIZE], rcvBuffer[MAX_BUFFER_SIZE];

  // ** Clean buffers:
  for (int i=0; i<MAX_BUFFER_SIZE; i++) {
    sndBuffer[i] = 0;
    rcvBuffer[i] = 0;
  }
  int iframe = 0;                              //reset frame counter

  // ** Construct opcode for the selected chip (all but chip_id are BYPASS = all 1's),
  int idevice, ichip, ibit;
  int bit;
  for (idevice=0; idevice<devices_in_chain_; idevice++) {          //loop over all the chips in this chain

    ichip = devices_in_chain_ - idevice - 1;                       //chip order in chain is reversed

    for (ibit=0; ibit<bits_in_opcode_[ichip]; ibit++) {            //up to the number of bits in this chip's opcode
      bit = 1;                                                     //BYPASS
      if (ichip == chip_id_)                                       //this is the chip we want
	bit = (opcode >> ibit) & 0x1;                              //extract bit from opcode
      tdi[iframe++]=bit;
    }
  }

  //  (*MyOutput_) << "There are " << std::dec << iframe << " frames to send..." << std::endl;

  if (iframe > MAX_NUM_FRAMES) 
    (*MyOutput_) << "EMUjtag: ShfIR_ShfDR IR ERROR: Too many frames -> " << iframe << std::endl;

  //pack tdi into an array of char so scan can handle it:
  packCharBuffer(tdi,iframe,sndBuffer);

  //  (*MyOutput_) << "sndBuffer to ShfIR=";
  //  for (int i=iframe/8; i>=0; i--) 
  //    (*MyOutput_) << " " << std::hex << (sndBuffer[i]&0xff); 
  //  (*MyOutput_) << std::endl;
  
  tmb_->scan(INSTR_REGISTER, sndBuffer, iframe, rcvBuffer, NO_READ_BACK);


  // ** Second JTAG operation is to shift out the data register...
  // **Clean buffers**
  for (int i=0; i<MAX_BUFFER_SIZE; i++) {
    sndBuffer[i] = 0;
    rcvBuffer[i] = 0;
  }
  iframe = 0;                              //reset frame counter

  // ** Set up TMS to shift in the data bits for this chip, BYPASS code for others **
  int offset;

  for (idevice=0; idevice<devices_in_chain_; idevice++) {  // loop over all of the chips in this chain

    ichip = devices_in_chain_ - idevice - 1;               // chip order in chain is reversed

    if (ichip == chip_id_) {                               // this is the chip we want
      offset = iframe;                                     // here is the beginning of the data
      for (ibit=0; ibit<register_length_; ibit++)          // up to the number of bits specified for this register
	tdi[iframe++] = write_data[ibit];                  // Shift in the data for TDI
    } else {                                               // bypass register is one frame      
      tdi[iframe++] = 0;                                   // No data goes out to bypass regs
    }
  }

  if (iframe > MAX_NUM_FRAMES) 
    (*MyOutput_) << "EMUjtag: ShfIR_ShfDR DR ERROR: Too many frames -> " << iframe << std::endl;

  //pack tdi into an array of char so scan can handle it:
  packCharBuffer(tdi,iframe,sndBuffer);

  //  (*MyOutput_) << "write_data  = ";
  //  for (int i=register_length_-1; i>=0; i--)
  //    (*MyOutput_) << write_data[i];
  //  (*MyOutput_) << std::endl;

  //  (*MyOutput_) << "TDI into DR = ";
  //  for (int i=iframe-1; i>=0; i--)
  //    (*MyOutput_) << tdi[i];
  //  (*MyOutput_) << std::endl;

  //  (*MyOutput_) << "sndBuffer to ShfDR=";
  //  for (int i=iframe/8-1; i>=0; i--) 
  //    (*MyOutput_) << ((sndBuffer[i] >> 4) & 0xf) << (sndBuffer[i] & 0xf);  
  //  (*MyOutput_) << std::endl;
  
  tmb_->scan(DATA_REGISTER, sndBuffer, iframe, rcvBuffer, READ_BACK);


  // ** copy relevant section of tdo to data array **
  unpackCharBuffer(rcvBuffer,register_length_,offset,shfDR_tdo_);

  //  (*MyOutput_) << "TDO from DR = ";
  //  for (int i=register_length_-1; i>=0; i--)
  //    (*MyOutput_) << shfDR_tdo_[i];
  //  (*MyOutput_) << std::endl;

  //  char tempBuffer[MAX_BUFFER_SIZE];
  //  packCharBuffer(shfDR_tdo_,register_length_,tempBuffer);
  //  for (int i=(register_length_/8)-1; i>=0; i--) 
  //    (*MyOutput_) << ((tempBuffer[i] >> 4) & 0xf) << (tempBuffer[i] & 0xf);  
  //  (*MyOutput_) << std::endl;

  return;
}
//
void EMUjtag::ShfIR_ShfDR(const int selected_chip, 
			  const int opcode, 
			  const int size_of_register) {
  int all_zeros[MAX_NUM_FRAMES] = {};  // Shift in all 0's on tdi for read-only registers
  ShfIR_ShfDR(selected_chip,
	      opcode,
	      size_of_register,
	      all_zeros);
  return;
}
//
void EMUjtag::packCharBuffer(int * bitVector, 
			     int Nbits, 
			     char * charVector) {

  // pack array of bits in "bitVector" (of size "Nbits") into an array 
  // of char "charVector"...
  // charVector is packed such that the index reads from right to left, i.e., 
  // bitVector[0]     = LSB of charVector[0]
  // bitVector[Nbits] = MSB of charVector[Nbits/8]

  for (int i=0; i<Nbits/8; i++) {
    charVector[i] = 0;
  }

  int bufferbit = 0;
  int bufferctr = 0;
  for (int bit=0; bit<Nbits; bit++) {
    if (bufferbit==8) {                                  //end of this character in charVector
      bufferctr++;                                       //increment the number of characters in charVector
      bufferbit = 0;                                     //reset character filling
    }
    charVector[bufferctr] |= ((bitVector[bit]&0xff) << bufferbit);          //fill the charVector
    bufferbit++;
  }
 return; 
}
//
void EMUjtag::unpackCharBuffer(char * buffer, 
			       int length, 
			       int firstBit, 
			       int * bitVector) {
  // project specific bits from "buffer" into array of bits "bitVector" 
  // project "length" bits beginning at "firstBit" (counting from LSB)
  // In other words, "buffer" is unpacked such that: 
  // counting from LSB, the "firstBit" bit of "buffer" = bitVector[0]  
  // counting from LSB, the "firstBit+length" bit of "buffer" = bitVector[length-1] 


  int k=0;
  int ival;

  int bit=0;
  int bufferctr=0;
  char data;
  data = buffer[bufferctr];
  for(int i=0; i < firstBit+length; i++) {
    ival = data & 0x01;
    data >>= 1;
    bit++;
    if(bit==8) { 
      bit=0;
      bufferctr++;
      data = buffer[bufferctr];
    }
    if (i>=firstBit) bitVector[k++]=ival;      //This is the data to keep
  }

  return;
}
//
void EMUjtag::setup_jtag(int chain) {

  //This member sets the following characteristics:
  //  - which JTAG chain you are looking at
  //  - how many chips are on the chain
  //  - the number of bits in each chip's opcode
  //  - chip_id_ to dummy value (in excess of possible number of devices on chain)
  //  - register length to zero 
  //start(N):
  //  - set the jtag chain for the boot register used in VMEController_jtag::scan(...)

  std::cout << "setup_chain" << std::endl ;
  
  jtag_chain_ = chain;
  devices_in_chain_ = 0;
  for (int device=0; device<MAX_NUM_DEVICES; device++)
    bits_in_opcode_[device] = 0;

  if (jtag_chain_ == ChainAlctSlowFpga) {
    devices_in_chain_ = NumberChipsAlctSlowFpga; 
    bits_in_opcode_[0] = OpcodeSizeAlctSlowFpga;
    tmb_->start(6);
  } else if (jtag_chain_ == ChainAlctSlowProm) {           //288, 384, or 672
    devices_in_chain_ = NumberChipsAlctSlowProm;
    bits_in_opcode_[0] = OpcodeSizeAlctSlowProm;
    bits_in_opcode_[1] = OpcodeSizeAlctSlowProm;
    bits_in_opcode_[2] = OpcodeSizeAlctSlowProm;
    tmb_->start(8);
  } else if (jtag_chain_ == ChainTmbMezz) { 
    devices_in_chain_ = NumberChipsTmbMezz;
    bits_in_opcode_[0] = OpcodeSizeTmbMezzFpga;
    bits_in_opcode_[1] = OpcodeSizeTmbMezzProm;
    bits_in_opcode_[2] = OpcodeSizeTmbMezzProm;
    bits_in_opcode_[3] = OpcodeSizeTmbMezzProm;
    bits_in_opcode_[4] = OpcodeSizeTmbMezzProm;
    tmb_->start(3);
  } else if (jtag_chain_ == ChainTmbUser) { 
    devices_in_chain_ = NumberChipsTmbUser;
    bits_in_opcode_[0] = OpcodeSizeTmbUserProm;
    bits_in_opcode_[1] = OpcodeSizeTmbUserProm;
    tmb_->start(4);
  } else if (jtag_chain_ == ChainRat) {
    devices_in_chain_ = NumberChipsRat;
    bits_in_opcode_[0] = OpcodeSizeRatFpga;
    bits_in_opcode_[1] = OpcodeSizeRatProm;
    tmb_->start(10);
  }

  if (devices_in_chain_ == 0) {
    (*MyOutput_) << "EMUjtag: Unsupported JTAG chain " << jtag_chain_ <<std::endl;
    jtag_chain_ = -1;
    ::sleep(5);    
  }  else {

    tmb_->RestoreIdle();      //Valid JTAG chain:  bring the state machine to Run-Time Idle

    (*MyOutput_) << "EMUjtag: JTAG chain " << std::hex << jtag_chain_ 
		 << " has " << std::dec << devices_in_chain_ << " devices" << std::endl;
  }

  chip_id_ = MAX_NUM_DEVICES;
  register_length_ = 0;
  
  return;
}

