#ifndef EMUjtag_h
#define EMUjtag_h

#include <string>
#include <fstream>
#include "EMU_JTAG_constants.h"

class TMB;
class EMUjtag 
{
public:
  // *****************************************************************************************
  // ** N.B. In this class, the data arrays are ordered such that the least significant bit **
  // ** (array index 0) is the first bit which is shifted into or out of the JTAG chain.    **
  // *****************************************************************************************
  //
  //EMUjtag();
  EMUjtag(TMB * );
  ~EMUjtag();
  //
  inline void RedirectOutput(std::ostream * Output) { MyOutput_ = Output ; }
  //
  /////////////////////////////////////////////////////////////////////////////
  // set up your JTAG stuff correctly:
  /////////////////////////////////////////////////////////////////////////////
  void setup_jtag(int JTAGchain);
  //
  //////////////////////////////////////////////////////////////////////////////////////////////
  // Use EMUjtag to shift data into Instruction Registers and into (and out of) Data Registers
  //////////////////////////////////////////////////////////////////////////////////////////////
  // See EMU_JTAG_constants.h for the values of
  //    - JTAGchain 
  //    - selected_chip
  //    - opcode
  //    - length_of_register
  // for following methods:
  //
  void ShfIR_ShfDR(const int selected_chip,
		   const int opcode,
		   const int length_of_register,
		   const int * tdi_to_shift_in);  //1 bit per index, see comment above for bit ordering
  //
  void ShfIR_ShfDR(const int selected_chip,
		   const int opcode,
		   const int length_of_register); //drop tdi argument for read-only registers
  //
  inline int * GetDRtdo() { return chip_tdo_in_bits_ ; }  //this is tdo only for "selected_chip"
  inline int GetRegLength() { return register_length_ ; } //this is the register length for the "opcode" of "selected chip"
  //
  //
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  // Check if JTAG read values = JTAG write values:
  //  ... recommended use:  after write command, call CompareBitByBit if GetCheckJtagWrite()==true
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  inline void SetCheckJtagWrite(bool check_write) { check_write_ = check_write; }   
  inline bool GetCheckJtagWrite() { return check_write_; }
  //
  void CompareBitByBit(int * write_vector,     // Compare the data in the first "length" bits
		       int * read_vector,      // of "write_vector[]" with "read_vector[]"
		       int length);           
  //
  //
  ///////////////////////////////////////////////////////////////
  // Useful functions to convert one type into another type
  ///////////////////////////////////////////////////////////////
  void packCharBuffer(int * vector_of_bits,    // pack a vector of bits into a vector of characters
		      int Nbits,               // number of bits to pack
		      char * vector_of_char); 
  //
  void unpackCharBuffer(char * vector_of_char, // turn a vector of characters into a vector of bits
			int length,            // length of vector of bits
			int firstBit,          // first bit to begin (i.e., vector_of_bits[0] = (vector_of_char>>firstBit)
			int * vector_of_bits);
  //
  int bits_to_int(int * vector_of_bits,        // convert a vector of bits into an integer
		  int length_of_vector,        // length of vector of bits
		  int start_from_MsbOrLsb);    // start_from = LSBfirst => LSB of return integer = vector_of_bits[0]
  //                                                         = MSBfirst => LSB of return integer = vector_of_bits[length_of_vector-1]
  //
  void int_to_bits(int value_to_expand,        // Turn an integer into a vector of bits
		   int number_of_bits,         // number of bits into which to expand integer
		   int * vector_of_bits,
		   int start_from_MsbOrLsb);   // start_from = LSBfirst => vector_of_bits[0] = LSB of "value_to_expand"
  //                                                         = MSBfirst => vector_of_bits[0] = MSB of "value_to_expand"
  //
  //
  ////////////////////////////////////////////////////////////////////////
  // XSVF programming:
  ////////////////////////////////////////////////////////////////////////
  void SetXsvfFilename(std::string filename);       //set the base filename for xsvf file handling
  //                                                  -> filename.dat  = prom image file in format AAAA DD (address, data)
  //                                                  -> filename.xsvf = binary file which gets shifted into prom with JTAG
  //                                                  -> filename.log  = ascii file which gets shifted into prom with JTAG
  //
  void SetWhichUserProm(int device);                //device = [ChipLocationTmbUserPromTMB, ChipLocationTmbUserPromALCT]
  int  GetWhichUserProm();
  //
  //--------------------------------------------------------------
  // prom image file handling -> configuration data for the prom
  //--------------------------------------------------------------
  void CreateUserPromFile();                        //create filename.dat
  bool ReadUserPromFile();                          //read filename.dat from disk
  //
  int GetUserPromImage(int address);                // address=[0 - (TOTAL_NUMBER_OF_ADDRESSES-1)]
  //
  //------------------------------------------------
  // XSVF file handling
  //------------------------------------------------
  void CreateXsvfFile();                    //creates filename.xsvf from filename.dat 
  //                                        // ----> IMPORTANT:  need to specify the prom with SetWhichUserProm(device)
  //
  void ReadXsvfFile(bool create_logfile);   //read filename.xsvf from disk
  void ReadXsvfFile();                      // ...no argument given => no logfile created
  //
  //------------------------------------------------
  // Program proms with XSVF file
  //------------------------------------------------  
  void ProgramUserProm();                             //program user proms with filename.xsvf 
  void ProgramTMBProms();                             //program TMB mezzanine proms with filename.xsvf 
  //
  void CheckUserProm();                               //compare program in SetWhichUserProm(device) with filename.dat 
  //
  inline int GetNumberOfVerifyErrors() { return verify_error_; }
  //
  //------------------------------------------------
  // Check operation of user proms after hard reset
  //------------------------------------------------  
  void CheckVMEStateMachine();
  void CheckJTAGStateMachine();
  //
  //
  ////////////////////////////////////////////////////////////////////////
  // SVF programming:
  ////////////////////////////////////////////////////////////////////////
  void Parse(char *buf,int *Count,char **Word);
  int  SVFLoad(int*, const char *, int);
  //
  //
protected:
  //
  //
private:
  //
  std::ostream * MyOutput_ ;
  TMB * tmb_ ;
  //
  bool debug_;
  bool check_write_;
  /////////////////////////////////
  // setup private values:
  /////////////////////////////////
  void SetWriteToDevice_(bool communicate);
  bool GetWriteToDevice_();
  bool write_to_device_;
  //
  int jtag_chain_;
  int devices_in_chain_;
  int max_xsvf_image_number_;
  //
  //
  /////////////////////////////////
  // ShfIR_ShfDR private values:
  /////////////////////////////////
  void ShfIR_(const int selected_chip, 
	      const int opcode);
  void ShfDR_(const int selected_chip, 
	      const int size_of_register, 
	      const int * write_data);
  //
  int bits_in_opcode_[MAX_NUM_DEVICES];
  int chip_id_;
  int register_length_;
  // As noted at the top of the header file, the following are ordered such that:
  //    - LSB of byte index = 0 = first bit which is shifted in/out
  //    - bit index = 0 = first bit which is shifted in/out
  int tdi_in_bits_[MAX_NUM_FRAMES];        // -> tdi for full chain (including bypass bits)
  int chip_tdo_in_bits_[MAX_NUM_FRAMES];   // -> tdo only for chip we are interested in
  char tdi_in_bytes_[MAX_BUFFER_SIZE];     // -> tdi for full chain (including bypass bits)
  char tdo_in_bytes_[MAX_BUFFER_SIZE];     // -> tdo for full chain (including bypass bits)
  char tdo_mask_in_bytes_[MAX_BUFFER_SIZE];
  //
  ////////////////////////////
  // XSVF private values:
  ////////////////////////////
  //----------------------
  // file stuff
  //----------------------
  std::string filename_dat_;
  std::string filename_xsvf_;
  std::string filename_log_;
  //
  std::string AddTrailer_(std::string filename,
			  std::string trailer);
  //
  std::ofstream Logfile_;
  //
  //-------------------
  // prom image stuff
  //-------------------
  int read_ascii_prom_image_[TOTAL_NUMBER_OF_ADDRESSES];
  int write_ascii_prom_image_[TOTAL_NUMBER_OF_ADDRESSES];
  //
  int data_word_count_;
  int prom_image_word_count_;
  int ComputeCheckSum_(int begin_address,     //compute the checksum for begin_address<=address<=end_address
		       int end_address);       
  //
  void InsertHeaderAndTrailer_(int * data_to_go_into_prom);
  void SetUserPromImage_(int address, int value);                 
  void WritePromDataToDisk_();
  //
  //
  //-------------------
  // write xsvf file 
  //-------------------
  int which_user_prom_;
  //
  int image_counter_;
  char write_xsvf_image_[MAX_XSVF_IMAGE_NUMBER];
  int number_of_write_bytes_;
  //
  bool CreateXsvfImage_();
  void WritePreambleIntoXsvfImage_();
  void WritePromImageIntoXsvfImage_();
  void WriteInterimBetweenImageAndVerifyIntoXsvfImage_();
  void WritePromImageIntoXsvfVerify_();
  void WritePostambleIntoXsvfImage_();
  void WriteXsvfImageToDisk_();
  //
  void SetWriteXsvfImage_(int address,int value);
  void WriteXCOMPLETE_();
  void WriteXTDOMASK_(int number_of_bits_in_mask,
		      int * vector_of_bits_for_mask);
  void WriteXSIR_(int opcode);
  int  SumOpcodeBits_();
  void WriteXRUNTEST_(int length_of_time);         //length of time = pause in microseconds
  void WriteXREPEAT_(int number_of_times);
  void WriteXSDRSIZE_(int number_of_bits_in_data_register);
  void WriteXSDRTDO_(int number_of_bits_in_data_register,
		     int * tdi_bits,
		     int * tdo_expected_bits);
  void WriteXSTATE_(int state);                    //state = [TLR, RTI]
  //
  //
  //-------------------
  // read xsvf file 
  //-------------------
  int read_xsvf_image_[MAX_XSVF_IMAGE_NUMBER];
  int number_of_read_bytes_;
  //
  void ReadXsvfFile_(bool create_logfile);
  //		     
  int  GetReadXsvfImage_(int address);
  //
  void DecodeXsvfImage_();
  void ParseXCOMPLETE_();
  void ParseXTDOMASK_();
  void ParseXSIR_();
  void ParseXRUNTEST_();
  void ParseXREPEAT_();
  void ParseXSDRSIZE_();
  void ParseXSDRTDO_();
  void ParseXSTATE_();
  //
  int NumberOfCommands_[NUMBER_OF_DIFFERENT_XSVF_COMMANDS];
  //
  //-------------------
  // program_prom 
  //-------------------
  int verify_error_;
  //
  int xdr_length_;
  int xruntest_time_;
  int xrepeat_times_;
  //
  //
};

#endif

