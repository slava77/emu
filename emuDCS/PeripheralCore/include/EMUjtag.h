#ifndef EMUjtag_h
#define EMUjtag_h

#include <string>
#include <fstream>
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
  void setup_jtag(int JTAGchain);
  //
  void ShfIR_ShfDR(const int selected_chip,
		   const int opcode,
		   const int length_of_register,
		   const int * tdi_to_shift_in);  //1 bit per index
  //
  void ShfIR_ShfDR(const int selected_chip,
		   const int opcode,
		   const int length_of_register); //drop tdi argument for read-only registers
  //
  inline int * GetDRtdo() { return shfDR_tdo_ ; }
  inline int GetRegLength() { return register_length_ ; }
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
			int firstBit,          // first bit to begin (i.e., bitVector[0] = (buffer>>firstBit)
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
  void SetXsvfFilename(std::string filename);       //set the base filename by which the rest of the filenames are referred to
  //
  //------------------------------------------------
  // prom image 
  //------------------------------------------------
  void CreateUserPromFile();                      //create Prom Image File with configuration data
  void ReadUserPromFile();                        //read Prom Image File from disk
  //
  int GetUserPromImage(int address);              //address=[0 - (TOTAL_NUMBER_OF_ADDRESSES-1)]
  //------------------------------------------------
  // XSVF image
  //------------------------------------------------
  void CreateXsvfFile();                      //creates XSVF file from Prom Image File
  //
  void ReadXsvfFile(bool create_logfile);     //read XSVF file from disk
  void ReadXsvfFile();                        //no argument given => no logfile created
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
  int jtag_chain_;
  int devices_in_chain_;
  //
  //
  /////////////////////////////////
  // ShfIR_ShfDR private values:
  /////////////////////////////////
  int bits_in_opcode_[MAX_NUM_DEVICES];
  int chip_id_;
  int register_length_;
  int shfDR_tdo_[MAX_NUM_FRAMES];
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
  void InsertBlockBoundaries_(int * data_to_go_into_prom, 
			      int number_of_data_words_to_go_into_prom);
  void SetUserPromImage_(int address, int value);                 
  void WritePromDataToDisk_();
  //
  bool prom_file_ok_;
  //
  //-------------------
  // xsvf file stuff
  //-------------------
  int read_xsvf_image_[MAX_XSVF_IMAGE_NUMBER];
  char write_xsvf_image_[MAX_XSVF_IMAGE_NUMBER];
  int number_of_write_bytes_;
  int number_of_read_bytes_;
  //
  void SetWriteXsvfImage_(int address,int value);
  void WriteXsvfDataToDisk_();
  //
  int  GetReadXsvfImage_(int address);
  //
  void DecodeXsvfImage_();
  int image_counter_;
  //
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
  int xdr_length_;
  int tdomask_[MAX_BYTES_TDO];
  int tdoexpected_[MAX_BYTES_TDO];
  int tdivalue_[MAX_BYTES_TDI];
  //
  //
};

#endif

