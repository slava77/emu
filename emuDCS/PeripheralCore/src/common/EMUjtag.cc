#include "EMUjtag.h"
//
#include <iostream>
#include <iomanip>
#include <unistd.h> // for sleep
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
//
#include "TMB.h"

//EMUjtag::EMUjtag(){
//}
//
EMUjtag::EMUjtag(TMB * tmb) :
  tmb_(tmb),debug_(false)
{
  //
  std::cout << "Creating EMUjtag" << std::endl ;
  //
  MyOutput_ = &std::cout ;
  //
  SetXsvfFilename("tempfilename");
  prom_file_ok_ = false;
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
//////////////////////////////////////////////////////////////////////////////////////////////
// Use EMUjtag to shift data into Instruction Registers and into (and out of) Data Registers
//////////////////////////////////////////////////////////////////////////////////////////////
void EMUjtag::setup_jtag(int chain) {
  //
  //This member sets the following characteristics:
  //  - which JTAG chain you are looking at
  //  - how many chips are on the chain
  //  - the number of bits in each chip's opcode
  //  - chip_id_ to dummy value (in excess of possible number of devices on chain)
  //  - register length to zero 
  //start(N):
  //  - set the jtag chain for the boot register used in VMEController_jtag::scan(...)
  //
  if(debug_){
    std::cout << "setup_chain" << std::endl ;
  }
  //
  jtag_chain_ = chain;
  devices_in_chain_ = 0;
  for (int device=0; device<MAX_NUM_DEVICES; device++)
    bits_in_opcode_[device] = 0;
  //
  if (jtag_chain_ == ChainAlctSlowFpga) {
    //
    devices_in_chain_ = NumberChipsAlctSlowFpga; 
    bits_in_opcode_[0] = OpcodeSizeAlctSlowFpga;
    tmb_->start(6);
    //
  } else if (jtag_chain_ == ChainAlctSlowProm) {
    //
    devices_in_chain_ = NumberChipsAlctSlowProm;
    bits_in_opcode_[0] = OpcodeSizeAlctSlowProm;
    bits_in_opcode_[1] = OpcodeSizeAlctSlowProm;
    bits_in_opcode_[2] = OpcodeSizeAlctSlowProm;
    tmb_->start(7);
    //
  } else if (jtag_chain_ == ChainAlctFastFpga) {
    //
    devices_in_chain_ = NumberChipsAlctFastFpga; 
    bits_in_opcode_[0] = OpcodeSizeAlctFastFpga;
    tmb_->start(8);
    //
  } else if (jtag_chain_ == ChainTmbMezz) { 
    //
    devices_in_chain_ = NumberChipsTmbMezz;
    bits_in_opcode_[0] = OpcodeSizeTmbMezzFpga;
    bits_in_opcode_[1] = OpcodeSizeTmbMezzProm;
    bits_in_opcode_[2] = OpcodeSizeTmbMezzProm;
    bits_in_opcode_[3] = OpcodeSizeTmbMezzProm;
    bits_in_opcode_[4] = OpcodeSizeTmbMezzProm;
    tmb_->start(3);
    //
  } else if (jtag_chain_ == ChainTmbUser) { 
    //
    devices_in_chain_ = NumberChipsTmbUser;
    bits_in_opcode_[0] = OpcodeSizeTmbUserProm;
    bits_in_opcode_[1] = OpcodeSizeTmbUserProm;
    tmb_->start(4);
    //
  } else if (jtag_chain_ == ChainRat) {
    //
    devices_in_chain_ = NumberChipsRat;
    bits_in_opcode_[0] = OpcodeSizeRatFpga;
    bits_in_opcode_[1] = OpcodeSizeRatProm;
    tmb_->start(10);
    //
  }
  //
  if (devices_in_chain_ == 0) {
    (*MyOutput_) << "EMUjtag: Unsupported JTAG chain " << jtag_chain_ <<std::endl;
    jtag_chain_ = -1;
    ::sleep(5);    
  }  else {
    //
    tmb_->RestoreIdle();      //Valid JTAG chain:  bring the state machine to Run-Time Idle
    //
    if(debug_){
      (*MyOutput_) << "EMUjtag: JTAG chain " << std::hex << jtag_chain_ 
		   << " has " << std::dec << devices_in_chain_ << " devices" << std::endl;
    }
  }
  //
  chip_id_ = MAX_NUM_DEVICES;
  register_length_ = 0;
  //
  return;
}
//
void EMUjtag::ShfIR_ShfDR(const int selected_chip, 
			  const int opcode, 
			  const int size_of_register, 
			  const int * write_data) {
  //
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
  //
  // N.B. JTAG instructions and data are loaded and readout in order
  //      starting with chipN, chipN-1, ... ,chip1, and ending with chip0
  //
  if (jtag_chain_ < 0) return;
  //
  chip_id_ = selected_chip;
  register_length_ = size_of_register;
  //
  if (debug_){    
    (*MyOutput_) << "EMUjtag: Use " << std::dec << bits_in_opcode_[chip_id_] 
		 << " bits to write opcode 0x" << std::hex << opcode 
		 << " to chip " << std::dec << chip_id_ 
		 << " on chain 0x" << std::hex << jtag_chain_
		 << " -> use " << std::dec << register_length_ 
		 << " bits tdi/tdo" << std::hex << std::endl;
  }
  //
  //** Clear the read data which was previously there:
  for (int i=0; i<MAX_NUM_FRAMES; i++) 
    shfDR_tdo_[i] = 0;
  //
  int tdi[MAX_NUM_FRAMES];
  char sndBuffer[MAX_BUFFER_SIZE], rcvBuffer[MAX_BUFFER_SIZE];
  //
  // ** Clean buffers:
  for (int i=0; i<MAX_BUFFER_SIZE; i++) {
    sndBuffer[i] = 0;
    rcvBuffer[i] = 0;
  }
  int iframe = 0;                              //reset frame counter
  //
  // ** Construct opcode for the selected chip (all but chip_id are BYPASS = all 1's),
  int idevice, ichip, ibit;
  int bit;
  for (idevice=0; idevice<devices_in_chain_; idevice++) {          //loop over all the chips in this chain
    //
    ichip = devices_in_chain_ - idevice - 1;                       //chip order in chain is reversed
    //
    for (ibit=0; ibit<bits_in_opcode_[ichip]; ibit++) {            //up to the number of bits in this chip's opcode
      bit = 1;                                                     //BYPASS
      if (ichip == chip_id_)                                       //this is the chip we want
	bit = (opcode >> ibit) & 0x1;                              //extract bit from opcode
      tdi[iframe++]=bit;
    }
  }
  //
  //  (*MyOutput_) << "There are " << std::dec << iframe << " frames to send..." << std::endl;
  //
  if (iframe > MAX_NUM_FRAMES) 
    (*MyOutput_) << "EMUjtag: ShfIR_ShfDR IR ERROR: Too many frames -> " << iframe << std::endl;
  //
  //pack tdi into an array of char so scan can handle it:
  packCharBuffer(tdi,iframe,sndBuffer);
  //
  //  (*MyOutput_) << "sndBuffer to ShfIR=";
  //  for (int i=iframe/8; i>=0; i--) 
  //    (*MyOutput_) << " " << std::hex << (sndBuffer[i]&0xff); 
  //  (*MyOutput_) << std::endl;
  //
  tmb_->scan(INSTR_REGISTER, sndBuffer, iframe, rcvBuffer, NO_READ_BACK);
  //
  //
  // ** Second JTAG operation is to shift out the data register...
  // **Clean buffers**
  for (int i=0; i<MAX_BUFFER_SIZE; i++) {
    sndBuffer[i] = 0;
    rcvBuffer[i] = 0;
  }
  iframe = 0;                              //reset frame counter
  //
  // ** Set up TMS to shift in the data bits for this chip, BYPASS code for others **
  int offset;
  //
  for (idevice=0; idevice<devices_in_chain_; idevice++) {  // loop over all of the chips in this chain
    //
    ichip = devices_in_chain_ - idevice - 1;               // chip order in chain is reversed
    //
    if (ichip == chip_id_) {                               // this is the chip we want
      offset = iframe;                                     // here is the beginning of the data
      for (ibit=0; ibit<register_length_; ibit++)          // up to the number of bits specified for this register
	tdi[iframe++] = write_data[ibit];                  // Shift in the data for TDI
    } else {                                               // bypass register is one frame      
      tdi[iframe++] = 0;                                   // No data goes out to bypass regs
    }
  }
  //
  if (iframe > MAX_NUM_FRAMES) 
    (*MyOutput_) << "EMUjtag: ShfIR_ShfDR DR ERROR: Too many frames -> " << iframe << std::endl;
  //
  //pack tdi into an array of char so scan can handle it:
  packCharBuffer(tdi,iframe,sndBuffer);
  //
  //  (*MyOutput_) << "write_data  = ";
  //  for (int i=register_length_-1; i>=0; i--)
  //    (*MyOutput_) << write_data[i];
  //  (*MyOutput_) << std::endl;
  //
  //  (*MyOutput_) << "TDI into DR = ";
  //  for (int i=iframe-1; i>=0; i--)
  //    (*MyOutput_) << tdi[i];
  //  (*MyOutput_) << std::endl;
  //
  //  (*MyOutput_) << "sndBuffer to ShfDR=";
  //  for (int i=iframe/8; i>=0; i--) 
  //    (*MyOutput_) << ((sndBuffer[i] >> 4) & 0xf) << (sndBuffer[i] & 0xf);  
  //  (*MyOutput_) << std::endl;
  //
  tmb_->scan(DATA_REGISTER, sndBuffer, iframe, rcvBuffer, READ_BACK);
  //
  // ** copy relevant section of tdo to data array **
  unpackCharBuffer(rcvBuffer,register_length_,offset,shfDR_tdo_);
  //
  //  (*MyOutput_) << "TDO from DR = ";
  //  for (int i=register_length_-1; i>=0; i--)
  //    (*MyOutput_) << shfDR_tdo_[i];
  //  (*MyOutput_) << std::endl;
  //
  //  char tempBuffer[MAX_BUFFER_SIZE];
  //  packCharBuffer(shfDR_tdo_,register_length_,tempBuffer);
  //  for (int i=(register_length_/8)-1; i>=0; i--) 
  //    (*MyOutput_) << ((tempBuffer[i] >> 4) & 0xf) << (tempBuffer[i] & 0xf);  
  //  (*MyOutput_) << std::endl;
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
    for (int i=(length/8)-1; i>=0; i--) 
      writedump << std::hex << ((WriteBuffer[i] >> 4) & 0xf) << (WriteBuffer[i] & 0xf);  
    //
    std::ostringstream readdump;
    char ReadBuffer[MAX_BUFFER_SIZE];
    packCharBuffer(read_vector,length,ReadBuffer);
    for (int i=(length/8)-1; i>=0; i--) 
      readdump << std::hex << ((ReadBuffer[i] >> 4) & 0xf) << (ReadBuffer[i] & 0xf);  
    //
    std::ostringstream dump;
    dump << "EMUjtag: JTAG READ ->" << readdump.str() << " does not equal WRITE->" << writedump.str();
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
  //    (*MyOutput_) << "value = " << std::hex << value << std::endl;
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
//----------------------------------//
// prom image file create/read
//----------------------------------//
void EMUjtag::CreateUserPromFile() {
  //
  (*MyOutput_) << "EMUjtag:  Creating user prom image file" << std::endl;
  //
  // Create the data which are shifted out by the user prom
  //
  //*******************************//
  // dummy data:  walking ones...
  //  N.B. depending on the header/trailer in the blocks as required by the TMB,   
  //       we need a maximum here which corresponds with the size of the prom....
  int data_count=285;
  int data_to_prom[data_count];
  //
  for (int address=0; address<data_count; address++) {
    data_to_prom[address] = 1 << (address%8);
    if (address==0) data_to_prom[address] = (int)'C';
    if (address==1) data_to_prom[address] = (int)'a';
    if (address==2) data_to_prom[address] = (int)'n';
    if (address==3) data_to_prom[address] = (int)' ';
    if (address==4) data_to_prom[address] = (int)'y';
    if (address==5) data_to_prom[address] = (int)'o';
    if (address==6) data_to_prom[address] = (int)'u';
    if (address==7) data_to_prom[address] = (int)' ';
    if (address==8) data_to_prom[address] = (int)'r';
    if (address==9) data_to_prom[address] = (int)'e';
    if (address==10) data_to_prom[address] = (int)'a';
    if (address==11) data_to_prom[address] = (int)'d';
    if (address==12) data_to_prom[address] = (int)' ';
    if (address==13) data_to_prom[address] = (int)'m';
    if (address==14) data_to_prom[address] = (int)'e';
    if (address==15) data_to_prom[address] = (int)'?';
    //
    if (address==253) data_to_prom[address] = (int)'H';
    if (address==254) data_to_prom[address] = (int)'e';
    if (address==255) data_to_prom[address] = (int)'r';
    if (address==256) data_to_prom[address] = (int)'e';
    if (address==257) data_to_prom[address] = (int)' ';
    if (address==258) data_to_prom[address] = (int)'i';
    if (address==259) data_to_prom[address] = (int)'s';
    if (address==260) data_to_prom[address] = (int)' ';
    if (address==261) data_to_prom[address] = (int)'t';
    if (address==262) data_to_prom[address] = (int)'h';
    if (address==263) data_to_prom[address] = (int)'e';
    if (address==264) data_to_prom[address] = (int)' ';
    if (address==265) data_to_prom[address] = (int)'s';
    if (address==266) data_to_prom[address] = (int)'e';
    if (address==267) data_to_prom[address] = (int)'c';
    if (address==268) data_to_prom[address] = (int)'o';
    if (address==269) data_to_prom[address] = (int)'n';
    if (address==270) data_to_prom[address] = (int)'d';
    if (address==271) data_to_prom[address] = (int)' ';
    if (address==272) data_to_prom[address] = (int)'b';
    if (address==273) data_to_prom[address] = (int)'l';
    if (address==274) data_to_prom[address] = (int)'o';
    if (address==275) data_to_prom[address] = (int)'c';
    if (address==276) data_to_prom[address] = (int)'k';
    if (address==277) data_to_prom[address] = (int)' ';
    if (address==278) data_to_prom[address] = (int)'o';
    if (address==279) data_to_prom[address] = (int)'f';
    if (address==280) data_to_prom[address] = (int)' ';
    if (address==281) data_to_prom[address] = (int)'d';
    if (address==282) data_to_prom[address] = (int)'a';
    if (address==283) data_to_prom[address] = (int)'t';
    if (address==284) data_to_prom[address] = (int)'a';
  }
  // end dummy data
  //*******************************//
  //
  InsertBlockBoundaries_(data_to_prom,data_count);
  //
  WritePromDataToDisk_();
  //
  return;
}
//
void EMUjtag::InsertBlockBoundaries_(int * data_to_go_into_prom,
				     int number_of_data_words_to_go_into_prom) {
  // clear the ascii prom image
  for (int i=0; i<TOTAL_NUMBER_OF_ADDRESSES; i++) 
    SetUserPromImage_(i,0);
  //
  // Fill the ascii prom image with the desired data.
  // Each block has a header and trailer which needs to be 
  // inserted into the stream...
  //
  int block_counter = 0;
  int data_counter = 0;
  int address_counter = 0;
  while (address_counter < TOTAL_NUMBER_OF_ADDRESSES) {
    //
    int previous_address = address_counter;
    //
    // block header goes here:
    //
    // data:
    if ( data_counter < number_of_data_words_to_go_into_prom)
      SetUserPromImage_(address_counter++,
			data_to_go_into_prom[data_counter++]);
    //
    // block trailer:
    if ( (address_counter%NUMBER_OF_ADDRESSES_PER_BLOCK) == 
	 (NUMBER_OF_ADDRESSES_PER_BLOCK-3) )
      SetUserPromImage_(address_counter++,
			block_counter++);
    //
    if ( (address_counter%NUMBER_OF_ADDRESSES_PER_BLOCK) == 
	 (NUMBER_OF_ADDRESSES_PER_BLOCK-2) )
      SetUserPromImage_(address_counter++,
			0xab);
    //
    if ( (address_counter%NUMBER_OF_ADDRESSES_PER_BLOCK) == (NUMBER_OF_ADDRESSES_PER_BLOCK-1) )
      SetUserPromImage_(address_counter++,
			0xcd);
    //
    if (address_counter == previous_address) address_counter++; 
    //
  }
  //
  return;
}
//
void EMUjtag::ReadUserPromFile() {
  //
  (*MyOutput_) << "EMUjtag:  READ user prom image file " << filename_dat_ << std::endl;
  //
  for (int address=0; address<TOTAL_NUMBER_OF_ADDRESSES; address++)
    read_ascii_prom_image_[address]=0;
  //
  std::ifstream Readfile;
  Readfile.open(filename_dat_.c_str());
  //
  if ( Readfile.is_open() ) {
    //
    prom_file_ok_ = true;
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
      int index_value, image_value;
      instring >> std::hex >> index_value >> image_value;
      //
      //      std::cout << "line " << std::dec << index_value 
      //		<< ", image = " << image_value
      //		<< std::endl;
      //
      read_ascii_prom_image_[index_value] = image_value;
    } 
  } else {
    //
    prom_file_ok_ = false;
    //
    (*MyOutput_) << "EMUjtag:  ERROR Prom data file " << filename_dat_ 
		 << " does not exist.  Please create it..." << std::endl;
  }
  //
  Readfile.close();
  //
  return;
}
//
void EMUjtag::WritePromDataToDisk_() {
  //
  // prom image file has format AAAA DD -> AAAA=address, DD=prom data
  //
  (*MyOutput_) << "EMUjtag: Write file " << filename_dat_ << " to disk" << std::endl;
  //
  std::ofstream file_to_write;
  file_to_write.open(filename_dat_.c_str());
  //
  for (int index=0; index<TOTAL_NUMBER_OF_ADDRESSES; index++) 
    file_to_write << std::hex 
		  << std::setw(5) << index                     
		  << std::setw(3) << write_ascii_prom_image_[index]
		  << std::endl;
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
  return read_ascii_prom_image_[address];
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
    (*MyOutput_) << "SetUserPromImage ERROR: value " << value 
		 << " is too large...  should be between 0 and 255" 
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
// XSVF file create/read
//----------------------------------//
void EMUjtag::CreateXsvfFile() {
  //
  (*MyOutput_) << "EMUjtag:  Creating XSVF file..." << std::endl;
  //
  for (int i=0; i<MAX_XSVF_IMAGE_NUMBER; i++) 
    write_xsvf_image_[i] = 0;
  //
  ReadUserPromFile();
  //
  if ( !prom_file_ok_ ) {
    (*MyOutput_) << "EMUjtag:  Not creating XSVF file..." << std::endl;
    return;
  }
  //
  // For the moment just put in dummy data....
  number_of_write_bytes_ = MAX_XSVF_IMAGE_NUMBER;
  for (int i=0; i<number_of_write_bytes_; i++) 
    SetWriteXsvfImage_(i,i%256);
  //
  WriteXsvfDataToDisk_();
  //
  return;
}
//
void EMUjtag::WriteXsvfDataToDisk_() {
  //
  (*MyOutput_) << "EMUjtag: Write XSVF file " << filename_xsvf_ << " to disk" << std::endl;
  //
  std::ofstream file_to_write;
  file_to_write.open(filename_xsvf_.c_str(),
		     std::ios::binary);        
  //
  for (int index=0; index<number_of_write_bytes_; index++) 
      file_to_write << write_xsvf_image_[index];
  //
  file_to_write.close();
  //
  return;
}
//
void EMUjtag::ReadXsvfFile() {
  //
  // Default is no logfile written:
  ReadXsvfFile(false);
  //
  return;
}
//
void EMUjtag::ReadXsvfFile(bool create_logfile) {
  //
  (*MyOutput_) << "EMUjtag:  Read XSVF file " << filename_xsvf_ << " from disk" << std::endl;
  //
  for (int i=0; i<MAX_XSVF_IMAGE_NUMBER; i++) 
    read_xsvf_image_[i]=0; 
  //
  std::ifstream Readfile;
  Readfile.open(filename_xsvf_.c_str(),
		    std::ifstream::binary);     //xsvf file is binary
  //
  int byte_counter=0;
  while ( Readfile.good() ) 
    read_xsvf_image_[byte_counter++] = Readfile.get();
  //
  number_of_read_bytes_ = --byte_counter;
  //
  //  for (int i=200; i<300; i++) 
  //    std::cout << "read_xsvf_image_[" << std::dec << i 
  //	      << "] = " << read_xsvf_image_[i] << std::endl;
  //
  Readfile.close();
  //
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
  //    if ( (byte+1)%10 == 0) 
  //      Logfile_ << std::endl;
  //  }
  //  Logfile_ << std::endl;
  //
  DecodeXsvfImage_();
  //
  if (create_logfile)
    Logfile_.close();
  //
  return;
}
//
void EMUjtag::ParseXCOMPLETE_() {
  //
  if (NumberOfCommands_[XCOMPLETE] > 1) {
    (*MyOutput_) << "EMUjtag: XCOMPLETE ERROR multiple eof's reached..." << std::endl;
    image_counter_ = MAX_XSVF_IMAGE_NUMBER;
    return;
  }
  //
  Logfile_ << "XCOMPLETE -> End of XSVF file reached" << std::endl;
  return;
}
//
void EMUjtag::ParseXTDOMASK_() {
  //
  for (int byte=0; byte<MAX_BYTES_TDO; byte++) 
    tdomask_[byte] = 0;
  //
  int number_of_bytes = (xdr_length_-1)/8 + 1;
  //
  if (number_of_bytes > MAX_BYTES_TDO) {
    (*MyOutput_) << "EMUjtag: XTDOMASK ERROR number of tdo bytes " << number_of_bytes 
		 << " greater than " << MAX_BYTES_TDO << std::endl;
    image_counter_=MAX_XSVF_IMAGE_NUMBER;
    return;
  }
  //
  for (int byte=0; byte<number_of_bytes; byte++) 
    tdomask_[byte] = GetReadXsvfImage_(image_counter_++) & 0xff;
      //
  Logfile_ << "XTDOMASK -> " << xdr_length_ << " bits, mask =";
  for (int byte=0; byte<number_of_bytes; byte++)
    Logfile_ << std::hex << ( (tdomask_[byte]>>4) & 0xf) << (tdomask_[byte] & 0xf);
  Logfile_ << std::endl;
  //
  return;
}
//
void EMUjtag::ParseXSIR_() {
  //
  for (int byte=0; byte<MAX_BYTES_TDI; byte++) 
    tdivalue_[byte] = 0;
  //
  int number_of_bits = GetReadXsvfImage_(image_counter_++) & 0xff;
  int number_of_bytes = (number_of_bits-1)/8+1;
  //
  for (int byte=0; byte<number_of_bytes; byte++) 
    tdivalue_[byte] = GetReadXsvfImage_(image_counter_++) & 0xff;
  //
  Logfile_ << "XSIR -> " << number_of_bits << " bits, TDI value =";
  for (int byte=0; byte<number_of_bytes; byte++)
    Logfile_ << std::hex << ( (tdivalue_[byte]>>4) & 0xf) << (tdivalue_[byte] & 0xf);
  Logfile_ << std::endl;
  //
  return;
}
//
void EMUjtag::ParseXRUNTEST_() {
  //
  //pack 4 bytes to give the length of the pause
  //
  int xruntest_time = 0;
  for (int i=3; i>=0; i--) 
    xruntest_time |= ( (GetReadXsvfImage_(image_counter_++)&0xff) << 8*i);
  //
  Logfile_ << "XRUNTEST -> time = " << std::dec << xruntest_time << " uSec " <<  std::endl;      
  return;
}
//
void EMUjtag::ParseXREPEAT_() {
  //
  int number_of_times = GetReadXsvfImage_(image_counter_++) & 0xff;
  //
  Logfile_ << "XREPEAT -> " << number_of_times << " times" << std::endl;
  return;
}
//
void EMUjtag::ParseXSDRSIZE_() {
  //
  //pack 4 bytes to give the length of the data register
  //
  xdr_length_ = 0;
  for (int i=3; i>=0; i--) 
    xdr_length_ |= ( (GetReadXsvfImage_(image_counter_++)&0xff) << 8*i);
  //
  Logfile_ << "XSDRSIZE -> length = " << std::dec << xdr_length_ << std::endl;      
  return;
}
//
void EMUjtag::ParseXSDRTDO_() {
  //
  for (int byte=0; byte<MAX_BYTES_TDI; byte++) 
    tdivalue_[byte] = 0;
  //
  int number_of_bytes = (xdr_length_-1)/8 + 1;
  //
  if (number_of_bytes > MAX_BYTES_TDI) {
    (*MyOutput_) << "EMUjtag: XSDRTDO ERROR number of tdi bytes " << number_of_bytes 
		 << " greater than " << MAX_BYTES_TDI << std::endl;
    image_counter_=MAX_XSVF_IMAGE_NUMBER;
    return;
  }
  //
  for (int byte=0; byte<number_of_bytes; byte++)
    tdivalue_[byte] = GetReadXsvfImage_(image_counter_++) & 0xff;
      //
  Logfile_ << "XSDRTDO -> " << std::dec << xdr_length_ << " bits, TDI value =";
  for (int byte=0; byte<number_of_bytes; byte++)
    Logfile_ << std::hex 
	     << ( (tdivalue_[byte]>>4) & 0xf ) 
	     << (tdivalue_[byte] & 0xf);
  Logfile_ << std::endl;
  //
  for (int byte=0; byte<MAX_BYTES_TDO; byte++) 
    tdoexpected_[byte] = 0;
  //
  if (number_of_bytes > MAX_BYTES_TDO) {
    (*MyOutput_) << "EMUjtag: XSDRTDO ERROR number of expected tdo bytes " << number_of_bytes 
		 << " greater than " << MAX_BYTES_TDO << std::endl;
    image_counter_=MAX_XSVF_IMAGE_NUMBER;
    return;
  }
  //
  for (int byte=0; byte<number_of_bytes; byte++)
    tdoexpected_[byte] = GetReadXsvfImage_(image_counter_++) & 0xff;
      //
  Logfile_ << "XSDRTDO -> " << std::dec << xdr_length_ << " bits, TDO expected =";
  for (int byte=0; byte<number_of_bytes; byte++)
    Logfile_ << std::hex 
	     << ( (tdoexpected_[byte]>>4) & 0xf ) 
	     << (tdoexpected_[byte] & 0xf);
  Logfile_ << std::endl;
  //
  return;
}
//
void EMUjtag::ParseXSTATE_() {
  int state = GetReadXsvfImage_(image_counter_++) & 0xff;
  //
  if (state == 0) {
    Logfile_ << "XSTATE -> TLR" << std::endl;
  } else if (state == 1) {
    Logfile_ << "XSTATE -> RTI" << std::endl;
  } else {
    Logfile_ << "XSTATE -> ???" << std::endl;
  }
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
  while (image_counter_ < number_of_read_bytes_) {
    //
    int command = GetReadXsvfImage_(image_counter_++);
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
int EMUjtag::GetReadXsvfImage_(int address) {
  //
  if (address >= MAX_XSVF_IMAGE_NUMBER) {
    (*MyOutput_) << "GetReadXsvfImage ERROR: address " << address 
		 << " out of range...  should be between 0 and " << MAX_XSVF_IMAGE_NUMBER-1 
		 << std::endl;
    return 0;
  }
  //
  return read_xsvf_image_[address]; 
}
//
void EMUjtag::SetWriteXsvfImage_(int address, int value) {
  //
  if (address >= MAX_XSVF_IMAGE_NUMBER) {
    (*MyOutput_) << "SetReadXsvfImage ERROR: address " << address 
		 << " out of range...  should be between 0 and " << MAX_XSVF_IMAGE_NUMBER-1 
		 << std::endl;
    return;
  }
  //
  write_xsvf_image_[address] = (char) (value & 0xff);
  return; 
}
//
//
//////////////////////////////////////////
// File-handling tools
//////////////////////////////////////////
void EMUjtag::SetXsvfFilename(std::string filename) {
  //
  filename_dat_ = AddTrailer_(filename,"dat");  
  filename_xsvf_ = AddTrailer_(filename,"xsvf");  
  filename_log_ = AddTrailer_(filename,"log");  
  //
  return;
}
//
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
int EMUjtag::SVFLoad(int *jch, const char *fn, int db )
{
  int MAXBUFSIZE=8200;
  unsigned char snd[MAXBUFSIZE], rcv[MAXBUFSIZE], expect[MAXBUFSIZE],rmask[MAXBUFSIZE],smask[MAXBUFSIZE],cmpbuf[MAXBUFSIZE];
  unsigned char sndbuf[MAXBUFSIZE],rcvbuf[MAXBUFSIZE], realsnd[MAXBUFSIZE];
  unsigned char sndhdr[MAXBUFSIZE],sndtdr[MAXBUFSIZE], sndhir[MAXBUFSIZE], sndtir[MAXBUFSIZE];
  unsigned char hdrsmask[MAXBUFSIZE],tdrsmask[MAXBUFSIZE], hirsmask[MAXBUFSIZE], tirsmask[MAXBUFSIZE];
  int rcvword;
  FILE *dwnfp, *ftmptdi, *ftmpsmask;
  char buf[MAXBUFSIZE], buf2[256];
  //  char buf[8192],buf2[256];
  char *Word[256],*lastn;
  const char *downfile;
  char fStop;
  int jchan;
  unsigned char sndvalue;
  fpos_t ftdi_pos, fsmask_pos;
  unsigned char send_tmp;//, rcv_tmp;
  int i,j,Count,nbytes,nbits,nframes,step_mode,pause;
  int hdrbits = 0, tdrbits = 0, hirbits = 0, tirbits = 0;
  int hdrbytes = 0, tdrbytes = 0, hirbytes = 0, tirbytes = 0; 
  int nowrit, cmpflag, errcntr;
  static int count;
  // MvdM struct JTAG_BBitStruct   driver_data;
  // int jtag_chain[4] = {1, 0, 5, 4};
  int jtag_chain_tmb[6] = {7, 6, 9, 8, 3, 1};
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
  
  total_packages = 0 ;
  send_packages = 0 ;
  jchan = *jch;
  downfile = fn;
  errcntr = 0;
  if (downfile==NULL)    downfile="default.svf";
  
  dwnfp    = fopen(downfile,"r");
  while (fgets(buf,256,dwnfp) != NULL) 
    {
      memcpy(buf,buf,256);
      Parse(buf, &Count, &(Word[0]));
      if( strcmp(Word[0],"SDR")==0 ) total_packages++ ;
    }
  fclose(dwnfp) ;
  
  printf("=== Programming Design with %s through JTAG chain %d\n",downfile, jchan);  
  printf("=== Have to send %d DATA packages \n",total_packages) ;
  dwnfp    = fopen(downfile,"r");
  
  if (dwnfp == NULL)
    {
      perror(downfile);
      
      return -1;
    }
  
  tmb_->start(jtag_chain_tmb[jchan-1]); 
  
  count=0; 
  nowrit=1;
  step_mode=0;
  while (fgets(buf,256,dwnfp) != NULL)  
    {
      if((buf[0]=='/'&&buf[1]=='/')||buf[0]=='!')
	{
	  if (db>4)          printf("%s",buf);
	}
      else 
	{
	  if(strrchr(buf,';')==0)
	    {
	      lastn=strrchr(buf,'\r');
	      if(lastn!=0)lastn[0]='\0';
	      lastn=strrchr(buf,'\n');
	      if(lastn!=0)lastn[0]='\0';
	      memcpy(buf2,buf,256);
	      Parse(buf2, &Count, &(Word[0]));
	      if(( strcmp(Word[0],"SDR")==0) || (strcmp(Word[0],"SIR")==0))
		{
		  sscanf(Word[1],"%d",&nbits);
		  if (nbits>MAXBUFSIZE) // === Handle Big Bitstreams
		    {
		      (*MyOutput_) << "EMUjtag. nbits larger than buffer size" << std::endl;
		    }
		  else do  // == Handle Normal Bitstreams
		    {
		      lastn=strrchr(buf,'\r');
		      if(lastn!=0)lastn[0]='\0';
		      lastn=strrchr(buf,'\n');
		      if(lastn!=0)lastn[0]='\0';
		      if (fgets(buf2,256,dwnfp) != NULL)
			{
			  strcat(buf,buf2);
			}
		      else 
			{
			  if (db)              printf("End of File encountered.  Quiting\n");
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
	    for(i=0;i<MAXBUFSIZE;i++)
	    {
	    snd[i]=0;
	    cmpbuf[i]=0;
	    sndbuf[i]=0;
	    rcvbuf[i]=0;
	    }
	  */
	  Parse(buf, &Count, &(Word[0]));
	  count=count+1;
	  cmpflag=0;
	  // ==================  Parsing commands from SVF file ====================
	  // === Handling HDR ===
	  if(strcmp(Word[0],"HDR")==0)
	    {
	      sscanf(Word[1],"%d",&hdrbits);
	      hdrbytes=(hdrbits)?(hdrbits-1)/8+1:0;
	    if (db)	  
	      printf("Sending %d bits of Header Data\n", hdrbits);
	    // if (db>3)          printf("HDR: Num of bits - %d, num of bytes - %d\n",hdrbits,hdrbytes);
	    for(i=2;i<Count;i+=2)
	      {
		if(strcmp(Word[i],"TDI")==0)
		  {
		    for(j=0;j<hdrbytes;j++)
		      {
			sscanf(&Word[i+1][2*(hdrbytes-j-1)+1],"%2X",(int *)&sndhdr[j]);
			// printf("%2X",sndhdr[j]);
		      }
		    // printf("\n%d\n",nbytes);
    		  }
      		if(strcmp(Word[i],"SMASK")==0)
		  {
		    for(j=0;j<hdrbytes;j++)
		      {
      		  	sscanf(&Word[i+1][2*(hdrbytes-j-1)+1],"%2X",(int *)&hdrsmask[j]);
		      }
		  }
     	 	if(strcmp(Word[i],"TDO")==0)
		  {
		    //if (db>2)             cmpflag=1;
		    cmpflag=1;
		    for(j=0;j<hdrbytes;j++)
		      {
			sscanf(&Word[i+1][2*(hdrbytes-j-1)+1],"%2X",(int *)&expect[j]);
		      }
		  }
      		if(strcmp(Word[i],"MASK")==0)
		  {
		    for(j=0;j<hdrbytes;j++)
		      {
			sscanf(&Word[i+1][2*(hdrbytes-j-1)+1],"%2X",(int *)&rmask[j]);
		      }
		  }
	      }
	    }
	  
	  // === Handling HIR ===
	  else if(strcmp(Word[0],"HIR")==0)
	    {
	      // for(i=0;i<3;i++)sndbuf[i]=tdi_pre_sdr[i];
	      // cmpflag=1;    //disable the comparison for no TDO SDR
	      sscanf(Word[1],"%d",&hirbits);
	      hirbytes=(hirbits)?(hirbits-1)/8+1:0;
	      if (db)	  
		printf("Sending %d bits of Header Data\n", hirbits);
	      // if (db>3)          printf("HIR: Num of bits - %d, num of bytes - %d\n",hirbits,hirbytes);
	      for(i=2;i<Count;i+=2)
		{
		  if(strcmp(Word[i],"TDI")==0)
		  {
		    for(j=0;j<hirbytes;j++)
		      {
			sscanf(&Word[i+1][2*(hirbytes-j-1)+1],"%2X",(int *)&sndhir[j]);
			// printf("%2X",sndhir[j]);
		      }
		    // printf("\n%d\n",nbytes);
    		  }
		  if(strcmp(Word[i],"SMASK")==0)
		    {
		      for(j=0;j<hirbytes;j++)
		      {
      		  	sscanf(&Word[i+1][2*(hirbytes-j-1)+1],"%2X",(int *)&hirsmask[j]);
		      }
		    }
		  if(strcmp(Word[i],"TDO")==0)
		    {
		      //if (db>2)             cmpflag=1;
		      cmpflag=1;
		      for(j=0;j<hirbytes;j++)
			{
			  sscanf(&Word[i+1][2*(hirbytes-j-1)+1],"%2X",(int *)&expect[j]);
			}
		    }
		  if(strcmp(Word[i],"MASK")==0)
		    {
		      for(j=0;j<hirbytes;j++)
			{
			  sscanf(&Word[i+1][2*(hirbytes-j-1)+1],"%2X",(int *)&rmask[j]);
			}
		    }
		}
	    }	
	  
	  // === Handling TDR ===
	  else if(strcmp(Word[0],"TDR")==0)
	    {
	      // for(i=0;i<3;i++)sndbuf[i]=tdi_pre_sdr[i];
	      // cmpflag=1;    //disable the comparison for no TDO SDR
	      sscanf(Word[1],"%d",&tdrbits);
	      tdrbytes=(tdrbits)?(tdrbits-1)/8+1:0;
	      if (db)	  
		printf("Sending %d bits of Tailer Data\n", tdrbits);
	      // if (db>3)          printf("TDR: Num of bits - %d, num of bytes - %d\n",tdrbits,tdrbytes);
	      for(i=2;i<Count;i+=2)
	      {
		if(strcmp(Word[i],"TDI")==0)
		  {
		    for(j=0;j<tdrbytes;j++)
		      {
			sscanf(&Word[i+1][2*(tdrbytes-j-1)+1],"%2X",(int *)&sndtdr[j]);
			// printf("%2X",sndhir[j]);
		      }
		    // printf("\n%d\n",nbytes);
    		  }
      		if(strcmp(Word[i],"SMASK")==0)
		  {
		    for(j=0;j<tdrbytes;j++)
		      {
			sscanf(&Word[i+1][2*(tdrbytes-j-1)+1],"%2X",(int *)&tdrsmask[j]);
		      }
		  }
		if(strcmp(Word[i],"TDO")==0)
		  {
		    //if (db>2)             cmpflag=1;
		    cmpflag=1;
		    for(j=0;j<tdrbytes;j++)
		      {
			sscanf(&Word[i+1][2*(tdrbytes-j-1)+1],"%2X",(int *)&expect[j]);
		      }
		  }
      		if(strcmp(Word[i],"MASK")==0)
		  {
		    for(j=0;j<tdrbytes;j++)
		      {
			sscanf(&Word[i+1][2*(tdrbytes-j-1)+1],"%2X",(int *)&rmask[j]);
		      }	
		  }
	      }
	    }
	  
	  // === Handling TDR ===
	  else if(strcmp(Word[0],"TIR")==0)
	  {
	    // for(i=0;i<3;i++)sndbuf[i]=tdi_pre_sdr[i];
	    // cmpflag=1;    //disable the comparison for no TDO SDR
	    sscanf(Word[1],"%d",&tirbits);
	    tirbytes=(tirbits)?(tirbits-1)/8+1:0;
	    if (db)	  
	      printf("Sending %d bits of Tailer Data\n", tdrbits);
	    // if (db>3)          printf("TIR: Num of bits - %d, num of bytes - %d\n",tirbits,tirbytes);
	    for(i=2;i<Count;i+=2)
	      {
		if(strcmp(Word[i],"TDI")==0)
		  {
		    for(j=0;j<tirbytes;j++)
		      {
			sscanf(&Word[i+1][2*(tirbytes-j-1)+1],"%2X",(int *)&sndtir[j]);
			    // printf("%2X",sndhir[j]);
		      }
		    // printf("\n%d\n",nbytes);
    		  }
      		if(strcmp(Word[i],"SMASK")==0)
		  {
		    for(j=0;j<tirbytes;j++)
		      {
				  sscanf(&Word[i+1][2*(tirbytes-j-1)+1],"%2X",(int *)&tirsmask[j]);
		      }
		  }
		if(strcmp(Word[i],"TDO")==0)
		  {
		    //if (db>2)             cmpflag=1;
		    cmpflag=1;
		    for(j=0;j<tirbytes;j++)
		      {
			sscanf(&Word[i+1][2*(tirbytes-j-1)+1],"%2X",(int *)&expect[j]);
		      }
		  }
      		if(strcmp(Word[i],"MASK")==0)
		  {
		    for(j=0;j<tirbytes;j++)
		      {
			sscanf(&Word[i+1][2*(tirbytes-j-1)+1],"%2X",(int *)&rmask[j]);
		      }
		  }
	      }
	  }
	  // === Handling SDR ===
	  else if(strcmp(Word[0],"SDR")==0)
	    {
	      for(i=0;i<3;i++)sndbuf[i]=tdi_pre_sdr[i];
	      // cmpflag=1;    //disable the comparison for no TDO SDR
	    sscanf(Word[1],"%d",&nbits);
	    nbytes=(nbits)?(nbits-1)/8+1:0;
	    if (db)	  printf("Sending %d bits Data\n", nbits);
	    // if (db>3)          printf("SDR: Num of bits - %d, num of bytes - %d\n",nbits,nbytes);
	    for(i=2;i<Count;i+=2)
	      {
	      if(strcmp(Word[i],"TDI")==0)
		{
		  for(j=0;j<nbytes;j++)
		    {
		      sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&snd[j]);
		      //                printf("%2X",snd[j]);
		    }
		  //                printf("\n%d\n",nbytes);
		}
	      if(strcmp(Word[i],"SMASK")==0)
		{
		  for(j=0;j<nbytes;j++)
		    {
		      sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&smask[j]);
		    }
		}
	      if(strcmp(Word[i],"TDO")==0)
		{
		  //if (db>2)             cmpflag=1;
		  cmpflag=1;
		  for(j=0;j<nbytes;j++)
		    {
		      sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&expect[j]);
		    }
		}
	      if(strcmp(Word[i],"MASK")==0)
		{
		  for(j=0;j<nbytes;j++)
		    {
		      sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&rmask[j]);
		    }
		}
	      }
	    for(i=0;i<nbytes;i++)
	      {
	      send_tmp = snd[i]&smask[i];
	      for(j=0;j<8;j++)
		{
		  if ((i*8+j)< nbits) 
		    { 
		      sndbuf[i*8+j+3]=send_tmp&0x01; 
		    }
		  send_tmp = send_tmp >> 1;
		}
	      }
	    for(i=0;i<4;i++)sndbuf[nbits+3]=tdi_post[i];         
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
	    
	    if (db>6) {	printf("SDR Send Data:\n");
	    for (i=0; i< ((hdrbits+nbits+tdrbits-1)/8+1); i++)
	    printf("%02X",realsnd[i]);
	    printf("\n");
	    }
	    send_packages++ ;
	    printf("%c[0m", '\033');
	    printf("%c[1m", '\033');
	    //printf("%c[2;50H", '\033');
	    if ( send_packages == 1 )   {
	      printf("%c7", '\033');
	    }
	  printf("%c8", '\033'); 
	  printf(" Sending %d/%d ",send_packages,total_packages) ;
	  printf("%c8", '\033'); 
	  printf("%c[0m", '\033');
	  if ( send_packages == total_packages ) printf("\n") ;
	  //
	  tmb_->scan(DATA_REG, (char*)realsnd, hdrbits+nbits+tdrbits, (char*)rcv, 2); 
	  //
	  if (cmpflag==1)
	    {     
	      /*
		for(i=0;i<nbytes;i++)
		{
		rcv_tmp = 0;
		for(j=0;j<8;j++)
		{
		if ((i*8+j) < nbits)
		{
		rcv_tmp |= ((rcvbuf[i*8+j+3]<<7) &0x80); 
		}
		rcv_tmp = rcv_tmp >> (j<7);
		
		}	
		rcv[nbytes-1-i] = rcv_tmp;		
		}
	      */
	      if (db>4){	  	printf("SDR Readback Data:\n");
	      //for(i=0;i<nbytes;i++) printf("%02X",rcv[i]);
	      for (i=0; i< ((hdrbits+nbits+tdrbits-1)/8+1); i++) 
		printf("%02X",rcv[i]);
	      printf("\n");
	      }	
	      
	      for(i=0;i<nbytes;i++)
		{
		  rcvword = rcv[i+(hdrbits/8)]+(((int)rcv[i+1+(hdrbits/8)])<<8);
		  rcvword = rcvword>>(hdrbits%8);
		  rcvword = rcv[i];
		  // if (((rcv[nbytes-1-i]^expect[i]) & (rmask[i]))!=0 && cmpflag==1)
		  if ((((rcvword&0xFF)^expect[i]) & (rmask[i]))!=0 && cmpflag==1)
		    {
		      printf("1.read back wrong, at i %02d  rdbk %02X  expect %02X  rmask %02X\n",i,rcv[i]&0xFF,expect[i]&0xFF,rmask[i]&0xFF);
		      errcntr++;
		    }
		}	
	    }
	  /*         if (cmpflag==1)
		     {
		     for(i=3;i<nbits+3;i++) printf("%d",rcvbuf[i]);
		     printf("\n");
		     }
	  */       
          }
        // === Handling SIR ===
        else if(strcmp(Word[0],"SIR")==0)
          {
	    for(i=0;i<4;i++)sndbuf[i]=tdi_pre_sir[i];
	    // cmpflag=1;    //disable the comparison for no TDO SDR
	    sscanf(Word[1],"%d",&nbits);
	    nbytes=(nbits)?(nbits-1)/8+1:0;
	    if (db)	  printf("Sending %d bits of Command\n",nbits);
	    // if (db>3)          printf("SIR: Num of bits - %d, num of bytes - %d\n",nbits,nbytes);
	    for(i=2;i<Count;i+=2)
	      {
		if(strcmp(Word[i],"TDI")==0)
		  {
		    for(j=0;j<nbytes;j++)
		      {
			sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&snd[j]);
		      }
		  }
		if(strcmp(Word[i],"SMASK")==0)
		  {
		    for(j=0;j<nbytes;j++)
		      {
			sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&smask[j]);
		      }
		  }
		if(strcmp(Word[i],"TDO")==0)
		  {
			cmpflag=1;
			// if (db>2)              cmpflag=1;
			for(j=0;j<nbytes;j++)
			  {
			    sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&expect[j]);
			  }
		  }
		if(strcmp(Word[i],"MASK")==0)
		  {
		    for(j=0;j<nbytes;j++)
		      {
			sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&rmask[j]);
		      }
		  }
	      }
	    for(i=0;i<nbytes;i++)
	      {
		send_tmp = snd[i]&smask[i];
		// printf("\n%d - ", send_tmp);
		for(j=0;j<8;j++)
		  {
		    if ((i*8+j)< nbits) 
		      {
			sndbuf[i*8+j+4]=send_tmp&0x01;
			// printf("%d", sndbuf[i*8+j+4]);
		      }
		    send_tmp = send_tmp >> 1;
		  }
	      }
	    for(i=0;i<4;i++)sndbuf[nbits+4]=tdi_post[i];
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
	    tmb_->scan(INSTR_REG, (char*)realsnd, hirbits+nbits+tirbits, (char*)rcv, 0); 
	    //

	    if (db>6) { 	printf("SIR Send Data:\n");
	    for (i=0; i< ((hirbits+nbits+tirbits-1)/8+1);  i++)
	      printf("%02X",realsnd[i]);
	    printf("\n");
	    }
	    //	  jtag_load_(&fd, &nframes, tms_pre_sir, sndbuf, rcvbuf, &step_mode);
	    if (cmpflag==1)
	      {
		/*               for(i=0;i<nbytes;i++)
				 {
				 rcv_tmp = 0;
				 for(j=0;j<8;j++)
				 {
				 if ((i*8+j) < nbits)
				 {
				 rcv_tmp |= ((rcvbuf[i*8+j+4]<<7) &0x80);
				 }
				 rcv_tmp = rcv_tmp >> (j<7);
				 
				 }
				 rcv[nbytes-1-i] = rcv_tmp;
				 }
		*/
		if (db>4){                printf("SIR Readback Data:\n");
                //for(i=0;i<nbytes;i++) printf("%02X",rcv[i]);
		for (i=0; i< ((hdrbits+nbits+tdrbits-1)/8+1); i++) printf("%02X",rcv[i]);
                printf("\n");
		}
		
                for(i=0;i<nbytes;i++)
		  {
		    rcvword = rcv[i+(hirbits/8)]+(((int)rcv[i+1+(hirbits/8)])<<8);
		    rcvword = rcvword>>(hirbits%8);
		    rcvword = rcv[i];
		    // if (((rcv[nbytes-1-i]^expect[i]) & (rmask[i]))!=0 && cmpflag==1)
		    if ((((rcvword&0xFF)^expect[i]) & (rmask[i]))!=0 && cmpflag==1)
		      {
			printf("2.read back wrong, at i %02d  rdbk %02X  expect %02X  rmask %02X\n",i,rcv[i]&0xFF,expect[i]&0xFF,rmask[i]&0xFF);
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
          }
	// === Handling RUNTEST ===
	  else if(strcmp(Word[0],"RUNTEST")==0)
	    {
	      sscanf(Word[1],"%d",&pause);
	      //printf("RUNTEST:  %d\n",pause);
	      usleep(pause+100);
	      // InsertDelayJTAG(pause,MYMICROSECONDS);
	    }
	  // === Handling STATE ===
	  else if((strcmp(Word[0],"STATE")==0)&&(strcmp(Word[1],"RESET")==0)&&(strcmp(Word[2],"IDLE;")==0))
	    {
	      //          printf("STATE: goto reset idle state\n");
	      // Put send STATE RESET here
          }
	  // === Handling TRST ===
	  else if(strcmp(Word[0],"TRST")==0)
	    {
	      //          printf("TRST\n");
          }
	// === Handling ENDIR ===
        else if(strcmp(Word[0],"ENDIR")==0)
	  {
	    //          printf("ENDIR\n");
          }
	  // === Handling ENDDR ===
	  else if(strcmp(Word[0],"ENDDR")==0)
	  {
	    //	   printf("ENDDR\n");
          }
	}
      }
  tmb_->endDevice();
  fclose(dwnfp);
  return errcntr; 
}


// ====
// SVF File Parser module
// ====
void EMUjtag::Parse(char *buf,int *Count,char **Word)
{

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


