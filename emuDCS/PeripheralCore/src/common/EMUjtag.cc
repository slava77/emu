#include "EMUjtag.h"
//
#include <iostream>
#include <iomanip>
#include <unistd.h> // for sleep
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <time.h>
//
#include "TMB.h"
#include "TMB_constants.h"
#include "VMEController.h"
//
//
//EMUjtag::EMUjtag(){
//}
//
EMUjtag::EMUjtag(TMB * tmb) :
  tmb_(tmb),debug_(0)
{
  //
  std::cout << "Creating EMUjtag" << std::endl ;
  //
  MyOutput_ = &std::cout ;
  //
  SetWriteToDevice_(true);     //normal JTAG operation writes to the device
  jtag_chain_ = -1;
  //
  // Defaults for prom programming
  SetXsvfFilename("dummy");
  file_io_ = false;
  which_user_prom_ = -1;
  xdr_length_ = 0;
  xruntest_time_ = 0;
  xrepeat_times_ = 0;
  //
  alct_type_for_prom_ = 0;
  //
};
//
EMUjtag::~EMUjtag() {
  //
  //
}
//////////////////////////////////////////////////////////////////////////////////////////////
// Set up your JTAG interface
//////////////////////////////////////////////////////////////////////////////////////////////
void EMUjtag::setup_jtag(int chain) {
  //
  //This member sets the following characteristics:
  //  - gives control of the JTAG chain to the FPGA
  //  - which JTAG chain you are looking at
  //  - how many chips are on the chain
  //  - the number of bits in each chip's opcode
  //  - register length to zero 
  //start(N,source):
  //  - set the jtag chain through the TMB using the jtagSourceBoot or jtagSourceFPGA
  //
  if(debug_){
    std::cout << "setup_chain" << std::endl ;
  }
  // set the bit on the bootstrap register to set the source of the JTAG chain to the the FPGA:
  // If start(N,source) is called with source=jtagSourceBoot, this call will override this setting here...
  if (GetWriteToDevice_()) {
    short unsigned int BootReg;
    tmb_->tmb_get_boot_reg(&BootReg);
    tmb_->tmb_set_boot_reg(BootReg & 0xff7f);
  }
  //
  jtag_chain_ = chain;
  devices_in_chain_ = 0;
  //
  for (int device=0; device<MAX_NUM_DEVICES; device++)
    bits_in_opcode_[device] = 0;
  //
  if (jtag_chain_ == ChainAlctSlowFpga) {
    //
    devices_in_chain_ = NumberChipsAlctSlowFpga; 
    bits_in_opcode_[0] = OpcodeSizeAlctSlowFpga;
    if (GetWriteToDevice_()) tmb_->start(6,jtagSourceFPGA);
    //
  } else if (jtag_chain_ == ChainAlctSlowMezz) {
    //
    devices_in_chain_ = NumberChipsAlctSlowMezz;
    bits_in_opcode_[0] = OpcodeSizeAlctSlowMezzFpga;
    bits_in_opcode_[1] = OpcodeSizeAlctSlowMezzProm;
    bits_in_opcode_[2] = OpcodeSizeAlctSlowMezzProm;
    if (GetWriteToDevice_()) tmb_->start(7,jtagSourceFPGA);
    //
  } else if (jtag_chain_ == ChainAlctFastFpga) {
    //
    devices_in_chain_ = NumberChipsAlctFastFpga; 
    bits_in_opcode_[0] = OpcodeSizeAlctFastFpga;
    if (GetWriteToDevice_()) tmb_->start(8,jtagSourceFPGA);
    //
  } else if (jtag_chain_ == ChainAlctFastMezz) {
    //
    devices_in_chain_ = NumberChipsAlctFastMezz; 
    bits_in_opcode_[0] = OpcodeSizeAlctFastMezzFpga;
    bits_in_opcode_[1] = OpcodeSizeAlctFastMezzProm;
    bits_in_opcode_[2] = OpcodeSizeAlctFastMezzProm;
    if (GetWriteToDevice_()) tmb_->start(2,jtagSourceFPGA);
    //
  } else if (jtag_chain_ == ChainTmbMezz) { 
    //
    devices_in_chain_ = NumberChipsTmbMezz;
    bits_in_opcode_[0] = OpcodeSizeTmbMezzFpga;
    bits_in_opcode_[1] = OpcodeSizeTmbMezzProm;
    bits_in_opcode_[2] = OpcodeSizeTmbMezzProm;
    bits_in_opcode_[3] = OpcodeSizeTmbMezzProm;
    bits_in_opcode_[4] = OpcodeSizeTmbMezzProm;
    // For programming the TMB mezzanine proms, we want to use the bootstrap register 
    // After the TMB has been programmed, the FPGA can control the JTAG chain....
    if (GetWriteToDevice_()) tmb_->start(3,jtagSourceBoot);
    //
  } else if (jtag_chain_ == ChainTmbUser) { 
    //
    devices_in_chain_ = NumberChipsTmbUser;
    bits_in_opcode_[0] = OpcodeSizeTmbUserProm;
    bits_in_opcode_[1] = OpcodeSizeTmbUserProm;
    if (GetWriteToDevice_()) tmb_->start(4,jtagSourceFPGA);
    //
  } else if (jtag_chain_ == ChainRat) {
    //
    devices_in_chain_ = NumberChipsRat;
    bits_in_opcode_[0] = OpcodeSizeRatFpga;
    bits_in_opcode_[1] = OpcodeSizeRatProm;
    if (GetWriteToDevice_()) tmb_->start(10,jtagSourceFPGA);
    //
  }
  //
  if (devices_in_chain_ == 0) {
    (*MyOutput_) << "EMUjtag: Unsupported JTAG chain " << jtag_chain_ <<std::endl;
    jtag_chain_ = -1;
    ::sleep(5);    
  }  else {
    //
    if (GetWriteToDevice_()) tmb_->RestoreIdle();      //Valid JTAG chain:  bring the state machine to Run-Time Idle
    //
    if(debug_){
      (*MyOutput_) << "EMUjtag: JTAG chain " << std::hex << jtag_chain_ 
		   << " has " << std::dec << devices_in_chain_ << " devices" << std::endl;
    }
  }
  //
  register_length_ = 0;
  //
  return;
}
//
void EMUjtag::SetXsvfFilename(std::string filename) {
  //
  filename_dat_ = AddTrailer_(filename,"dat");  
  filename_xsvf_ = AddTrailer_(filename,"xsvf");  
  filename_log_ = AddTrailer_(filename,"log");  
  //
  file_io_ = true;
  //
  return;
}
//
void EMUjtag::ClearXsvfFilename() {
  //
  SetXsvfFilename("dummy");
  //
  file_io_ = false;
  //
  return;
}
//
void EMUjtag::SetWhichUserProm(int device) {
  //
  // Which User prom are you writing the xsvf file for?
  //
  which_user_prom_ = device;
  return;
}
//
int EMUjtag::GetWhichUserProm() {
  //
  return which_user_prom_;
}
//
void EMUjtag::SetWriteToDevice_(bool communicate) {
  //
  // Enable communication with the physical device:
  //
  write_to_device_ = communicate;
  return;
}
//
bool EMUjtag::GetWriteToDevice_() {
  //
  return write_to_device_;
}
//
//
//////////////////////////////////////////////////////////////////////////////////////////////
// Use EMUjtag to shift data into Instruction Registers and into (and out of) Data Registers
//////////////////////////////////////////////////////////////////////////////////////////////
void EMUjtag::ShfIR_(const int selected_chip, 
		     const int opcode) {
  //
  // This function sets up the bits of the "opcode" to write to the Instruction 
  // Register for the "selected_chip" on jtag_chain_ (setup_jtag()).  
  //
  // N.B. JTAG instructions and data are loaded and readout in order
  //      starting with chipN, chipN-1, ... ,chip1, and ending with chip0
  //
  if (debug_){    
    (*MyOutput_) << "EMUjtag: ShfIR -> Use " << std::dec << bits_in_opcode_[selected_chip] 
		 << " bits to write opcode 0x" << std::hex << opcode 
		 << " to chip " << std::dec << selected_chip 
		 << " on chain 0x" << std::hex << jtag_chain_ << std::endl;
  }
  //
  // Have you run setup_jtag?
  if (jtag_chain_ < 0) return;
  //
  //** Clear the bit buffers:
  for (int i=0; i<MAX_NUM_FRAMES; i++) {
    tdi_in_bits_[i] = 0;
    chip_tdo_in_bits_[i] = 0;
  }
  //
  // **Clear the byte buffers**
  for (int i=0; i<MAX_BUFFER_SIZE; i++) {
    tdi_in_bytes_[i] = 0;
    tdo_in_bytes_[i] = 0;
  }
  //
  int iframe = 0;                              //reset frame counter
  //
  // ** Construct opcode for the selected chip (all but chip_id are BYPASS = all 1's),
  for (int idevice=0; idevice<devices_in_chain_; idevice++) {          //loop over all the chips in this chain
    //
    int ichip = devices_in_chain_ - idevice - 1;                       //chip order in chain is reversed
    //
    for (int ibit=0; ibit<bits_in_opcode_[ichip]; ibit++) {            //up to the number of bits in this chip's opcode
      int bit = 1;                                                     //BYPASS
      if (ichip == selected_chip)                                  //this is the chip we want
	bit = (opcode >> ibit) & 0x1;                              //extract bit from opcode
      tdi_in_bits_[iframe++]=bit;
    }
  }
  //
  //  (*MyOutput_) << "There are " << std::dec << iframe << " frames to send..." << std::endl;
  //
  if (iframe > MAX_NUM_FRAMES) 
    (*MyOutput_) << "EMUjtag: ShfIR ERROR: Too many frames -> " << iframe << std::endl;
  //
  //pack tdi into an array of char so scan can handle it:
  packCharBuffer(tdi_in_bits_,iframe,tdi_in_bytes_);
  //
  if (debug_ >= 10) {
    (*MyOutput_) << "tdi bytes to ShfIR=";
    for (int i=iframe/8; i>=0; i--) 
      (*MyOutput_) << " " << std::hex << (tdi_in_bytes_[i]&0xff); 
    (*MyOutput_) << std::endl;
  }
  //
  if (GetWriteToDevice_()) tmb_->scan(INSTR_REGISTER, tdi_in_bytes_, iframe, tdo_in_bytes_, NO_READ_BACK);
  //
  return;
}
//
void EMUjtag::ShfDR_(const int selected_chip, 
		     const int size_of_register, 
		     const int * write_data) {
  //
  // This function shifts in "write_data" on tdi to the Data Register 
  // (one bit per index).  Clearly one bit per index is not very memory 
  // efficient, but it allows for easy manipulation by the user.  For 
  // read-only JTAG, this array should be all zeros.  The number of bits 
  // shifted in on tdi or out on tdo is "size_of_register".
  //
  // The data which is shifted out of the data register is put into the 
  // array "chip_tdo_in_bits_" (also one bit per index).   
  //
  // N.B. JTAG instructions and data are loaded and readout in order
  //      starting with chipN, chipN-1, ... ,chip1, and ending with chip0
  //
  //Have you run setup_jtag?
  //
  if (jtag_chain_ < 0) return;
  //
  register_length_ = size_of_register;
  //
  if (debug_){    
    (*MyOutput_) << "EMUjtag: ShfDR -> Write data to chip " << std::dec << selected_chip 
		 << " on chain 0x" << std::hex << jtag_chain_
		 << " using " << std::dec << register_length_ 
		 << " bits tdi/tdo" << std::hex << std::endl;
  }
  //
  //** Clear the bit buffers:
  for (int i=0; i<MAX_NUM_FRAMES; i++) {
    tdi_in_bits_[i] = 0;
    chip_tdo_in_bits_[i] = 0;
  }
  //
  // **Clear the byte buffers**
  for (int i=0; i<MAX_BUFFER_SIZE; i++) {
    tdi_in_bytes_[i] = 0;
    tdo_in_bytes_[i] = 0;
  }
  //
  int iframe = 0;                              //reset frame counter
  //
  // ** Shift in the data bits for the selected chip, BYPASS code for others **
  int offset;
  //
  for (int idevice=0; idevice<devices_in_chain_; idevice++) {  // loop over all of the chips in this chain
    //
    int ichip = devices_in_chain_ - idevice - 1;               // chip order in chain is reversed
    //
    if (ichip == selected_chip) {                              // this is the chip we want
      offset = iframe;                                         // here is the beginning of the data
      for (int ibit=0; ibit<register_length_; ibit++)          // up to the number of bits specified for this register
	tdi_in_bits_[iframe++] = write_data[ibit];             // Shift in the data for TDI
    } else {                                                   // bypass register is one frame      
      tdi_in_bits_[iframe++] = 0;                              // No data goes out to bypass regs
    }
  }
  //
  if (iframe > MAX_NUM_FRAMES) 
    (*MyOutput_) << "EMUjtag: ShfDR ERROR: Too many frames -> " << iframe << std::endl;
  //
  //pack tdi into an array of char so scan can handle it:
  packCharBuffer(tdi_in_bits_,iframe,tdi_in_bytes_);
  //
  if (debug_ >= 10) {
    (*MyOutput_) << "write_data  = ";
    for (int i=register_length_-1; i>=0; i--)
      (*MyOutput_) << write_data[i];
    (*MyOutput_) << std::endl;
    //
    (*MyOutput_) << "TDI in bits into DR = ";
    for (int i=iframe-1; i>=0; i--)
      (*MyOutput_) << tdi_in_bits_[i];
    (*MyOutput_) << std::endl;
    //
    (*MyOutput_) << "TDI in bytes into DR=";
    for (int i=iframe/8; i>=0; i--) 
      (*MyOutput_) << ((tdi_in_bytes_[i] >> 4) & 0xf) << (tdi_in_bytes_[i] & 0xf);  
    (*MyOutput_) << std::endl;
  }
  //
  if (GetWriteToDevice_()) tmb_->scan(DATA_REGISTER, tdi_in_bytes_, iframe, tdo_in_bytes_, READ_BACK);
  //
  // ** Copy relevant section of tdo to bit-wise data array.  That is,
  //     => chip_tdo_in_bits_  = tdo only for chip we are interested in
  //     => tdo_in_bytes_ = tdo for full chain (including bypass bits)
  unpackCharBuffer(tdo_in_bytes_,register_length_,offset,chip_tdo_in_bits_);
  //
  if (debug_ >= 10) {
    (*MyOutput_) << "TDO in bits from DR = ";
    for (int i=register_length_-1; i>=0; i--)
      (*MyOutput_) << chip_tdo_in_bits_[i];
    (*MyOutput_) << std::endl;
    //
    (*MyOutput_) << "TDO in bytes from DR = ";
    char tempBuffer[MAX_BUFFER_SIZE];
    packCharBuffer(chip_tdo_in_bits_,register_length_,tempBuffer);
    for (int i=(register_length_/8)-1; i>=0; i--) 
      (*MyOutput_) << ((tempBuffer[i] >> 4) & 0xf) << (tempBuffer[i] & 0xf);  
    (*MyOutput_) << std::endl;
  }
  //
  return;
}
//
void EMUjtag::ShfIR_ShfDR(const int selected_chip, 
			  const int opcode, 
			  const int size_of_register, 
			  const int * write_data) {
  //
  // Enable ability to fill user prom with data:
  tmb_->OkVmeWrite(vme_usr_jtag_adr);    
  //
  // Enable communication with the physical device:
  SetWriteToDevice_(true);
  //
  // ** First JTAG operation is to shift in the opcode to the instruction register...
  ShfIR_(selected_chip,opcode);
  //
  // ** Second JTAG operation is to shift tdi into and tdo out of the data register...
  ShfDR_(selected_chip,size_of_register,write_data);
  //
  return;
}
//
void EMUjtag::ShfIR_ShfDR(const int selected_chip, 
			  const int opcode, 
			  const int size_of_register) {
  int all_zeros[MAX_NUM_FRAMES] = {};  // Shift in all 0's on tdi for read-only registers
  //
  ShfIR_ShfDR(selected_chip,
	      opcode,
	      size_of_register,
	      all_zeros);
  return;
}
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////
// Check if JTAG read values = JTAG write values:
///////////////////////////////////////////////////////////////////////////////////////////////////
void EMUjtag::CompareBitByBit(int * write_vector,
			      int * read_vector,
			      int length) {
  int error_counter=0;
  //
  for (int i=0; i<length; i++)
    if (write_vector[i] != read_vector[i]) 
      error_counter++;
  //
  if (error_counter != 0) {
    //  Pack up the vectors into buffers of characters to send to the error output location:
    std::ostringstream writedump;
    char WriteBuffer[MAX_BUFFER_SIZE];
    packCharBuffer(write_vector,length,WriteBuffer);
    for (int i=(length-1)/8; i>=0; i--) 
      writedump << std::hex << ((WriteBuffer[i] >> 4) & 0xf) << (WriteBuffer[i] & 0xf);  
    //
    std::ostringstream readdump;
    char ReadBuffer[MAX_BUFFER_SIZE];
    packCharBuffer(read_vector,length,ReadBuffer);
    for (int i=(length-1)/8; i>=0; i--) 
      readdump << std::hex << ((ReadBuffer[i] >> 4) & 0xf) << (ReadBuffer[i] & 0xf);  
    //
    std::ostringstream dump;
    dump << "EMUjtag: JTAG READ ->" << readdump.str() << " does not equal WRITE->" << writedump.str();
    //
    (*MyOutput_) << "EMUjtag: JTAG READ ->" << readdump.str() 
		 << " does not equal WRITE->" << writedump.str() << std::endl;
    //
    tmb_->SendOutput(dump.str(),"ERROR");
  }
  return;
}
//
////////////////////////////////////////////////////////////////////////////////////////////////////
// Useful functions to convert one type into another type
////////////////////////////////////////////////////////////////////////////////////////////////////
void EMUjtag::packCharBuffer(int * bitVector, 
			     int Nbits, 
			     char * charVector) {
  // pack array of bits in "bitVector" (of size "Nbits") into an array 
  // of char "charVector"...
  // charVector is packed such that the index reads from right to left, i.e., 
  // bitVector[0]     = LSB of charVector[0]
  // bitVector[Nbits] = MSB of charVector[Nbits/8]
  //
  for (int i=0; i<Nbits/8; i++) 
    charVector[i] = 0;
  //
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
  //
  int k=0;
  int ival;
  //
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
  //
  return;
}
//
int EMUjtag::bits_to_int(int * bits,
			 int length,
			 int MsbOrLsb) {
  //convert vector of "bits" into an integer
  // MsbOrLsb = 0 for LSB first -> bits[0] = LSB of integer, bits[length] = MSB of integer
  //          = 1 for MSB first -> bits[0] = MSB of integer, bits[length] = LSB of integer
  //
  if (length>32) {
    (*MyOutput_) << "bits_to_int ERROR: Too many bits -> " << length << std::endl;
    return 0;
  }
  //
  int ibit;
  int value = 0;
  if (MsbOrLsb == 0) {       // Translate LSB first    
    for (ibit=0; ibit<length; ibit++) 
      value |= ((bits[ibit]&0x1) << ibit);
  } else {                   // Translate MSB first
    for (ibit=0; ibit<length; ibit++) 
      value |= ((bits[length-ibit-1]&0x1) << ibit);
  }
  //
  if (debug_ >= 5)
    (*MyOutput_) << "bits_to_int output value = " << std::hex << value << std::endl;
  //
  return value;
}
//
void EMUjtag::int_to_bits(int value,
			  int length,
			  int * bits,
			  int MsbOrLsb) {
  // expand integer "value" into first "length" slots of the vector of "bits"
  // MsbOrLsb = 0 for LSB first -> bits[0] = LSB of "value", bits[length] = MSB of "value"
  //          = 1 for MSB first -> bits[length] = LSB of "value", bits[0] = MSB of "value"
  //
  for (int ibit=0; ibit<length; ibit++) {
    if (MsbOrLsb == 0) {       // Translate LSB first    
      bits[ibit] = (value >> ibit) & 0x1;
    } else {                   // Translate MSB first
      bits[ibit] = (value >>(length-ibit-1)) & 0x1;
    }
    //    (*MyOutput_) << "bits[" << ibit << "] = " << bits[ibit] << std::endl;
  }
  //
  return;
}
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
// XSVF programming.....
////////////////////////////////////////////////////////////////////////////////////////////////////
void EMUjtag::CheckAndProgramProm(int which_prom) {
  //
  // If ( which_prom == ChipLocationTmbUserPromTMBClear or ChipLocationTmbUserPromALCTClear),
  //    this method will clear the user proms with values to make it pass the "TMB User PROM Test"
  // If ( which_prom == ChipLocationTmbUserPromTMB or ChipLocationTmbUserPromALCT),
  //   This method assumes you have:
  //    a) SetFillVmeWriteVecs(true);
  //    b) written to the registers you want to be configured...
  //
  read_ascii_prom_image_ = new int[TOTAL_NUMBER_OF_ADDRESSES];
  write_ascii_prom_image_ = new int[TOTAL_NUMBER_OF_ADDRESSES];
  SetWhichUserProm(which_prom);
  CreateUserPromFile();
  //
  CheckUserProm();          //is the prom already programmed with this configuration?
  //    
  while ( GetNumberOfVerifyErrors() != 0 ) { 
    CreateXsvfFile();
    ProgramUserProm();
    CheckUserProm();
  }
  //
  delete read_ascii_prom_image_;
  delete write_ascii_prom_image_;
  return;
}
//
//----------------------------------//
// prom image file create/read
//----------------------------------//
void EMUjtag::CreateUserPromFile() {
  //
  (*MyOutput_) << "EMUjtag:  Creating user prom image file" << std::endl;
  //
  // Create the data which is loaded to the TMB or ALCT
  //
  int data_to_prom[TOTAL_NUMBER_OF_ADDRESSES] = {};
  int address_counter = 0;
  //
  if (GetWhichUserProm() == ChipLocationTmbUserPromTMB) {
    //
    // VME state machine on TMB requires the data to be written into the prom in the following format:
    //
    TmbUserVmeAddress = tmb_->GetVecVmeAddress();
    TmbUserDataLsb = tmb_->GetVecDataLsb();
    TmbUserDataMsb = tmb_->GetVecDataMsb();
    //
    for (unsigned int data_counter=0; data_counter<TmbUserVmeAddress.size(); data_counter++) {
      data_to_prom[address_counter++] = TmbUserVmeAddress.at(data_counter);
      data_to_prom[address_counter++] = 0x00;
      data_to_prom[address_counter++] = 0x00;
      data_to_prom[address_counter++] = TmbUserDataLsb.at(data_counter);
      data_to_prom[address_counter++] = TmbUserDataMsb.at(data_counter);
    }
    //
  } else if (GetWhichUserProm() == ChipLocationTmbUserPromALCT) {
    //
    // JTAG state machine on TMB requires the data to be written into the prom in the following format:
    //
    AlctUserDataLsb = tmb_->GetVecDataLsb();
    //
    for (unsigned int data_counter=0; data_counter<AlctUserDataLsb.size(); data_counter++) 
      data_to_prom[address_counter++] = (int) (AlctUserDataLsb.at(data_counter) & 0x7f);
    //
  } else if (GetWhichUserProm() == ChipLocationTmbUserPromTMBClear) {
    //
    data_to_prom[address_counter++] = 0xab;
    data_to_prom[address_counter++] = 0x01;
    data_to_prom[address_counter++] = 0x02;
    data_to_prom[address_counter++] = 0x04;
    data_to_prom[address_counter++] = 0x08;
    data_to_prom[address_counter++] = 0x10;
    data_to_prom[address_counter++] = 0x20;
    data_to_prom[address_counter++] = 0x40;
    data_to_prom[address_counter++] = 0x80;
    data_to_prom[address_counter++] = 0xee;
    //
  } else if (GetWhichUserProm() == ChipLocationTmbUserPromALCTClear) {
    data_to_prom[address_counter++] = 0xcd;
    data_to_prom[address_counter++] = 0x01;
    data_to_prom[address_counter++] = 0x02;
    data_to_prom[address_counter++] = 0x04;
    data_to_prom[address_counter++] = 0x08;
    data_to_prom[address_counter++] = 0x10;
    data_to_prom[address_counter++] = 0x20;
    data_to_prom[address_counter++] = 0x40;
    data_to_prom[address_counter++] = 0x80;
    data_to_prom[address_counter++] = 0xbb;
    //
  } else {
    (*MyOutput_) << "EMUjtag:  CreateUserPromFile ERROR User Prom " << std::dec << GetWhichUserProm()
		 << "does not exist... SetWhichUserProm to a valid user prom" << std::endl;
    return;
    //
  }
  //
  data_word_count_ = address_counter;
  //
  InsertHeaderAndTrailer_(data_to_prom);
  //
  if (file_io_)
    WritePromDataToDisk_();
  //
  return;
}
//
void EMUjtag::InsertHeaderAndTrailer_(int * data_to_go_into_prom) {
  //
  // Fill the ascii prom image with the desired data....
  //
  // clear the ascii prom image:
  for (int address_counter=0; address_counter<TOTAL_NUMBER_OF_ADDRESSES; address_counter++) 
    SetUserPromImage_(address_counter,0xff);
  //
  // Need the date information to time stamp the header
  //
  time_t rawtime;
  struct tm * timeinfo;
  //
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  //
  int month_counting_from_one = timeinfo->tm_mon + 1;
  int day = timeinfo->tm_mday;
  int yearAD = timeinfo->tm_year + 1900;
  //
  int tens_digit_month_counting_from_one = month_counting_from_one / 10;
  int ones_digit_month_counting_from_one = month_counting_from_one % 10;
  int month_in_hex = ( (tens_digit_month_counting_from_one & 0xf) << 4 ) | (ones_digit_month_counting_from_one & 0xf);
  //
  int tens_digit_day = day / 10;
  int ones_digit_day = day % 10;
  int day_in_hex = ( (tens_digit_day & 0xf) << 4 ) | (ones_digit_day & 0xf);
  //
  int thousands_digit_yearAD = yearAD / 1000;
  int hundreds_digit_yearAD = (yearAD / 100) % 10;
  int msb_year_in_hex = ( (thousands_digit_yearAD & 0xf) << 4 ) | (hundreds_digit_yearAD & 0xf);
  //
  int tens_digit_yearAD = (yearAD / 10) % 10;
  int ones_digit_yearAD = yearAD % 10;
  int lsb_year_in_hex = ( (tens_digit_yearAD & 0xf) << 4 ) | (ones_digit_yearAD & 0xf);
  //
  int address_counter = 0;
  //
  //
  // Header:
  if (GetWhichUserProm() == ChipLocationTmbUserPromTMB) {
    //
    SetUserPromImage_(address_counter++,
		      0xBC);              // Begin CLCT header marker, if missing state machine stops
    address_counter++;                    // these two values are the total word count, to be 
    address_counter++;                    // filled in at the end....
    SetUserPromImage_(address_counter++,
		      month_in_hex);      // date
    SetUserPromImage_(address_counter++,
		      day_in_hex);        // date
    SetUserPromImage_(address_counter++,
		      lsb_year_in_hex);   // date
    SetUserPromImage_(address_counter++,
		      msb_year_in_hex);   // date
    int version = 1;                      // needs to come from database or xml
    SetUserPromImage_(address_counter++,
		      version);           //version
    int option1 = 0xaa;
    int option2 = 0x55;
    SetUserPromImage_(address_counter++,
		      option1);           // future
    SetUserPromImage_(address_counter++,
		      option2);           // future
    SetUserPromImage_(address_counter++,
		      option1);           // future
    SetUserPromImage_(address_counter++,
		      option2);           // future
    SetUserPromImage_(address_counter++,
		      option1);           // future
    SetUserPromImage_(address_counter++,
		      option2);
    SetUserPromImage_(address_counter++,
		      option1);
    SetUserPromImage_(address_counter++,
		      0xEC);               //end of header marker
    //
  } else if (GetWhichUserProm() == ChipLocationTmbUserPromALCT) {
    //
    SetUserPromImage_(address_counter++,
		      0xBA);                // Begin ALCT header marker, if missing state machine stops
    //
    int hundreds_digit_alct_type = GetAlctTypeForProm() / 100;
    int tens_digit_alct_type     = GetAlctTypeForProm() / 10 % 10;
    int ones_digit_alct_type     = GetAlctTypeForProm() % 10;
    SetUserPromImage_(address_counter++,
		      0x80 | (hundreds_digit_alct_type & 0xf) ); 
    SetUserPromImage_(address_counter++,
		      0x80 | (tens_digit_alct_type & 0xf) ); 
    SetUserPromImage_(address_counter++,
		      0x80 | (ones_digit_alct_type & 0xf) );
    // 
    SetUserPromImage_(address_counter++,
		      0x80 | tens_digit_month_counting_from_one); //date
    SetUserPromImage_(address_counter++,
		      0x80 | ones_digit_month_counting_from_one); //date
    SetUserPromImage_(address_counter++,
		      0x80 | tens_digit_day);                     //date
    SetUserPromImage_(address_counter++,
		      0x80 | ones_digit_day);                     //date
    SetUserPromImage_(address_counter++,
		      0x80 | thousands_digit_yearAD);             //date
    SetUserPromImage_(address_counter++,
		      0x80 | hundreds_digit_yearAD);              //date
    SetUserPromImage_(address_counter++,
		      0x80 | tens_digit_yearAD);                  //date
    SetUserPromImage_(address_counter++,
		      0x80 | ones_digit_yearAD);                  //date
    int version = 1;                      // needs to come from database or xml
    SetUserPromImage_(address_counter++,
		      0x80 | version);           //version
    int future = 0;                     
    SetUserPromImage_(address_counter++,
		      0x80 | future);            //future use
    SetUserPromImage_(address_counter++,
		      0x80 | future);            //future use
    SetUserPromImage_(address_counter++,
		      0xAA);                     //ALCT end of header marker
  }
  //
  //
  // Data:
  int data_counter = 0;
  while ( data_counter < data_word_count_) 
    SetUserPromImage_(address_counter++,
  		      data_to_go_into_prom[data_counter++]);
  //
  //
  // Trailer:
  if (GetWhichUserProm() == ChipLocationTmbUserPromTMB) {
    //
    SetUserPromImage_(address_counter++,
		      vme_wrt_dat_ck_adr & 0xFF);
    SetUserPromImage_(address_counter++,
		      0x00);
    SetUserPromImage_(address_counter++,
		      0x00);
    SetUserPromImage_(address_counter++,
		      0xAA);
    SetUserPromImage_(address_counter++,
		      0x55);

    SetUserPromImage_(address_counter++,
		      0xFC);               //end of data marker    
    //
    address_counter++;                     //checksum
    //
    SetUserPromImage_(address_counter++,
		      0xFF);               //end of data marker    
    //
  } else if (GetWhichUserProm() == ChipLocationTmbUserPromALCT) {
    //
    // Include a write to the TMB FPGA JTAG chain so that 
    // TMB can verify correct operation of the JTAG state machine:
    //
    SetUserPromImage_(address_counter++,
		      0x60);               //jtag chain = C, tck lo
    //
    SetUserPromImage_(address_counter++,
		      0x64);               //jtag chain = C, tck hi
    //
    SetUserPromImage_(address_counter++,
		      0xFA);               //end of data marker    
    // 
    // Add the word count to the ALCT trailer:
    int word_count = address_counter;
    SetUserPromImage_(address_counter++,
		      0xC0 | ((word_count >> 12) & 0xf) );
    SetUserPromImage_(address_counter++,
		      0xC0 | ((word_count >>  8) & 0xf) );
    SetUserPromImage_(address_counter++,
		      0xC0 | ((word_count >>  4) & 0xf) );
    SetUserPromImage_(address_counter++,
		      0xC0 | ((word_count >>  0) & 0xf) );
    //
    int begin_address = 0;
    int end_address = address_counter - 1;
    int check_sum = ComputeCheckSum_(begin_address,
				     end_address);
    //
    SetUserPromImage_(address_counter++,
		      0xC0 | ((check_sum >> 4) & 0xf) );             
    //
    SetUserPromImage_(address_counter++,
		      0xC0 | ((check_sum >> 0) & 0xf) );             
    //
    SetUserPromImage_(address_counter++,
		      0xFF);               // end of data marker
  }
  //
  // Total number of words in prom image:
  prom_image_word_count_ = address_counter;
  //
  // Put in the total number of bits and checksum:
  if (GetWhichUserProm() == ChipLocationTmbUserPromTMB ) {
    //
    SetUserPromImage_( 1,
		       (prom_image_word_count_ >> 0) & 0xff );
    //
    SetUserPromImage_( 2,
		       (prom_image_word_count_ >> 8) & 0xff );
    //
    int begin_address = 0;
    int end_address = prom_image_word_count_ - 3;
    int check_sum = ComputeCheckSum_(begin_address,
				     end_address);
    //
    SetUserPromImage_( (prom_image_word_count_ - 2),
		       check_sum );	       
  } 
  //
  // Now the prom image has been created, so point the user into the correct prom to make the xsvf file...
  if (GetWhichUserProm() == ChipLocationTmbUserPromTMBClear) {
    SetWhichUserProm(ChipLocationTmbUserPromTMB);
  } else if (GetWhichUserProm() == ChipLocationTmbUserPromALCTClear) {
    SetWhichUserProm(ChipLocationTmbUserPromALCT);
  }
  //
  return;
}
//
int EMUjtag::ComputeCheckSum_(int begin_address,
			      int end_address) {
  int check_sum = 0;
  //
  for (int address=begin_address; address<=end_address; address++) {
    check_sum += write_ascii_prom_image_[address];
    check_sum &= 0xff;
  }
  //
  return check_sum;
}
//
bool EMUjtag::ReadUserPromFile() {
  //
  // Clear the read_ascii_prom_image_ vector:
  for (int address=0; address<TOTAL_NUMBER_OF_ADDRESSES; address++) 
    read_ascii_prom_image_[address]=0xff;
  //
  if (file_io_) {
    (*MyOutput_) << "EMUjtag:  READ user prom image file " << filename_dat_ << std::endl;
    //
    int index_value;
    //
    std::ifstream Readfile;
    Readfile.open(filename_dat_.c_str());
    //
    if ( Readfile.is_open() ) {
      //
      while ( Readfile.good() ) {
	//
	// prom image file has format AAAA DD -> AAAA=address, DD=prom data
	//
	std::string line;
	std::getline(Readfile,line);
	//
	std::istringstream instring(line);
	//
	int image_value;
	instring >> std::hex >> index_value >> image_value;
	//
	read_ascii_prom_image_[index_value] = image_value;
	//
	//      std::cout << "line " << std::dec << index_value 
	//       		<< ", image = 0x" << std::hex <<  GetUserPromImage(index_value)
	//      		<< std::endl;
	//      ::sleep(1);
      } 
      //
    } else {
      //
      (*MyOutput_) << "EMUjtag:  ERROR Prom data file " << filename_dat_ 
		   << " does not exist.  Please create it..." << std::endl;
      return false;
    }
    //
    Readfile.close();
    prom_image_word_count_ = index_value+1;
    //
  } else {
    //
    (*MyOutput_) << "EMUjtag:  READ internal user prom image vector" << std::endl;
    //
    for (int address=0; address<prom_image_word_count_; address++) 
      read_ascii_prom_image_[address] = write_ascii_prom_image_[address];
    //
  }
  //
  return true;
}
//
void EMUjtag::WritePromDataToDisk_() {
  //
  (*MyOutput_) << "EMUjtag: Write file " << filename_dat_ << " to disk" << std::endl;
  //
  std::ofstream file_to_write;
  file_to_write.open(filename_dat_.c_str());
  // 
  // need to include index=prom_image_word_count_ in order to have 0xff be the last word in the file...
  //
  for (int index=0; index<prom_image_word_count_; index++) {
    //
    // prom image file has format AAAA DD -> AAAA=address, DD=prom data
    //
    file_to_write << std::hex 
		  << std::setw(1) 
		  << ( (index >> 12) & 0xf )
		  << ( (index >> 8) & 0xf )
		  << ( (index >> 4) & 0xf )
		  << ( index & 0xf )
		  << std::setw(2) 
		  << ( (write_ascii_prom_image_[index] >> 4) & 0xf )
		  << ( write_ascii_prom_image_[index] & 0xf )
		  << std::endl;
  }
  //
  file_to_write.close();
  return;
}
//
int EMUjtag::GetUserPromImage(int address) {
  //
  if (address >= TOTAL_NUMBER_OF_ADDRESSES) {
    (*MyOutput_) << "GetUserPromImage ERROR: address " << address 
		 << " out of range...  should be between 0 and " << TOTAL_NUMBER_OF_ADDRESSES-1 
		 << std::endl;
    return 999;
  }
  //
  return read_ascii_prom_image_[address] & 0xff;
}
//
void EMUjtag::SetUserPromImage_(int address,
				int value) {
  //
  if (address >= TOTAL_NUMBER_OF_ADDRESSES) {
    (*MyOutput_) << "SetUserPromImage ERROR: address " << address 
		 << " out of range...  should be between 0 and " << TOTAL_NUMBER_OF_ADDRESSES-1 
		 << std::endl;
    return;
  }
  //
  if (value > 0xff) {
    (*MyOutput_) << "SetUserPromImage ERROR: address " << std::hex << address 
		 << " value " << value << " is too large...  should be between 0 and 255" 
		 << std::endl;
    return;
  }
  // 
  write_ascii_prom_image_[address] = value & 0xff;
  //
  return;
}
//
//
//----------------------------------//
// Create XSVF file
//----------------------------------//
void EMUjtag::CreateXsvfFile() {
  //
  (*MyOutput_) << "EMUjtag:  Creating XSVF file for user prom " 
	       << std::dec << GetWhichUserProm() << std::endl;
  //
  if ( !ReadUserPromFile() ) {
    (*MyOutput_) << "EMUjtag:  prom image file not OK... Not creating XSVF file..." << std::endl;
    return;
  }
  //
  // Don't write to a physical device, you are writing to a file:
  SetWriteToDevice_(false);
  //
  if ( !CreateXsvfImage_() ) {
    (*MyOutput_) << "EMUjtag:  xsvf image creation not OK... Not creating XSVF file..." << std::endl;
    return;
  }
  //
  if (file_io_)
    WriteXsvfImageToDisk_();
  //
  // Go back to default jtag mode:
  SetWriteToDevice_(true);
  //
  return;
}
//
bool EMUjtag::CreateXsvfImage_() {
  //
  // Clear the XSVF image:
  //  while (image_counter_ < MAX_XSVF_IMAGE_NUMBER) 
  //    SetWriteXsvfImage_(image_counter_++,0); 
  xsvf_image_.clear();
  //
  // Reset the byte counter:
  image_counter_ = 0;
  //
  // Are we writing to a valid user prom?
  if (GetWhichUserProm() != ChipLocationTmbUserPromTMB && 
      GetWhichUserProm() != ChipLocationTmbUserPromALCT) {
    (*MyOutput_) << "EMUjtag:  ERROR User Prom " << std::dec << GetWhichUserProm()
		 << "does not exist... SetWhichUserProm to a valid user prom" << std::endl;
    return false;
  }
  //
  // Here is the protocol to download the xsvf program into the 2 user proms:
  //
  // Setup the jtag protocol stuff:
  setup_jtag(ChainTmbUser);
  //
  // Preamble needed to program the user prom:
  WritePreambleIntoXsvfImage_();
  //
  // Insert the prom image into the programming structure:
  WritePromImageIntoXsvfImage_();
  //
  WriteInterimBetweenImageAndVerifyIntoXsvfImage_();
  //
  // Insert the prom image into the verification structure:
  // N.B. Remove this step from writing the user prom xsvf files,
  //      since we can use CheckUserProm() to check the program versus
  //      the data file....
  //  WritePromImageIntoXsvfVerify_();
  //
  //
  // Postamble to finish programming the user prom:
  WritePostambleIntoXsvfImage_();
  //
  number_of_write_bytes_ = image_counter_;
  //
  std::vector<char>(xsvf_image_).swap(xsvf_image_);   //reduce the size allocated to xsvf_image to the size that is needed
  //
  return true;                   // xsvf image creation OK
}
//
void EMUjtag::WritePreambleIntoXsvfImage_() {
  //
  // Preamble needed to program the user prom:
  //
  int all_zeros[MAX_NUM_FRAMES] = {}; 
  int tdo_mask[MAX_NUM_FRAMES] = {}; 
  int tdi_in[MAX_NUM_FRAMES] = {}; 
  int tdo_expected[MAX_NUM_FRAMES] = {}; 
  //
  WriteXREPEAT_(0);
  WriteXSTATE_(TLR);
  WriteXSTATE_(RTI);
  //
  WriteXRUNTEST_(0);
  WriteXSIR_(PROMidCode);
  WriteXSDRSIZE_(RegSizeTmbUserProm_PROMidCode);
  int_to_bits(MASK_TO_TREAT_512k_LIKE_256k,
	      RegSizeTmbUserProm_PROMidCode,
	      tdo_mask,
	      LSBfirst);
  WriteXTDOMASK_(RegSizeTmbUserProm_PROMidCode,
		 tdo_mask);
  int_to_bits(PROM_ID_256k,
	      RegSizeTmbUserProm_PROMidCode,
	      tdo_expected,
	      LSBfirst);
  WriteXSDRTDO_(RegSizeTmbUserProm_PROMidCode,
		all_zeros,
		tdo_expected);
  //
  WriteXRUNTEST_(110000);
  WriteXSIR_(PROMconLd);
  //
  WriteXRUNTEST_(0);
  WriteXSIR_(PROMbypass);
  WriteXSTATE_(TLR);
  //
  WriteXSIR_(PROMispen);
  WriteXSDRSIZE_(RegSizeTmbUserProm_PROMispen);
  WriteXTDOMASK_(RegSizeTmbUserProm_PROMispen,
		 all_zeros);
  int_to_bits(0x34,
	      RegSizeTmbUserProm_PROMispen,
	      tdi_in,
	      LSBfirst);
  WriteXSDRTDO_(RegSizeTmbUserProm_PROMispen,
		tdi_in,
		all_zeros);
  //
  WriteXSIR_(PROMaddress);
  WriteXRUNTEST_(2);
  WriteXSDRSIZE_(RegSizeTmbUserProm_PROMaddress);
  WriteXTDOMASK_(RegSizeTmbUserProm_PROMaddress,
		 all_zeros);
  int_to_bits(0x1,
	      RegSizeTmbUserProm_PROMaddress,
	      tdi_in,
	      LSBfirst);
  WriteXSDRTDO_(RegSizeTmbUserProm_PROMaddress,
		tdi_in,
		all_zeros);
  //
  // Reduce the time for clearing the small (256K) prom from 15 seconds (default value from Xilinx to 1...
  //  WriteXRUNTEST_(15000001);
  WriteXRUNTEST_(1000001);
  WriteXSIR_(PROMerase);
  //
  WriteXRUNTEST_(110000);
  WriteXSIR_(PROMconLd);
  //
  WriteXSTATE_(TLR);
  //
  WriteXRUNTEST_(0);
  WriteXSIR_(PROMispen);
  WriteXSDRSIZE_(RegSizeTmbUserProm_PROMispen);
  WriteXTDOMASK_(RegSizeTmbUserProm_PROMispen,
		 all_zeros);
  int_to_bits(0x34,
	      RegSizeTmbUserProm_PROMispen,
	      tdi_in,
	      LSBfirst);
  WriteXSDRTDO_(RegSizeTmbUserProm_PROMispen,
		tdi_in,
		all_zeros);
  //
  return;
}
//
void EMUjtag::WritePromImageIntoXsvfImage_() {
  //
  int all_zeros[MAX_NUM_FRAMES] = {}; 
  int tdi_in[MAX_NUM_FRAMES] = {}; 
  //
  int address_counter = 0;
  for (int block_counter=0; block_counter<TOTAL_NUMBER_OF_BLOCKS; block_counter++) {
    //
    // Break out of the loop if you don't have any more data to write:
    if (address_counter > prom_image_word_count_) break;
    //
    WriteXSIR_(PROMwriteData0);
    WriteXRUNTEST_(2);
    WriteXSDRSIZE_(RegSizeTmbUserProm_PROMwriteData0);    
    WriteXTDOMASK_(RegSizeTmbUserProm_PROMwriteData0,
		   all_zeros);
    //
    // Fill the tdi written to the prom with the prom image data:
    //
    char character_buffer[NUMBER_OF_BITS_PER_BLOCK/NUMBER_OF_BITS_PER_ADDRESS];
    for (int address_within_block=0; address_within_block<NUMBER_OF_ADDRESSES_PER_BLOCK; address_within_block++) {
      character_buffer[address_within_block] = (GetUserPromImage(address_counter++) & 0xff);
      //
      //      (*MyOutput_) << " address within block = " << std::dec << address_within_block 
      //		   << ", address in image = " << (address_counter-1)
      //		   << ", tdi = 0x" << std::hex << tdi_in[address_within_block] 
      //		   << std::endl;
      //      ::usleep(250000);
    }
    // unpack the character buffer we filled with prom data into a bit-vector like all other jtag arrays....
    unpackCharBuffer(character_buffer,
		     NUMBER_OF_BITS_PER_BLOCK,
		     0,
		     tdi_in);
    WriteXSDRTDO_(RegSizeTmbUserProm_PROMwriteData0,
		  tdi_in,
		  all_zeros);
    //
    WriteXRUNTEST_(0);
    WriteXSIR_(PROMaddress);
    WriteXRUNTEST_(2);
    WriteXSDRSIZE_(RegSizeTmbUserProm_PROMaddress);
    WriteXTDOMASK_(RegSizeTmbUserProm_PROMaddress,
		   all_zeros);
    int_to_bits( (block_counter<<5) ,
		 RegSizeTmbUserProm_PROMaddress,
		 tdi_in,
		 LSBfirst);
    WriteXSDRTDO_(RegSizeTmbUserProm_PROMaddress,
		  tdi_in,
		  all_zeros);
    //
    WriteXRUNTEST_(14001);
    WriteXSIR_(PROMprogram);
    //
    WriteXRUNTEST_(0);
  }
  //
  return;
}
//
void EMUjtag::WriteInterimBetweenImageAndVerifyIntoXsvfImage_() {
  //
  int all_zeros[MAX_NUM_FRAMES] = {}; 
  int tdi_in[MAX_NUM_FRAMES] = {}; 
  //
  WriteXSIR_(PROMaddress);
  WriteXRUNTEST_(1);
  int_to_bits(1,
	      RegSizeTmbUserProm_PROMaddress,
	      tdi_in,
	      LSBfirst);
  WriteXSDRTDO_(RegSizeTmbUserProm_PROMaddress,
		tdi_in,
		all_zeros);
  //
  WriteXRUNTEST_(37000);
  WriteXSIR_(PROMsErase);
  //
  WriteXRUNTEST_(110000);
  WriteXSIR_(PROMconLd);
  //
  WriteXRUNTEST_(0);
  WriteXSIR_(PROMispen);
  WriteXSDRSIZE_(RegSizeTmbUserProm_PROMispen);
  WriteXTDOMASK_(RegSizeTmbUserProm_PROMispen,
		 all_zeros);
  int_to_bits(0x34,
	      RegSizeTmbUserProm_PROMispen,
	      tdi_in,
	      LSBfirst);
  WriteXSDRTDO_(RegSizeTmbUserProm_PROMispen,
		tdi_in,
		all_zeros);
  //
  WriteXRUNTEST_(110000);
  WriteXSIR_(PROMconLd);
  //
  WriteXRUNTEST_(0);
  WriteXSIR_(PROMispen);
  int_to_bits(0x34,
	      RegSizeTmbUserProm_PROMispen,
	      tdi_in,
	      LSBfirst);
  WriteXSDRTDO_(RegSizeTmbUserProm_PROMispen,
		tdi_in,
		all_zeros);
  //
  return;
}
//
void EMUjtag::WritePromImageIntoXsvfVerify_() {
  //
  int all_zeros[MAX_NUM_FRAMES] = {}; 
  int all_ones[MAX_NUM_FRAMES];
  for (int i=0; i<MAX_NUM_FRAMES; i++) 
    all_ones[i] = 1;
  int tdi_in[MAX_NUM_FRAMES] = {}; 
  int tdo_expected[MAX_NUM_FRAMES] = {}; 
  //
  int address_counter = 0;
  for (int block_counter=0; block_counter<TOTAL_NUMBER_OF_BLOCKS/2; block_counter++) {
    //
    // Break out of the loop if you don't have any more data to verify:
    if (address_counter > prom_image_word_count_) break;
    //
    WriteXSIR_(PROMaddress);
    WriteXRUNTEST_(2);
    WriteXSDRSIZE_(RegSizeTmbUserProm_PROMaddress);
    WriteXTDOMASK_(RegSizeTmbUserProm_PROMaddress,
		   all_zeros);
    int_to_bits( (block_counter<<6) ,
		 RegSizeTmbUserProm_PROMaddress,
		 tdi_in,
		 LSBfirst);
    WriteXSDRTDO_(RegSizeTmbUserProm_PROMaddress,
		  tdi_in,
		  all_zeros);
    WriteXRUNTEST_(51);
    //
    WriteXSIR_(PROMverifyData0);
    WriteXSDRSIZE_(RegSizeTmbUserProm_PROMverifyData0);    
    WriteXTDOMASK_(RegSizeTmbUserProm_PROMverifyData0,
		   all_ones);
    //
    // Fill the tdo expected with the prom image data in blocks twice as large...
    //
    const int number_of_addresses_per_verify_block=RegSizeTmbUserProm_PROMverifyData0/NUMBER_OF_BITS_PER_ADDRESS;
    char character_buffer[number_of_addresses_per_verify_block];
    //
    for (int address_within_block=0; address_within_block<number_of_addresses_per_verify_block; address_within_block++) {
      character_buffer[address_within_block] = (GetUserPromImage(address_counter++) & 0xff);
      //
      //      (*MyOutput_) << " address within block = " << std::dec << address_within_block 
      //      		   << ", address in image = " << (address_counter-1)
      //      		   << ", tdi = " << std::hex << (int) character_buffer[address_within_block] 
      //      		   << std::endl;
      //      ::usleep(250000);
    }
    // unpack the character buffer we filled with prom data into a bit-vector like all other jtag arrays....
    unpackCharBuffer(character_buffer,
		     RegSizeTmbUserProm_PROMverifyData0,
		     0,
		     tdo_expected);
    WriteXSDRTDO_(RegSizeTmbUserProm_PROMverifyData0,
		  all_zeros,
		  tdo_expected);
    //
    if ( block_counter == (TOTAL_NUMBER_OF_BLOCKS/2)-1 ) {
      // on the last verify, wait for a longer amount of time
      // before writing the post-amble...
      WriteXRUNTEST_(110000);
    } else {
      WriteXRUNTEST_(0);
    }
    //
  }
  //
  return;
}
//
void EMUjtag::WritePostambleIntoXsvfImage_() {
  //
  // Postamble needed to program the user prom:
  //
  int all_zeros[MAX_NUM_FRAMES] = {}; 
  int all_ones[MAX_NUM_FRAMES];
  for (int i=0; i<MAX_NUM_FRAMES; i++) 
    all_ones[i] = 1;
  int tdi_in[MAX_NUM_FRAMES] = {}; 
  int tdo_expected[MAX_NUM_FRAMES] = {}; 
  //
  WriteXSIR_(PROMconLd);
  WriteXRUNTEST_(0);
  //
  WriteXSIR_(PROMispen);
  WriteXSDRSIZE_(RegSizeTmbUserProm_PROMispen);
  WriteXTDOMASK_(RegSizeTmbUserProm_PROMispen,
		 all_zeros);
  int_to_bits(0x34,
	      RegSizeTmbUserProm_PROMispen,
	      tdi_in,
	      LSBfirst);
  WriteXSDRTDO_(RegSizeTmbUserProm_PROMispen,
		tdi_in,
		all_zeros);
  //
  WriteXSIR_(PROMaddress);
  WriteXRUNTEST_(2);
  WriteXSDRSIZE_(RegSizeTmbUserProm_PROMaddress);
  WriteXTDOMASK_(RegSizeTmbUserProm_PROMaddress,
		 all_zeros);
  int_to_bits(0x4000,
	      RegSizeTmbUserProm_PROMaddress,
	      tdi_in,
	      LSBfirst);
  WriteXSDRTDO_(RegSizeTmbUserProm_PROMaddress,
		tdi_in,
		all_zeros);
  WriteXRUNTEST_(0);
  //
  // Put date of file creation as usercode in format 0xMMDDYYYY
  time_t rawtime;
  struct tm * timeinfo;
  //
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  //
  int month_counting_from_one = timeinfo->tm_mon + 1;
  int day = timeinfo->tm_mday;
  int yearAD = timeinfo->tm_year + 1900;
  //
  //  std::cout << "Month = " << month_counting_from_one << std::endl;
  //  std::cout << "Day = " << day << std::endl;
  //  std::cout << "Year = " << yearAD << std::endl;
  //
  int tens_digit_month_counting_from_one = month_counting_from_one / 10;
  int ones_digit_month_counting_from_one = month_counting_from_one % 10;
  int tens_digit_day = day / 10;
  int ones_digit_day = day % 10;
  int thousands_digit_yearAD = yearAD / 1000;
  int hundreds_digit_yearAD = (yearAD / 100) % 10;
  int tens_digit_yearAD = (yearAD / 10) % 10;
  int ones_digit_yearAD = yearAD % 10;
  int date_in_hex = 
    ( (tens_digit_month_counting_from_one & 0xf) << 28 ) |
    ( (ones_digit_month_counting_from_one & 0xf) << 24 ) |
    ( (tens_digit_day & 0xf) << 20 ) |
    ( (ones_digit_day & 0xf) << 16 ) |
    ( (thousands_digit_yearAD & 0xf) << 12 ) |
    ( (hundreds_digit_yearAD & 0xf) << 8 ) |
    ( (tens_digit_yearAD & 0xf) << 4 ) |
    ( (ones_digit_yearAD & 0xf) );
  //
  //  std::cout << "Date = 0x" << std::hex << date << std::endl;
  //
  WriteXSIR_(PROMuserCode);
  WriteXSDRSIZE_(RegSizeTmbUserProm_PROMuserCode);
  WriteXTDOMASK_(RegSizeTmbUserProm_PROMuserCode,
		 all_zeros);
  int_to_bits(date_in_hex,
	      RegSizeTmbUserProm_PROMuserCode,
	      tdi_in,
	      LSBfirst);
  WriteXSDRTDO_(RegSizeTmbUserProm_PROMuserCode,
		tdi_in,
		all_zeros);
  WriteXRUNTEST_(14001);
  //
  WriteXSIR_(PROMprogram);
  WriteXRUNTEST_(50);
  //
  WriteXSIR_(PROMusercodeData);
  WriteXTDOMASK_(RegSizeTmbUserProm_PROMusercodeData,
		 all_ones);
  int_to_bits(date_in_hex,
	      RegSizeTmbUserProm_PROMusercodeData,
	      tdo_expected,
	      LSBfirst);
  WriteXSDRTDO_(RegSizeTmbUserProm_PROMusercodeData,
		all_zeros,
		tdo_expected);
  WriteXRUNTEST_(110000);
  //
  //
  WriteXSIR_(PROMconLd);
  WriteXRUNTEST_(0);
  //
  WriteXSIR_(PROMispen);
  WriteXSDRSIZE_(RegSizeTmbUserProm_PROMispen);
  WriteXTDOMASK_(RegSizeTmbUserProm_PROMispen,
		 all_zeros);
  int_to_bits(0x34,
	      RegSizeTmbUserProm_PROMispen,
	      tdi_in,
	      LSBfirst);
  WriteXSDRTDO_(RegSizeTmbUserProm_PROMispen,
		tdi_in,
		all_zeros);
  //
  WriteXSIR_(PROMispen);
  int_to_bits(0x34,
	      RegSizeTmbUserProm_PROMispen,
	      tdi_in,
	      LSBfirst);
  WriteXSDRTDO_(RegSizeTmbUserProm_PROMispen,
		tdi_in,
		all_zeros);
  //
  WriteXSIR_(PROMaddress);
  WriteXRUNTEST_(2);
  WriteXSDRSIZE_(RegSizeTmbUserProm_PROMaddress);
  WriteXTDOMASK_(RegSizeTmbUserProm_PROMaddress,
		 all_zeros);
  int_to_bits(0x4000,
	      RegSizeTmbUserProm_PROMaddress,
	      tdi_in,
	      LSBfirst);
  WriteXSDRTDO_(RegSizeTmbUserProm_PROMaddress,
		tdi_in,
		all_zeros);
  WriteXRUNTEST_(0);
  //
  WriteXSIR_(PROMwriteData3);
  WriteXSDRSIZE_(RegSizeTmbUserProm_PROMwriteData3);
  WriteXTDOMASK_(RegSizeTmbUserProm_PROMwriteData3,
		 all_zeros);
  int_to_bits(0x3d,
	      RegSizeTmbUserProm_PROMwriteData3,
	      tdi_in,
	      LSBfirst);
  WriteXSDRTDO_(RegSizeTmbUserProm_PROMwriteData3,
		tdi_in,
		all_zeros);
  WriteXRUNTEST_(14001);
  //
  WriteXSIR_(PROMprogram);
  WriteXRUNTEST_(51);
  //
  WriteXSIR_(PROMverifyData3);
  WriteXTDOMASK_(RegSizeTmbUserProm_PROMverifyData3,
		 all_zeros);
  int_to_bits(0x3d,
	      RegSizeTmbUserProm_PROMverifyData3,
	      tdi_in,
	      LSBfirst);
  int_to_bits(0x3d,
	      RegSizeTmbUserProm_PROMverifyData3,
	      tdo_expected,
	      LSBfirst);
  WriteXSDRTDO_(RegSizeTmbUserProm_PROMverifyData3,
		tdi_in,
		tdo_expected);
  WriteXRUNTEST_(110000);
  //
  WriteXSIR_(PROMconLd);
  WriteXRUNTEST_(0);
  //
  WriteXSIR_(PROMispen);
  WriteXTDOMASK_(RegSizeTmbUserProm_PROMispen,
		 all_zeros);
  int_to_bits(0x34,
	      RegSizeTmbUserProm_PROMispen,
	      tdi_in,
	      LSBfirst);
  WriteXSDRTDO_(RegSizeTmbUserProm_PROMispen,
		tdi_in,
		all_zeros);
  WriteXRUNTEST_(110000);
  //
  WriteXSIR_(PROMconLd);
  //
  WriteXREPEAT_(0);
  WriteXREPEAT_(32);
  WriteXSTATE_(TLR);
  WriteXSTATE_(RTI);
  WriteXRUNTEST_(0);
  //
  WriteXSIR_(PROMbypass);
  WriteXSDRSIZE_(RegSizeTmbUserProm_PROMbypass);
  WriteXTDOMASK_(RegSizeTmbUserProm_PROMbypass,
		 all_zeros);
  WriteXSDRTDO_(RegSizeTmbUserProm_PROMbypass,
		all_zeros,
		all_zeros);
  //
  WriteXCOMPLETE_();
  //
  return;
}
//
void EMUjtag::WriteXTDOMASK_(int number_of_bits_in_mask,
			     int * vector_of_bits_for_mask) {
  // First character = command:
  SetWriteXsvfImage_(image_counter_++,XTDOMASK);
  //
  // Setup vector of tdi with bypass bits appropriately set...
  // The length to pass to ShfDR_ is the length of the vector
  // of bits for this chip (with no bypass bits)...
  ShfDR_(GetWhichUserProm(),
	 number_of_bits_in_mask,
	 vector_of_bits_for_mask);
  //
  int number_of_bytes = (xdr_length_-1)/8+1;
  //
  // The jtag byte arrays are arranged such that 
  // LSB of index = 0 = first bit that is shifted in.
  // However, the Xilinx xsvf file requires that the
  // instruction register is read in (written out) 
  // left-to-right, i.e., most-significant byte is
  // first.  Therefore we need to FLIP the order of the
  // bytes as they go into the XSVF file....
  //
  for (int byte=number_of_bytes-1; byte>=0; byte--)
    SetWriteXsvfImage_(image_counter_++,tdi_in_bytes_[byte]);  
  //
  return;
}
//
//
void EMUjtag::WriteXSDRTDO_(int number_of_data_register_bits_for_this_chip,
			    int * tdi_bits,
			    int * tdo_bits) {
  // First character = command:
  SetWriteXsvfImage_(image_counter_++,XSDRTDO);
  //
  // Here is the number of bytes which we are going to write
  int number_of_bytes = (xdr_length_-1)/8+1;
  //
  // Setup vector of tdi with bypass bits appropriately set...
  // The length to pass to ShfDR_ is the length of the vector
  // of bits for this chip (with no bypass bits)...
  ShfDR_(GetWhichUserProm(),
	 number_of_data_register_bits_for_this_chip,
	 tdi_bits);
  //
  //
  // The jtag byte arrays are arranged such that 
  // LSB of index = 0 = first bit that is shifted in.
  // However, the Xilinx xsvf file requires that the
  // instruction register is read in (written out) 
  // left-to-right, i.e., most-significant byte is
  // first.  Therefore we need to FLIP the order of the
  // bytes as they go into the XSVF file....
  //
  for (int byte=number_of_bytes-1; byte>=0; byte--)
    SetWriteXsvfImage_(image_counter_++,tdi_in_bytes_[byte]);  
  //
  // Now repeat the process for the tdo expected
  ShfDR_(GetWhichUserProm(),
	 number_of_data_register_bits_for_this_chip,
	 tdo_bits);
  //
  for (int byte=number_of_bytes-1; byte>=0; byte--)
    SetWriteXsvfImage_(image_counter_++,tdi_in_bytes_[byte]);  
  //
  return;
}
//
void EMUjtag::WriteXSIR_(int opcode) {
  //
  // First character = command:
  SetWriteXsvfImage_(image_counter_++,XSIR);
  //  
  // Second character = total number of bits:
  int number_of_bits = SumOpcodeBits_();
  SetWriteXsvfImage_(image_counter_++,number_of_bits);
  //
  // Setup opcodes with bypass bits appropriately set:
  ShfIR_(GetWhichUserProm(),opcode);
  //
  int number_of_bytes = (number_of_bits-1)/8+1;
  //
  // The jtag byte arrays are arranged such that 
  // LSB of index = 0 = first bit that is shifted in.
  // However, the Xilinx xsvf file requires that the
  // instruction register is read in (written out) 
  // left-to-right, i.e., most-significant byte is
  // first.  Therefore we need to FLIP the order of the
  // bytes as they go into the XSVF file....
  //
  for (int byte=number_of_bytes-1; byte>=0; byte--)
    SetWriteXsvfImage_(image_counter_++,tdi_in_bytes_[byte]);  
  //
  return;
}
//
int EMUjtag::SumOpcodeBits_() {
  //
  int total_bits = 0;
  //
  for (int idevice=0; idevice<devices_in_chain_; idevice++) 
    total_bits += bits_in_opcode_[idevice];
  //
  return total_bits;
}
//
void EMUjtag::WriteXRUNTEST_(int length_of_time) {
  //
  SetWriteXsvfImage_(image_counter_++,XRUNTEST);  
  //
  // The jtag byte arrays are arranged such that 
  // LSB of index = 0 = first bit that is shifted in.
  // However, the Xilinx xsvf file requires that the
  // instruction register is read in (written out) 
  // left-to-right, i.e., most-significant byte is
  // first.  Therefore we need to FLIP the order of the
  // bytes as they go into the XSVF file....
  //
  for (int byte=3; byte>=0; byte--) {
    int value = (length_of_time >> byte*8) & 0xff;
    SetWriteXsvfImage_(image_counter_++,value);  
  }
  //
  return;
}
//
void EMUjtag::WriteXSDRSIZE_(int number_of_bits_in_data_register) {
  //
  SetWriteXsvfImage_(image_counter_++,XSDRSIZE);  
  //
  int bits_in_excess_of_dr = devices_in_chain_ - 1;
  //
  xdr_length_ = number_of_bits_in_data_register + bits_in_excess_of_dr;
  //
  // The jtag byte arrays are arranged such that 
  // LSB of index = 0 = first bit that is shifted in.
  // However, the Xilinx xsvf file requires that the
  // instruction register is read in (written out) 
  // left-to-right, i.e., most-significant byte is
  // first.  Therefore we need to FLIP the order of the
  // bytes as they go into the XSVF file....
  //
  for (int byte=3; byte>=0; byte--) {
    int value = (xdr_length_ >> byte*8) & 0xff;
    SetWriteXsvfImage_(image_counter_++,value);
  }  
  //
  return;
}
//
void EMUjtag::WriteXREPEAT_(int number_of_times) {
  //
  SetWriteXsvfImage_(image_counter_++,XREPEAT);
  SetWriteXsvfImage_(image_counter_++,number_of_times);
  //
  return;
}
//
void EMUjtag::WriteXCOMPLETE_() {
  //
  SetWriteXsvfImage_(image_counter_++,XCOMPLETE);
  //
  return;
}
//
void EMUjtag::WriteXSTATE_(int state) {
  //
  SetWriteXsvfImage_(image_counter_++,XSTATE);
  SetWriteXsvfImage_(image_counter_++,state);
  //
  return;
}
//
void EMUjtag::WriteXsvfImageToDisk_() {
  //
  (*MyOutput_) << "EMUjtag:  Write XSVF file " << filename_xsvf_ << " to disk" << std::endl;
  //
  std::ofstream file_to_write;
  file_to_write.open(filename_xsvf_.c_str(),
		     std::ios::binary);        
  //
  for (int index=0; index<number_of_write_bytes_; index++) 
    file_to_write << GetReadXsvfImage_(index);
  //
  file_to_write.close();
  //
  return;
}
//
//
//----------------------------------//
// Read XSVF file
//----------------------------------//
void EMUjtag::ReadXsvfFile() {
  //
  // No argument = no logfile written:
  ReadXsvfFile(false);
  //
  return;
}
//
void EMUjtag::ReadXsvfFile(bool create_logfile) {
  //
  // With this method, just read the xsvf file, do not program prom:
  SetWriteToDevice_(false);
  //
  ReadXsvfFile_(create_logfile);
  //
  // Set default back to standard JTAG operation:
  SetWriteToDevice_(true);
  //
  return;
}
//
void EMUjtag::ReadXsvfFile_(bool create_logfile) {
  //
  if (file_io_) {
    (*MyOutput_) << "EMUjtag:  Read XSVF file " << filename_xsvf_ << " from disk" << std::endl;
    //
    //    for (int i=0; i<MAX_XSVF_IMAGE_NUMBER; i++) 
    //      xsvf_image_[i]=0; 
    xsvf_image_.clear(); 
    //
    std::ifstream Readfile;
    Readfile.open(filename_xsvf_.c_str(),
		  std::ifstream::binary);     //xsvf file is binary
    //
    int byte_counter=0;
    while ( Readfile.good() ) {
      int temp = Readfile.get();
      SetWriteXsvfImage_(byte_counter++,temp);
    }
    //    std::cout << "size before swap = " << xsvf_image_.capacity() << std::endl;
    //
    std::vector<char>(xsvf_image_).swap(xsvf_image_);   //reduce the size allocated to xsvf_image to the size that is needed
    //
    //    std::cout << "size after swap = " << xsvf_image_.capacity() << std::endl;
    //
    number_of_read_bytes_ = --byte_counter;
    //
    //  for (int i=200; i<300; i++) 
    //    std::cout << "xsvf_image_[" << std::dec << i 
    //	      << "] = " << GetReadXsvfImage_(i) << std::endl;
    //
    Readfile.close();
    //
  } else {
    //
    (*MyOutput_) << "EMUjtag:  Read internal XSVF image" << std::endl;
    //
    number_of_read_bytes_ = number_of_write_bytes_ ;
    //
  }
  //
  if (create_logfile) 
    Logfile_.open(filename_log_.c_str());        
  //
  Logfile_ << "Logfile for file " << filename_xsvf_ << std::endl;
  Logfile_ << "Number of bytes = " << number_of_read_bytes_ << std::endl;
  //
  //  for (int byte=0; byte<number_of_read_bytes_; byte++) {
  //    int value = GetReadXsvfImage_(byte) & 0xff;
  //    Logfile_ << std::hex << ( (value>>4)&0xf ) << (value&0xf) << " ";
  //    if ( (byte+1)%20 == 0) 
  //      Logfile_ << std::endl;
  //  }
  //
  Logfile_ << std::endl;
  //
  if (number_of_read_bytes_ > 0) {
    //
    verify_error_ = 0;     //start fresh with no verify errors
    //
    DecodeXsvfImage_();
    //
  } else {
    //
    verify_error_ = -1;    //tag that file does not exist
    //
    (*MyOutput_) << "EMUjtag:  ERROR XSVF file " << filename_xsvf_ << " does not exist" << std::endl;
    //
  }
  //
  (*MyOutput_) << "Number of verify errors = " << std::dec << verify_error_ << std::endl;
  //
  Logfile_ << std::endl;
  Logfile_ << "Number of verify errors = " << std::dec << verify_error_ << std::endl;
  //
  Logfile_ << std::endl;
  Logfile_ << "Summary statistics: " << std::endl;
  Logfile_ << "--------------------------" << std::endl;
  Logfile_ << " XCOMPLETE   -> " << std::setw(4) << std::dec << NumberOfCommands_[XCOMPLETE] << " times" << std::endl;
  Logfile_ << " XTDOMASK    -> " << std::setw(4) << std::dec << NumberOfCommands_[XTDOMASK] << " times" << std::endl;
  Logfile_ << " XSIR        -> " << std::setw(4) << std::dec << NumberOfCommands_[XSIR] << " times" << std::endl;
  Logfile_ << " XSDR        -> " << std::setw(4) << std::dec << NumberOfCommands_[XSDR] << " times" << std::endl;
  Logfile_ << " XRUNTEST    -> " << std::setw(4) << std::dec << NumberOfCommands_[XRUNTEST] << " times" << std::endl;
  Logfile_ << " XUNDEFINED5 -> " << std::setw(4) << std::dec << NumberOfCommands_[XUNDEFINED5] << " times" << std::endl;
  Logfile_ << " XUNDEFINED6 -> " << std::setw(4) << std::dec << NumberOfCommands_[XUNDEFINED6] << " times" << std::endl;
  Logfile_ << " XREPEAT     -> " << std::setw(4) << std::dec << NumberOfCommands_[XREPEAT] << " times" << std::endl;
  Logfile_ << " XSDRSIZE    -> " << std::setw(4) << std::dec << NumberOfCommands_[XSDRSIZE] << " times" << std::endl;
  Logfile_ << " XSDRTDO     -> " << std::setw(4) << std::dec << NumberOfCommands_[XSDRTDO] << " times" << std::endl;
  Logfile_ << " XSETSDRMASK -> " << std::setw(4) << std::dec << NumberOfCommands_[XSETSDRMASK] << " times" << std::endl;
  Logfile_ << " XSDRINC     -> " << std::setw(4) << std::dec << NumberOfCommands_[XSDRINC] << " times" << std::endl;
  Logfile_ << " XSDRB       -> " << std::setw(4) << std::dec << NumberOfCommands_[XSDRB] << " times" << std::endl;
  Logfile_ << " XSDRC       -> " << std::setw(4) << std::dec << NumberOfCommands_[XSDRC] << " times" << std::endl;
  Logfile_ << " XSDRE       -> " << std::setw(4) << std::dec << NumberOfCommands_[XSDRE] << " times" << std::endl;
  Logfile_ << " XSDRTDOB    -> " << std::setw(4) << std::dec << NumberOfCommands_[XSDRTDOB] << " times" << std::endl;
  Logfile_ << " XSDRTDOC    -> " << std::setw(4) << std::dec << NumberOfCommands_[XSDRTDOC] << " times" << std::endl;
  Logfile_ << " XSDRTDOE    -> " << std::setw(4) << std::dec << NumberOfCommands_[XSDRTDOE] << " times" << std::endl;
  Logfile_ << " XSTATE      -> " << std::setw(4) << std::dec << NumberOfCommands_[XSTATE] << " times" << std::endl;
  Logfile_ << " XENDIR      -> " << std::setw(4) << std::dec << NumberOfCommands_[XENDIR] << " times" << std::endl;
  Logfile_ << " XENDDR      -> " << std::setw(4) << std::dec << NumberOfCommands_[XENDDR] << " times" << std::endl;
  Logfile_ << " XSIR2       -> " << std::setw(4) << std::dec << NumberOfCommands_[XSIR2] << " times" << std::endl;
  Logfile_ << " XCOMMENT    -> " << std::setw(4) << std::dec << NumberOfCommands_[XCOMMENT] << " times" << std::endl;
  Logfile_ << " --------------------- " << std::endl;
  int total_commands = 0;
  for (int i=0; i<NUMBER_OF_DIFFERENT_XSVF_COMMANDS; i++)
    total_commands += NumberOfCommands_[i];
  Logfile_ << " total number-> " << std::setw(4) << std::dec << total_commands << std::endl;

  //
  if (create_logfile)
    Logfile_.close();
  //
  //  std::cout << "size before zeroing = " << xsvf_image_.capacity() << std::endl;
  //
  std::vector<char>().swap(xsvf_image_);   //reduce the size allocated to xsvf_image to 0
  //
  //  std::cout << "size after zeroing = " << xsvf_image_.capacity() << std::endl;
  //
  return;
}
//
void EMUjtag::DecodeXsvfImage_() {
  //
  for (int i=0; i<NUMBER_OF_DIFFERENT_XSVF_COMMANDS; i++)
    NumberOfCommands_[i] = 0;
  //
  image_counter_ = 0;
  //
  int command_counter = 0;
  //
  while (image_counter_ < number_of_read_bytes_) {
    //
    int command = GetReadXsvfImage_(image_counter_++);
    //
    command_counter++;
    //
    if ( GetWriteToDevice_() ) 
      if (command_counter % 100 == 0) {
	float percent_programmed = 100.* (float)image_counter_ / (float)number_of_read_bytes_;
	//
	(*MyOutput_) << "Slot " << (int) tmb_->slot() 
		     << ":  Programmed " << std::setprecision(1) << std::fixed << percent_programmed 
		     << "% of " << std::dec << number_of_read_bytes_ 
		     << " bytes with " << verify_error_ <<" verify errors" << std::endl;
      }
    //
    if (command > NUMBER_OF_DIFFERENT_XSVF_COMMANDS) {
      (*MyOutput_) << "EMUjtag: ERROR DecodeXsvfImage_ command = " 
		   << std::hex << command << " out of range" << std::endl;
      return;
    }
    NumberOfCommands_[command]++;
    //
    switch (command) {
      //
    case XCOMPLETE:
      ParseXCOMPLETE_();
      break;
      //
    case XTDOMASK:
      ParseXTDOMASK_();
      break;
      //
    case XSIR:
      ParseXSIR_();
      break;
      //
    case XRUNTEST:
      ParseXRUNTEST_();
      break;
      //
    case XREPEAT:
      ParseXREPEAT_();
      break;
      //
    case XSDRSIZE:
      ParseXSDRSIZE_();
      break;
      //
    case XSDRTDO:
      ParseXSDRTDO_();
      break;
      //
    case XSTATE:
      ParseXSTATE_();
      break;
      //
    default:
      (*MyOutput_) << "EMUjtag: ERROR DecodeXsvfImage_ unknown command = " 
		   << std::hex << command << std::endl;
      break;
    }
  }
  //
  return;
}
//
void EMUjtag::ParseXCOMPLETE_() {
  //
  if (NumberOfCommands_[XCOMPLETE] > 1) {
    //
    (*MyOutput_) << "EMUjtag: XCOMPLETE ERROR multiple eof's reached..." << std::endl;
    //
    // set the image counter high to prevent causing more damage:
    image_counter_ = MAX_XSVF_IMAGE_NUMBER;
    //
    return;
  }
  //
  Logfile_ << "XCOMPLETE -> End of XSVF file reached" << std::endl;
  //
  return;
}
//
void EMUjtag::ParseXTDOMASK_() {
  //
  for (int byte=0; byte<MAX_BUFFER_SIZE; byte++) 
    tdo_mask_in_bytes_[byte] = 0;
  //
  int number_of_bytes = (xdr_length_-1)/8 + 1;
  //
  if (number_of_bytes > MAX_BUFFER_SIZE) {
    (*MyOutput_) << "EMUjtag: XTDOMASK ERROR number of tdo bytes " << number_of_bytes 
		 << " greater than " << MAX_BUFFER_SIZE << std::endl;
    image_counter_=MAX_XSVF_IMAGE_NUMBER;
    return;
  }
  //
  // TDO mask is in the xsvf file beginning first with the most significant byte...
  // => read data into tdo_mask_in_bytes_ buffer backwards in order that 
  //    tdo_mask_in_bytes_ correctly masks the tdo we shift out to compare with
  //    the tdi we have shifted in....
  //
  Logfile_ << "XTDOMASK -> " << std::dec << std::setw(6) << xdr_length_ << " bits, mask =         0x";
  //
  for (int byte=number_of_bytes-1; byte>=0; byte--) {
    //
    tdo_mask_in_bytes_[byte] = GetReadXsvfImage_(image_counter_++) & 0xff;
    Logfile_ << std::hex << ( (tdo_mask_in_bytes_[byte]>>4) & 0xf) << (tdo_mask_in_bytes_[byte] & 0xf);
    //
  }
  Logfile_ << std::endl;
  //
  return;
}
//
void EMUjtag::ParseXSIR_() {
  //
  for (int byte=0; byte<MAX_BUFFER_SIZE; byte++) {
    tdi_in_bytes_[byte] = 0;
    tdo_in_bytes_[byte] = 0;
  }
  //
  int number_of_bits = GetReadXsvfImage_(image_counter_++) & 0xff;
  int number_of_bytes = (number_of_bits-1)/8+1;
  //
  // Opcode is in the xsvf file beginning first with the most significant byte...
  // => read data into tdi_in_bytes_ buffer backwards in order that least significant
  //    bit of tdi_in_bytes_[0] is the first bit we shift in:
  //
  Logfile_ << "XSIR ->     " << std::setw(6) << std::dec << number_of_bits << " bits, TDI value =    0x";
  //
  for (int byte=number_of_bytes-1; byte>=0; byte--) {
    //
    tdi_in_bytes_[byte] = GetReadXsvfImage_(image_counter_++) & 0xff;
    Logfile_ << std::hex << ( (tdi_in_bytes_[byte]>>4) & 0xf) << (tdi_in_bytes_[byte] & 0xf);
    //
  }
  Logfile_ << std::endl;
  //
  if ( GetWriteToDevice_() ) {
    //
    if(debug_){
      //
      (*MyOutput_) << "XSIR     -> Opcode ";
      for (int byte=number_of_bytes-1; byte>=0; byte--) {
	(*MyOutput_) << std::hex << ( (tdi_in_bytes_[byte]>>4) & 0xf) << (tdi_in_bytes_[byte] & 0xf);
      }
      (*MyOutput_) << std::endl;
      //
    }
    //
    // N.B. scan_alct assumes we start in RTI and returns us to RTI...
    //
    tmb_->scan(INSTR_REGISTER, tdi_in_bytes_, number_of_bits, tdo_in_bytes_, NO_READ_BACK);
    //
    // Wait the number of microseconds specified:
    //
    if(debug_){
      //
      (*MyOutput_) << "XRUNTEST -> Pause " << std::dec << xruntest_time_ << " uSec" << std::endl;
      //
    }
    ::usleep(xruntest_time_);
    //
  }
  //
  return;
}
//
void EMUjtag::ParseXRUNTEST_() {
  //
  // pack 4 bytes to give the length of the pause 
  // applicable to subsequent shfIR and shfDR commands
  //
  xruntest_time_ = 0;
  for (int i=3; i>=0; i--) 
    xruntest_time_ |= ( (GetReadXsvfImage_(image_counter_++)&0xff) << 8*i);
  //
  Logfile_ << "XRUNTEST -> " << std::setw(8) << std::dec << xruntest_time_ << " uSec " <<  std::endl;      
  //
  return;
}
//
void EMUjtag::ParseXREPEAT_() {
  //
  xrepeat_times_ = GetReadXsvfImage_(image_counter_++) & 0xff;
  //
  Logfile_ << "XREPEAT ->  " << std::setw(6) << std::dec << xrepeat_times_ << " times" << std::endl;
  //
  return;
}
//
void EMUjtag::ParseXSDRSIZE_() {
  //
  // pack 4 bytes to give the length of the data register 
  // applicable to subsequent shfIR and shfDR commands
  //
  xdr_length_ = 0;
  for (int i=3; i>=0; i--) 
    xdr_length_ |= ( (GetReadXsvfImage_(image_counter_++)&0xff) << 8*i);
  //
  Logfile_ << "XSDRSIZE -> " << std::setw(6) << std::dec << xdr_length_ << " bits" << std::endl;      
  return;
}
//
void EMUjtag::ParseXSDRTDO_() {
  //
  char tdo_expected_in_bytes[MAX_BUFFER_SIZE] = {};
  //
  // Clear tdi and tdo:
  //
  for (int byte=0; byte<MAX_BUFFER_SIZE; byte++) {
    tdi_in_bytes_[byte] = 0;
    tdo_in_bytes_[byte] = 0;
  }
  //
  int number_of_bytes = (xdr_length_-1)/8 + 1;
  //
  if (number_of_bytes > MAX_BUFFER_SIZE) {
    (*MyOutput_) << "EMUjtag: XSDRTDO ERROR number of tdi bytes " << number_of_bytes 
		 << " greater than " << MAX_BUFFER_SIZE << std::endl;
    image_counter_=MAX_XSVF_IMAGE_NUMBER;
    return;
  }
  //
  // TDI is in the xsvf file beginning first with the most significant byte...
  // => read data into tdi_in_bytes_ buffer backwards in order that least significant
  //    bit of tdi_in_bytes_[0] is the first bit we shift in:
  //
  Logfile_ << "XSDRTDO ->  " << std::setw(6) << std::dec << xdr_length_ << " bits, TDI value =    0x";
  //
  for (int byte=number_of_bytes-1; byte>=0; byte--) {
    //
    tdi_in_bytes_[byte] = GetReadXsvfImage_(image_counter_++) & 0xff;
    Logfile_ << std::hex << ( (tdi_in_bytes_[byte]>>4) & 0xf ) << (tdi_in_bytes_[byte] & 0xf);
    //
  }
  Logfile_ << std::endl;
  //
  // TDO is in the xsvf file beginning first with the most significant byte...
  // => read data into tdo_in_bytes_ buffer backwards in order that least significant
  //    bit of tdo_expected_in_bytes[0] is the first bit we shift out:
  //
  Logfile_ << "XSDRTDO ->  " << std::setw(6) << std::dec << xdr_length_ << " bits, TDO expected = 0x";
  //
  for (int byte=number_of_bytes-1; byte>=0; byte--) {
    //
    tdo_expected_in_bytes[byte] = GetReadXsvfImage_(image_counter_++) & 0xff;
    Logfile_ << std::hex << ( (tdo_expected_in_bytes[byte]>>4) & 0xf ) << (tdo_expected_in_bytes[byte] & 0xf);
    //
  }
  Logfile_ << std::endl;
  //
  if ( GetWriteToDevice_() ) {
    //
    if(debug_)
      (*MyOutput_) << "XSDRTDO  ->  " << std::setw(6) << std::dec << xdr_length_ << " bits: " << std::endl;
    //
    // On the data register:  shift in the TDI and shift out the TDO 
    //
    // if the tdo mask == 0, we don't care what is read back out on tdo, so don't read it back...
    bool read_back_tdo = false;
    for (int byte=number_of_bytes-1; byte>=0; byte--) 
      if (tdo_mask_in_bytes_[byte] != 0) 
	read_back_tdo = true;
    //
    if (read_back_tdo) {
      tmb_->scan(DATA_REGISTER, tdi_in_bytes_, xdr_length_, tdo_in_bytes_, READ_BACK);
    } else {
      tmb_->scan(DATA_REGISTER, tdi_in_bytes_, xdr_length_, tdo_in_bytes_, NO_READ_BACK);
    }
    //
    if(debug_){
      (*MyOutput_) << " expect    = ";
      for (int byte=number_of_bytes-1; byte>=0; byte--) 
	(*MyOutput_) << std::hex 
		     << (int) ( (tdo_expected_in_bytes[byte]>>4) &0xf )
		     << (int) ( tdo_expected_in_bytes[byte]&0xf );
      (*MyOutput_) << std::endl;
      //
      (*MyOutput_) << " read back = ";
      for (int byte=number_of_bytes-1; byte>=0; byte--) 
	(*MyOutput_) << std::hex 
		     << (int) ( (tdo_in_bytes_[byte]>>4) &0xf )
		     << (int) ( tdo_in_bytes_[byte]&0xf );
      (*MyOutput_) << std::endl;
      //
      (*MyOutput_) << " mask      = ";
      for (int byte=number_of_bytes-1; byte>=0; byte--) 
	(*MyOutput_) << std::hex 
		     << (int) ( (tdo_mask_in_bytes_[byte]>>4) &0xf )
		     << (int) ( tdo_mask_in_bytes_[byte]&0xf );
      (*MyOutput_) << std::endl;
    }
    //
    // compare the tdo shifted out with the expected tdo, utilizing the tdo mask:
    for (int byte=0; byte<number_of_bytes; byte++) {
      if ( (tdo_in_bytes_[byte] & tdo_mask_in_bytes_[byte] )  != 
	   (tdo_expected_in_bytes[byte] & tdo_mask_in_bytes_[byte]) ) {
	if (debug_) {
	  (*MyOutput_) << "----------> Error in byte " << std::dec << byte << std::endl;
	}
	verify_error_++;
      }
    }
    //
    // Wait the number of microseconds specified:
    if(debug_){
      //
      (*MyOutput_) << "XRUNTEST -> Pause " << std::dec << xruntest_time_ << " uSec" << std::endl;
      //
    }
    ::usleep(xruntest_time_);
    //
  }
  //
  return;
}
//
void EMUjtag::ParseXSTATE_() {
  int state = GetReadXsvfImage_(image_counter_++) & 0xff;
  //
  if (state == 0) {
    Logfile_ << "XSTATE ->      TLR" << std::endl;
  } else if (state == 1) {
    Logfile_ << "XSTATE ->      RTI" << std::endl;
  } else {
    Logfile_ << "XSTATE ->      ???" << std::endl;
  }
  //
  if ( GetWriteToDevice_() ) {
    for (int repeat=0; repeat<=xrepeat_times_; repeat++) {
      //
      if(debug_){
	//
	(*MyOutput_) << "XSTATE   -> RTI " << std::endl;
      }
      // Assumption is that when the STATE command comes, xsvf is going to want to 
      // set up to shift an opcode into the instruction register.//
      // Hence, we always push the state machine to RTI, even if it says TLR:
      //
      tmb_->RestoreIdle();
    }
  }
  //
  return;
}
//
int EMUjtag::GetReadXsvfImage_(unsigned int address) {
  //
  if (address >= xsvf_image_.size()) 
    (*MyOutput_) << "GetReadXsvfImage ERROR: address " << address 
		 << " out of range...  should be between 0 and " << xsvf_image_.size()-1 
		 << std::endl;
  //
  return ( (int) xsvf_image_.at(address) ); 
}
//
void EMUjtag::SetWriteXsvfImage_(unsigned int address, int value) {
  //
  //  if (address >= MAX_XSVF_IMAGE_NUMBER) {
  //    (*MyOutput_) << "SetWriteXsvfImage ERROR: address " << address 
  //		 << " out of range...  should be between 0 and " << MAX_XSVF_IMAGE_NUMBER-1 
  //		 << std::endl;
  //    return;
  //  }
  //
  //  xsvf_image_[address] = (char) (value & 0xff);
  xsvf_image_.push_back( (char) (value & 0xff) );
  return; 
}
//
//
//----------------------------------//
// Program PROM
//----------------------------------//
void EMUjtag::ProgramUserProm() {
  //
  (*MyOutput_) << "EMUjtag:  Programming User Prom..." << std::endl;
  //
  verify_error_ = 0;
  //
  time_t starttime = time (NULL);
  //
  SetWriteToDevice_(true);
  //
  setup_jtag(ChainTmbUser);
  //
  // default is not to write out a logfile (assume no access to disk....
  ReadXsvfFile_(false);
  //
  time_t endtime = time (NULL);
  //
  int time_elapsed = endtime - starttime;
  //
  (*MyOutput_) << "EMUjtag:  Programmed " << std::dec <<   number_of_read_bytes_ 
	       << " bytes in " << std::dec << time_elapsed << " seconds " << std::endl;
  //
  return;
}
//
void EMUjtag::CheckUserProm() {
  //
  (*MyOutput_) << "EMUjtag:  Compare user prom image with program in prom " 
	       << GetWhichUserProm() << "... " << std::endl;
  //
  verify_error_ = 0;
  //
  if ( !ReadUserPromFile() ) {
    (*MyOutput_) << "EMUjtag: ERROR VerifyUserProm prom file " << filename_dat_ << " does not exist..." << std::endl;
    return;
  }
  //
  tmb_->ClockOutPromProgram(GetWhichUserProm(),prom_image_word_count_);
  //
  bool date_changed = false;
  //
  for (int address=0; address<prom_image_word_count_; address++) {
    //    (*MyOutput_) << "EMUjtag: For address " << std::hex << address << std::endl;
    //    (*MyOutput_) << " -> prom image in file = " << std::hex << GetUserPromImage(address) << std::endl;
    //    (*MyOutput_) << " -> prom image in prom = " << std::hex << tmb_->GetClockedOutPromImage(address) << std::endl;
    //
    if ( tmb_->GetClockedOutPromImage(address) != GetUserPromImage(address) ) {
      //
      //stuff which is irrelevant if it is all that changes for the user proms...
      //
      if ( ( (GetWhichUserProm() == ChipLocationTmbUserPromTMB) &&  
	     (address == 0x03 ||    //month in "hex-ascii"
	      address == 0x04 ||    //day in "hex-ascii"
	      address == 0x05 ||    //tens-ones digits of year in hex-ascii
	      address == 0x06 ||    //thousands-hundreds digits of year in hex-ascii
	      address == prom_image_word_count_ - 2) ) //checksum (going to change if date changes...)
	   ||  
	   ( (GetWhichUserProm() == ChipLocationTmbUserPromALCT) &&
	     (address == 0x04 ||    //tens digit month hex-ascii
	      address == 0x05 ||    //ones digit month hex-ascii
	      address == 0x06 ||    //tens digit day hex-ascii
	      address == 0x07 ||    //ones digit day hex-ascii
	      address == 0x08 ||    //thousands digit year hex-ascii
	      address == 0x09 ||    //hundreds digit year hex-ascii
	      address == 0x0a ||    //tens digit year hex-ascii
	      address == 0x0b ||    //ones digit year hex-ascii
	      address == prom_image_word_count_ - 3 || //checksum (going to change if date changes...)
	      address == prom_image_word_count_ - 2) ) //checksum (going to change if date changes...)
	   ) {
	//
	date_changed = true;
	//
      } else {
	if (debug_) {
	  (*MyOutput_) << "EMUjtag: WARNING address 0x" << std::hex << address;
	  (*MyOutput_) << " to change from 0x" << std::hex << tmb_->GetClockedOutPromImage(address) 
		       << " -> 0x" << std::hex << GetUserPromImage(address) << std::endl;
	}
	verify_error_++;
      }
    }
  }
  //
  if (GetNumberOfVerifyErrors() == 0 && date_changed) {
    if (GetWhichUserProm() == ChipLocationTmbUserPromTMB) {
      (*MyOutput_) << "TMB configuration same as that loaded on " 
		   << std::hex << (( tmb_->GetClockedOutPromImage(0x04) >> 4) & 0xf)
		   << std::hex << (( tmb_->GetClockedOutPromImage(0x04) >> 0) & 0xf)
		   << "."
		   << std::hex << (( tmb_->GetClockedOutPromImage(0x03) >> 4) & 0xf)
		   << std::hex << (( tmb_->GetClockedOutPromImage(0x03) >> 0) & 0xf)
		   << "."
		   << std::hex << (( tmb_->GetClockedOutPromImage(0x06) >> 4) & 0xf)
		   << std::hex << (( tmb_->GetClockedOutPromImage(0x06) >> 0) & 0xf)
		   << std::hex << (( tmb_->GetClockedOutPromImage(0x05) >> 4) & 0xf)
		   << std::hex << (( tmb_->GetClockedOutPromImage(0x05) >> 0) & 0xf) 
		   << std::endl; 
    } else if (GetWhichUserProm() == ChipLocationTmbUserPromALCT) {
      (*MyOutput_) << "ALCT configuration same as that loaded on " 
		   << std::hex << (( tmb_->GetClockedOutPromImage(0x06) >> 0) & 0xf)
		   << std::hex << (( tmb_->GetClockedOutPromImage(0x07) >> 0) & 0xf)
		   << "."
		   << std::hex << (( tmb_->GetClockedOutPromImage(0x04) >> 0) & 0xf)
		   << std::hex << (( tmb_->GetClockedOutPromImage(0x05) >> 0) & 0xf)
		   << "."
		   << std::hex << (( tmb_->GetClockedOutPromImage(0x08) >> 0) & 0xf)
		   << std::hex << (( tmb_->GetClockedOutPromImage(0x09) >> 0) & 0xf)
		   << std::hex << (( tmb_->GetClockedOutPromImage(0x0a) >> 0) & 0xf)
		   << std::hex << (( tmb_->GetClockedOutPromImage(0x0b) >> 0) & 0xf) 
		   << std::endl; 
    }
    
  } else {
    (*MyOutput_) << "EMUjtag:  Number of verify errors = " 
		 << std::dec << GetNumberOfVerifyErrors() << std::endl;
    if ( GetNumberOfVerifyErrors() > 0 ) 
      ::sleep(1);
  }
  //
  return;
}
//
void EMUjtag::ProgramTMBProms() {
  //
  // N.B. we assume there is no JTAG verify done when programming the proms....
  //
  (*MyOutput_) << "EMUjtag: Programming TMB Proms..." << std::endl;
  //
  time_t starttime = time (NULL);
  //
  SetWriteToDevice_(true);
  //
  // Select the JTAG chain (puts the bootstrap register in control of the JTAG chain)
  setup_jtag(ChainTmbMezz);
  //
  // Default when programming prom is to write a logfile:
  ReadXsvfFile_(true);
  //
  if (tmb_->slot() < 22) {
    short unsigned int BootReg;
    tmb_->tmb_get_boot_reg(&BootReg);
    tmb_->tmb_set_boot_reg(BootReg & 0xff7f);     //give the JTAG chain back to the FPGA 
  }
  //
  time_t endtime = time (NULL);
  //
  int time_elapsed = endtime - starttime;
  //
  (*MyOutput_) << "EMUjtag: TMB Programming complete in " 
	       << std::dec << time_elapsed << " seconds"
	       << std::endl;  
  //
  return;
}
//
//
//////////////////////////////////////////
// File-handling tools
//////////////////////////////////////////
std::string EMUjtag::AddTrailer_(std::string filename,
				 std::string trailer) {
  //
  std::string extendedfile = filename+"."+trailer;
  return extendedfile;
}
//
//
//////////////////////////////////////////////////////////////////////////////////////////////////////
// SVF programming:
//////////////////////////////////////////////////////////////////////////////////////////////////////
int EMUjtag::SVFLoad(int *jch, const char *fn, int db ) {
  //
  // Note, in order to use this method, the TMB firmware must be loaded first...
  // Comment out below in order to use the bootstrap register
  //
  int MAXBUFSIZE=8200;
  unsigned char snd[MAXBUFSIZE], rcv[MAXBUFSIZE], expect[MAXBUFSIZE],rmask[MAXBUFSIZE],smask[MAXBUFSIZE],cmpbuf[MAXBUFSIZE];
  unsigned char sndbuf[MAXBUFSIZE],rcvbuf[MAXBUFSIZE], realsnd[MAXBUFSIZE];
  unsigned char sndhdr[MAXBUFSIZE],sndtdr[MAXBUFSIZE], sndhir[MAXBUFSIZE], sndtir[MAXBUFSIZE];
  unsigned char hdrsmask[MAXBUFSIZE],tdrsmask[MAXBUFSIZE], hirsmask[MAXBUFSIZE], tirsmask[MAXBUFSIZE];
  int rcvword;
  FILE *dwnfp;
  char buf[MAXBUFSIZE], buf2[256];
  //  char buf[8192],buf2[256];
  char *Word[256],*lastn;
  const char *downfile;
  int jchan;
  unsigned char send_tmp;//, rcv_tmp;
  int i,j,Count,nbytes,nbits,nframes,step_mode,pause;
  int hdrbits = 0, tdrbits = 0, hirbits = 0, tirbits = 0;
  int hdrbytes = 0, tdrbytes = 0, hirbytes = 0, tirbytes = 0; 
  int nowrit, cmpflag, errcntr;
  static int count;
  // MvdM struct JTAG_BBitStruct   driver_data;
  // int jtag_chain[4] = {1, 0, 5, 4};
  int jtag_chain_tmb[7] = {7, 6, 9, 8, 3, 1, 10};
  // === SIR Go through SelectDRScan->SelectIRScan->CaptureIR->ShiftIR  
  //char tms_pre_sir[4]={ 1, 1, 0, 0 }; 
  char tdi_pre_sir[4]={ 0, 0, 0, 0 };
  // === SDR Go through SelectDRScan->CaptureDR->ShiftDR
  // char tms_pre_sdr[3]={ 1, 0, 0 };
  char tdi_pre_sdr[3]={ 0, 0, 0 };
  // === SDR,SIR Go to RunTestIdle after scan
  // char tms_post[4]={ 0, 1, 1, 0 };
  char tdi_post[4]={ 0, 0, 0, 0 };
  int total_packages ;
  int send_packages ;
  //
  total_packages = 0 ;
  send_packages = 0 ;
  jchan = *jch;
  downfile = fn;
  errcntr = 0;
  if (downfile==NULL) downfile="default.svf";
  //
  printf("=== Programming Design with %s through JTAG chain %d\n",downfile, jchan);  
  //
  dwnfp = fopen(downfile,"r");
  if (dwnfp == NULL) {
    fprintf(stderr, "ERROR: failed to open file %s\n", downfile);
    return -1;
  }
  //
  while (fgets(buf,256,dwnfp) != NULL) {
    Parse(buf, &Count, &(Word[0]));
    if( strcmp(Word[0],"SDR")==0 ) total_packages++ ;
  }
  fseek(dwnfp, 0, SEEK_SET);
  //
  printf("=== Going to send %d DATA packages \n",total_packages) ;
  //
  // Uncomment the following line to use the Bootstrap register
  //  tmb_->start(jtag_chain_tmb[jchan-1]); 
  //
  tmb_->start(jtag_chain_tmb[jchan-1],jtagSourceFPGA);  
  //
  // turn on delay, otherwise the VCC's FIFO full
  tmb_->getTheController()->SetUseDelay(true);

  count=0; 
  nowrit=1;
  step_mode=0;
  while (fgets(buf,256,dwnfp) != NULL) {
    //
    if ((buf[0]=='/'&&buf[1]=='/')||buf[0]=='!') {
      if (db>4) printf("%s",buf);
      //
    } else {
      //
      if (strrchr(buf,';')==0) {
	lastn=strrchr(buf,'\r');
	if (lastn!=0) lastn[0]='\0';
	lastn=strrchr(buf,'\n');
	if (lastn!=0) lastn[0]='\0';
	memcpy(buf2,buf,256);
	Parse(buf2, &Count, &(Word[0]));
	if (( strcmp(Word[0],"SDR")==0) || (strcmp(Word[0],"SIR")==0)) {
	  sscanf(Word[1],"%d",&nbits);
	  if (nbits>MAXBUFSIZE) { 
	    // === Handle Big Bitstreams
	    (*MyOutput_) << "EMUjtag. nbits larger than buffer size" << std::endl;
	  } else do {
	    // == Handle Normal Bitstreams
	    lastn=strrchr(buf,'\r');
	    if (lastn!=0) lastn[0]='\0';
	    lastn=strrchr(buf,'\n');
	    if (lastn!=0) lastn[0]='\0';
	    if (fgets(buf2,256,dwnfp) != NULL) {
	      strcat(buf,buf2);
	    } else {
	      if (db) printf("End of File encountered.  Quiting\n");
	      return -1;
	    }
	  }
	  while (strrchr(buf,';')==0);
	}
      } 
      bzero(snd, sizeof(snd));
      bzero(cmpbuf, sizeof(cmpbuf));
      bzero(sndbuf, sizeof(sndbuf));
      bzero(rcvbuf, sizeof(rcvbuf));      
      /*
	for(i=0;i<MAXBUFSIZE;i++) {
	snd[i]=0;
	cmpbuf[i]=0;
	sndbuf[i]=0;
	rcvbuf[i]=0;
	}
      */
      Parse(buf, &Count, &(Word[0]));
      count=count+1;
      cmpflag=0;
      //
      // ==================  Parse commands from SVF file ====================
      //
      if(strcmp(Word[0],"HDR")==0) {
	//
	// === Handling HDR ===
	//
	sscanf(Word[1],"%d",&hdrbits);
	hdrbytes=(hdrbits)?(hdrbits-1)/8+1:0;
	//
	if (db)	  printf("HDR: Packing %d bits of Header Data\n", hdrbits);
	if (db>3) printf("HDR: Num of bits - %d, num of bytes - %d\n",hdrbits,hdrbytes);
	//
	for(i=2;i<Count;i+=2) {
	  if(strcmp(Word[i],"TDI")==0) {
	    if (db>5) printf("TDI ");
	    for(j=0;j<hdrbytes;j++) {
	      sscanf(&Word[i+1][2*(hdrbytes-j-1)+1],"%2X",(int *)&sndhdr[j]);
	      // printf("%2X",sndhdr[j]);
	    }
	    // printf("\n%d\n",nbytes);
	  }
	  if(strcmp(Word[i],"SMASK")==0) {
	    if (db>5) printf("SMASK ");
	    for(j=0;j<hdrbytes;j++) {
	      sscanf(&Word[i+1][2*(hdrbytes-j-1)+1],"%2X",(int *)&hdrsmask[j]);
	    }
	  }
	  if(strcmp(Word[i],"TDO")==0) {
	    if (db>5) printf("TDO ");
	    //if (db>2)             cmpflag=1;
	    cmpflag=1;
	    for(j=0;j<hdrbytes;j++) {
	      sscanf(&Word[i+1][2*(hdrbytes-j-1)+1],"%2X",(int *)&expect[j]);
	    }
	  }
	  if(strcmp(Word[i],"MASK")==0) {
	    if (db>5) printf("MASK ");
	    for(j=0;j<hdrbytes;j++) {
	      sscanf(&Word[i+1][2*(hdrbytes-j-1)+1],"%2X",(int *)&rmask[j]);
	    }
	  }
	}
	if (db>5) printf("\n");
      } else if(strcmp(Word[0],"HIR")==0) {
	//
	// === Handling HIR ===
	//
	// for(i=0;i<3;i++)sndbuf[i]=tdi_pre_sdr[i];
	// cmpflag=1;    //disable the comparison for no TDO SDR
	//
	sscanf(Word[1],"%d",&hirbits);
	hirbytes=(hirbits)?(hirbits-1)/8+1:0;
	//
	if (db)	  printf("HIR: Packing %d bits of Header Data\n", hirbits);
	if (db>3) printf("HIR: Num of bits - %d, num of bytes - %d\n",hirbits,hirbytes);
	//
	for(i=2;i<Count;i+=2) { 
	  if(strcmp(Word[i],"TDI")==0) {
	    if (db>5) printf("TDI ");
	    for(j=0;j<hirbytes;j++)  {
	      sscanf(&Word[i+1][2*(hirbytes-j-1)+1],"%2X",(int *)&sndhir[j]);
	      // printf("%2X",sndhir[j]);
	    }
	    // printf("\n%d\n",nbytes);
	  }
	  if(strcmp(Word[i],"SMASK")==0) {
	    if (db>5) printf("SMASK ");
	    for(j=0;j<hirbytes;j++) {
	      sscanf(&Word[i+1][2*(hirbytes-j-1)+1],"%2X",(int *)&hirsmask[j]);
	    }
	  }
	  if(strcmp(Word[i],"TDO")==0) {
	    if (db>5) printf("TDO ");
	    //if (db>2) cmpflag=1;
	    cmpflag=1;
	    for(j=0;j<hirbytes;j++) {
	      sscanf(&Word[i+1][2*(hirbytes-j-1)+1],"%2X",(int *)&expect[j]);
	    }
	  }
	  if(strcmp(Word[i],"MASK")==0) {
	    if (db>5) printf("MASK ");
	    for(j=0;j<hirbytes;j++) {
	      sscanf(&Word[i+1][2*(hirbytes-j-1)+1],"%2X",(int *)&rmask[j]);
	    }
	  }
	}
	if (db>5) printf("\n");
      } else if(strcmp(Word[0],"TDR")==0) {
	//
	// === Handling TDR ===
	//
	// for(i=0;i<3;i++)sndbuf[i]=tdi_pre_sdr[i];
	// cmpflag=1;    //disable the comparison for no TDO SDR
	//
	sscanf(Word[1],"%d",&tdrbits);
	tdrbytes=(tdrbits)?(tdrbits-1)/8+1:0;
	if (db)	  printf("TDR: Packing %d bits of Data Register\n", tdrbits);
	if (db>3) printf("TDR: Num of bits - %d, num of bytes - %d\n",tdrbits,tdrbytes);
	for(i=2;i<Count;i+=2) {
	  if(strcmp(Word[i],"TDI")==0) {
	    if (db>5) printf("TDI ");
	    for(j=0;j<tdrbytes;j++) {
	      sscanf(&Word[i+1][2*(tdrbytes-j-1)+1],"%2X",(int *)&sndtdr[j]);
	      // printf("%2X",sndhir[j]);
	    }
	    // printf("\n%d\n",nbytes);
	  }
	  if(strcmp(Word[i],"SMASK")==0) {
	    if (db>5) printf("SMASK ");
	    for(j=0;j<tdrbytes;j++) {
	      sscanf(&Word[i+1][2*(tdrbytes-j-1)+1],"%2X",(int *)&tdrsmask[j]);
	    }
	  }
	  if(strcmp(Word[i],"TDO")==0) {
	    if (db>5) printf("TDO ");
	    //if (db>2) cmpflag=1;
	    cmpflag=1;
	    for(j=0;j<tdrbytes;j++) {
	      sscanf(&Word[i+1][2*(tdrbytes-j-1)+1],"%2X",(int *)&expect[j]);
	    }
	  }
	  if(strcmp(Word[i],"MASK")==0) {
	    if (db>5) printf("MASK ");
	    for(j=0;j<tdrbytes;j++) {
	      sscanf(&Word[i+1][2*(tdrbytes-j-1)+1],"%2X",(int *)&rmask[j]);
	    }	
	  }
	}
      } else if(strcmp(Word[0],"TIR")==0) {
	//	  
	// === Handling TIR ===
	//
	// for(i=0;i<3;i++)sndbuf[i]=tdi_pre_sdr[i];
	// cmpflag=1;    //disable the comparison for no TDO SDR
	//
	sscanf(Word[1],"%d",&tirbits);
	tirbytes=(tirbits)?(tirbits-1)/8+1:0;
	//
	if (db)	  printf("TIR: Packing %d bits of Instruction Register\n", tdrbits);
	if (db>3) printf("TIR: Num of bits - %d, num of bytes - %d\n",tirbits,tirbytes);
	//
	for(i=2;i<Count;i+=2) {
	  if(strcmp(Word[i],"TDI")==0) {
	    if (db>5) printf("TDI ");
	    for(j=0;j<tirbytes;j++) {
	      sscanf(&Word[i+1][2*(tirbytes-j-1)+1],"%2X",(int *)&sndtir[j]);
	      // printf("%2X",sndhir[j]);
	    }
	    // printf("\n%d\n",nbytes);
	  }
	  if(strcmp(Word[i],"SMASK")==0) {
	    if (db>5) printf("SMASK ");
	    for(j=0;j<tirbytes;j++) {
	      sscanf(&Word[i+1][2*(tirbytes-j-1)+1],"%2X",(int *)&tirsmask[j]);
	    }
	  }
	  if(strcmp(Word[i],"TDO")==0) {
	    if (db>5) printf("TDO ");
	    //if (db>2) cmpflag=1;
	    cmpflag=1;
	    for(j=0;j<tirbytes;j++) {
	      sscanf(&Word[i+1][2*(tirbytes-j-1)+1],"%2X",(int *)&expect[j]);
	    }
	  }
	  if(strcmp(Word[i],"MASK")==0) {
	    if (db>5) printf("MASK ");
	    for(j=0;j<tirbytes;j++) {
	      sscanf(&Word[i+1][2*(tirbytes-j-1)+1],"%2X",(int *)&rmask[j]);
	    }
	  }
	}
	if (db>5) printf("\n");
      } else if(strcmp(Word[0],"SDR")==0) {
	//
	// === Handling SDR ===
	//
	for(i=0;i<3;i++) sndbuf[i]=tdi_pre_sdr[i];
	// cmpflag=1;    //disable the comparison for no TDO SDR
	sscanf(Word[1],"%d",&nbits);
	nbytes=(nbits)?(nbits-1)/8+1:0;
	//
	if (db)	  printf("SDR: Packing %d bits data to shift into data register...\n", nbits);
	if (db>3) printf("SDR: Num of bits - %d, num of bytes - %d\n",nbits,nbytes);
	//
	for(i=2;i<Count;i+=2) {
	  if(strcmp(Word[i],"TDI")==0) {
	    if (db>5) printf("TDI ");
	    for(j=0;j<nbytes;j++) {
	      sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&snd[j]);
	      // printf("%2X",snd[j]);
	    }
	    // printf("\n%d\n",nbytes);
	  }
	  if(strcmp(Word[i],"SMASK")==0) {
	    if (db>5) printf("SMASK ");
	    for(j=0;j<nbytes;j++) {
	      sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&smask[j]);
	    }
	  }
	  if(strcmp(Word[i],"TDO")==0) {
	    if (db>5) printf("TDO ");
	    //if (db>2) cmpflag=1;
	    cmpflag=1;
	    for(j=0;j<nbytes;j++) {
	      sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&expect[j]);
	    }
	  }
	  if(strcmp(Word[i],"MASK")==0) {
	    if (db>5) printf("MASK ");
	    for(j=0;j<nbytes;j++) {
	      sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&rmask[j]);
	    }
	  }
	}
	//
	for(i=0;i<nbytes;i++) {
	  send_tmp = snd[i]&smask[i];
	  for(j=0;j<8;j++) {
	    if ((i*8+j)< nbits) { 
	      sndbuf[i*8+j+3]=send_tmp&0x01; 
	    }
	    send_tmp = send_tmp >> 1;
	  }
	}
	for(i=0;i<4;i++) sndbuf[nbits+3]=tdi_post[i];         
	nframes=nbits+7;
	// Put send SDR here
	for (i=0; i< ((hdrbits+nbits+tdrbits-1)/8+1); i++)
	  realsnd[i] = 0;
	if (hdrbytes>0) {
	  for (i=0;i<hdrbytes;i++)
	    realsnd[i]=sndhdr[i];
	}
	for (i=0;i<nbits;i++)
	  realsnd[(i+hdrbits)/8] |= (snd[i/8] >> (i%8)) << ((i+hdrbits)%8);
	if (tdrbytes>0) {
	  for (i=0;i<tdrbits;i++)
	    realsnd[(i+hdrbits+nbits)/8] |= (sndtdr[i/8] >> (i%8)) << ((i+hdrbits+nbits)%8);
	}
	//
	if (db>6) {	
	  printf("SDR Send Data:\n");
	  for (i=0; i< ((hdrbits+nbits+tdrbits-1)/8+1); i++)
	    printf("%02X",realsnd[i]);
	  printf("\n");
	}
	send_packages++ ;
	printf("%c[0m", '\033');
	printf("%c[1m", '\033');
	//printf("%c[2;50H", '\033');
	if ( send_packages == 1 ) {
	  printf("%c7", '\033');
	}
	printf("%c8", '\033'); 
	printf(" Sending %d/%d ",send_packages,total_packages) ;
	printf("%c8", '\033'); 
	printf("%c[0m", '\033');
	if ( send_packages == total_packages ) printf("\n") ;
	//
	if (db) printf("SDR Send NOW\n");
	//	tmb_->scan(DATA_REG, (char*)realsnd, hdrbits+nbits+tdrbits, (char*)rcv, READ_BACK); 
	tmb_->scan(DATA_REG, (char*)realsnd, hdrbits+nbits+tdrbits, (char*)rcv, NO_READ_BACK); 
	//
	if (cmpflag==1) {     
	  //
	  if (db>6) {	  	
	    printf("SDR Readback Data: ");
	    for (i=0; i< ((hdrbits+nbits+tdrbits-1)/8+1); i++) 
	      printf("%02X",rcv[i]);
	    printf("\n");
	  }	
	  //    
	  for(i=0;i<nbytes;i++)	{
	    rcvword = rcv[i+(hdrbits/8)]+(((int)rcv[i+1+(hdrbits/8)])<<8);
	    rcvword = rcvword>>(hdrbits%8);
	    rcvword = rcv[i];
	    // if (((rcv[nbytes-1-i]^expect[i]) & (rmask[i]))!=0 && cmpflag==1)
	    if ((((rcvword&0xFF)^expect[i]) & (rmask[i]))!=0 && cmpflag==1) {
	      if (db)
		printf("SDR read back wrong, at i %02d  rdbk %02X  expect %02X  rmask %02X\n",
		       i,rcv[i]&0xFF,expect[i]&0xFF,rmask[i]&0xFF);
	      errcntr++;
	    }
	  }	
	}
	//
      } else if(strcmp(Word[0],"SIR")==0) {
	//
        // === Handling SIR ===
	//
	for(i=0;i<4;i++) sndbuf[i]=tdi_pre_sir[i];
	// cmpflag=1;    //disable the comparison for no TDO SDR
	sscanf(Word[1],"%d",&nbits);
	nbytes=(nbits)?(nbits-1)/8+1:0;
	//
	if (db)	  printf("SIR: Packing %d bits of data to shift into the instruction register...\n",nbits);
	if (db>3) printf("SIR: Num of bits - %d, num of bytes - %d\n",nbits,nbytes);
	//
	for(i=2;i<Count;i+=2) {
	  if(strcmp(Word[i],"TDI")==0) {
	    if (db>5) printf("TDI ");
	    for(j=0;j<nbytes;j++) {
	      sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&snd[j]);
	    }
	  }
	  if(strcmp(Word[i],"SMASK")==0) {
	    if (db>5) printf("SMASK ");
	    for(j=0;j<nbytes;j++) {
	      sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&smask[j]);
	    }
	  }
	  if(strcmp(Word[i],"TDO")==0) {
	    if (db>5) printf("TDO ");
	    cmpflag=1;
	    // if (db>2) cmpflag=1;
	    for(j=0;j<nbytes;j++) {
	      sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&expect[j]);
	    }
	  }
	  if(strcmp(Word[i],"MASK")==0) {
	    if (db>5) printf("MASK ");
	    for(j=0;j<nbytes;j++) {
	      sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&rmask[j]);
	    }
	  }
	}
	for(i=0;i<nbytes;i++) {
	  send_tmp = snd[i]&smask[i];
	  // printf("\n%d - ", send_tmp);
	  for(j=0;j<8;j++) {
	    if ((i*8+j)< nbits) {
	      sndbuf[i*8+j+4]=send_tmp&0x01;
	      // printf("%d", sndbuf[i*8+j+4]);
	    }
	    send_tmp = send_tmp >> 1;
	  }
	}
	for(i=0;i<4;i++) sndbuf[nbits+4]=tdi_post[i];
	nframes=nbits+8;
	// Put send SIR here
	for (i=0; i< ((hirbits+nbits+tirbits-1)/8+1);  i++)
	  realsnd[i] = 0;
	if (hirbytes>0) {
	  for (i=0;i<hirbytes;i++)
	    realsnd[i]=sndhir[i];
	}
	for (i=0;i<nbits;i++)
	  realsnd[(i+hirbits)/8] |= (snd[i/8] >> (i%8)) << ((i+hirbits)%8);
	if (tirbytes>0) {
	  for (i=0;i<tirbits;i++)
	    realsnd[(i+hirbits+nbits)/8] |= (sndtir[i/8] >> (i%8)) << ((i+hirbits+nbits)%8);
	}
	//
	if (db>6) { 	
	  printf("SIR Send Data: ");
	  for (i=0; i< ((hirbits+nbits+tirbits-1)/8+1);  i++)
	    printf("%02X",realsnd[i]);
	  printf("\n");
	}
	//
	if (db) printf("SIR Send NOW\n");
	tmb_->scan(INSTR_REG, (char*)realsnd, hirbits+nbits+tirbits, (char*)rcv, NO_READ_BACK); 
	//
	if (cmpflag==1) {
	  //
	  if (db>6) {                
	    printf("SIR Readback Data: ");
	    for (i=0; i< ((hdrbits+nbits+tdrbits-1)/8+1); i++) 
	      printf("%02X",rcv[i]);
	    printf("\n");
	  }
	  //	  
	  for(i=0;i<nbytes;i++) {
	    rcvword = rcv[i+(hirbits/8)]+(((int)rcv[i+1+(hirbits/8)])<<8);
	    rcvword = rcvword>>(hirbits%8);
	    rcvword = rcv[i];
	    // if (((rcv[nbytes-1-i]^expect[i]) & (rmask[i]))!=0 && cmpflag==1)
	    if ((((rcvword&0xFF)^expect[i]) & (rmask[i]))!=0 && cmpflag==1) {
	      if (db)
		printf("SIR read back wrong, at i %02d  rdbk %02X  expect %02X  rmask %02X\n",
		       i,rcv[i]&0xFF,expect[i]&0xFF,rmask[i]&0xFF);
	      errcntr++;
	    }
	  }
	}
	/*
	  if (cmpflag==1)
	  {
	  for(i=4;i<nbits+4;i++) printf("%d",rcvbuf[i]);
	  printf("\n");
	  }
	*/   
      } else if(strcmp(Word[0],"RUNTEST")==0) {
	//
	// === Handling RUNTEST ===
	//
	sscanf(Word[1],"%d",&pause);
	if (db) printf("RUNTEST:  %d\n",pause);
	usleep(pause+100);
	// 
      }	else if((strcmp(Word[0],"STATE")==0)&&(strcmp(Word[1],"RESET")==0)&&(strcmp(Word[2],"IDLE;")==0)) {
	//	
	// === Handling STATE ===
	//
      } else if(strcmp(Word[0],"TRST")==0) {
	//
	// === Handling TRST ===
	//
      } else if(strcmp(Word[0],"ENDIR")==0) {
	//
	// === Handling ENDIR ===
	//
      } else if(strcmp(Word[0],"ENDDR")==0) {
	//
	// === Handling ENDDR ===
	//
      }
      //      ::sleep(1);
    }
  }
  //
  tmb_->endDevice();
  // turn off delay.
  tmb_->getTheController()->SetUseDelay(false);
  fclose(dwnfp);
  return errcntr; 
}
//
void EMUjtag::Parse(char *buf,int *Count,char **Word) {
  //
  // SVF File Parser module
  //
  *Word = buf;
  *Count = 0;
  while(*buf != '\0')  {
    while ((*buf==' ') || (*buf=='\t') || (*buf=='\n') || (*buf=='"')) *(buf++)='\0';
    if ((*buf != '\n') && (*buf != '\0'))  {
      Word[(*Count)++] = buf;
    }
    while ((*buf!=' ')&&(*buf!='\0')&&(*buf!='\n')&&(*buf!='\t')&&(*buf!='"')) {
      buf++;
    }
  }
  buf = '\0';
}


