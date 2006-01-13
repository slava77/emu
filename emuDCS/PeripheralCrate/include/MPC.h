
#ifndef OSUcc

//-----------------------------------------------------------------------
// $Id: MPC.h,v 2.13 2006/01/13 10:07:00 mey Exp $
// $Log: MPC.h,v $
// Revision 2.13  2006/01/13 10:07:00  mey
// Got rid of virtuals
//
// Revision 2.12  2006/01/12 23:43:53  mey
// Update
//
// Revision 2.11  2006/01/12 22:35:50  mey
// UPdate
//
// Revision 2.10  2006/01/12 12:27:56  mey
// UPdate
//
// Revision 2.9  2006/01/11 13:26:37  mey
// Update
//
// Revision 2.8  2005/12/10 04:47:21  mey
// Fix bug
//
// Revision 2.7  2005/12/08 12:00:16  mey
// Update
//
// Revision 2.3  2005/08/22 07:55:44  mey
// New TMB MPC injector routines and improved ALCTTiming
//
// Revision 2.2  2005/08/17 12:27:22  mey
// Updated FindWinner routine. Using FIFOs now
//
// Revision 2.1  2005/06/06 19:22:35  geurts
// additions for MPC/SP connectivity tests (Dan Holmes)
//
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#ifndef MPC_h
#define MPC_h
#include "VMEModule.h"
#include <string>

class MPCParser;

class MPC : public VMEModule {
 public:
  friend class MPCParser;


  MPC(int newCrate, int slot);
  ~MPC();
  void firmwareVersion();

  /// overrides VMEModule.  I don't understand what it does.
  void start();
  void end() {};

  /// from the BOARDTYPE enum
  unsigned int boardType() const {return MPC_ENUM;} 
  //
  bool SelfTest();
  void configure();
  void init();
  //
  inline std::vector<unsigned long int> GetFIFOBLct0() { return FIFOBLct0 ; }
  inline std::vector<unsigned long int> GetFIFOBLct1() { return FIFOBLct1 ; }
  inline std::vector<unsigned long int> GetFIFOBLct2() { return FIFOBLct2 ; }
  //
  inline void ResetFIFOBLct() {
    FIFOBLct0.clear();
    FIFOBLct1.clear();
    FIFOBLct2.clear();
  }
  //
  inline void RedirectOutput(std::ostream * Output) { }
  void WriteRegister(int,int);
  int  ReadRegister(int);

  /// address is usually one of the above enums.  theBaseAddress
  /// defined in the constructor automatically added
  void read_fifo(unsigned long int address, char * data);

  void read_fifos();

  void read_fifosA();

  // dump_fifos just writes read_fifos output to file

  /// control status register
  void read_csr0();
 
  //fg void read_date();

  void read_status();

  void setTLK2501TxMode(int);

  void SoftReset() ;

  /// used by GUIs
  void executeCommand(std::string command);

  void enablePRBS();
  void disablePRBS();

  void initTestLinks(); // initialise the Links for injectSP() funct below:DH.
  void injectSP();      // injects some test patterns into fifos :dan Holmes
  void injectSP(char*); // as above you give me file with data in.:dan Holmes

  void setTransparentMode();
  void setTransparentMode(unsigned int);
  void setSorterMode();
  void setDelayFromTMB(unsigned char);

  /// MPC-SP02 interconnect validation procedure
  void interconnectTest();

 protected:
  /// MPC base address should always correspond to VME Slot 12 (=0x600000)
  static const unsigned long int theBaseAddress=0x00600000;
  enum VMEAddresses {
    /// FIFOs Ax[a,b] correspond to TMB x, a=[15-0] and b=[31-16]
    FIFO_A1a = 0x80, FIFO_A1b = 0x82, FIFO_A2a = 0x84, FIFO_A2b = 0x86, 
    FIFO_A3a = 0x88, FIFO_A3b = 0x8A, FIFO_A4a = 0x8C, FIFO_A4b = 0x8E, 
    FIFO_A5a = 0x90, FIFO_A5b = 0x92, FIFO_A6a = 0x94, FIFO_A6b = 0x96, 
    FIFO_A7a = 0x98, FIFO_A7b = 0x9A, FIFO_A8a = 0x9C, FIFO_A8b = 0x9E, 
    FIFO_A9a = 0xA0, FIFO_A9b = 0xA2, 
    /// FIFOs Bx correspond to the x-th best selected LCT
    FIFO_B1  = 0xA4, FIFO_B2  = 0xA6, FIFO_B3  = 0xA8,
    /// Various Control & Status registers (CSR3 read only)
    CSR0 = 0x00, CSR1 = 0xAA, CSR2 = 0xAC,
    CSR3 = 0xAE, CSR4 = 0xB8, CSR5 = 0xBA,
    /// L1-accept counter (read only)
    L1ACC = 0xB0,
    /// Transmit 511 words from all FIFO_A in test-mode (write only)
    TXMIT511 = 0xB2,
    /// Send TxEn "0" pulse to all three TLK2501 transmitters (write only)
    TXENALL = 0xB6
    // DATE     = 0xAA, STATUS   = 0xAE
  };
  enum CSR0options {
    CSR0_FPGATestMode = 0x01, CSR0_FPGAResetLogic = 0x02,
    CSR0_FPGAHardReset = 0x04,
    CSR0_TDI = 0x08, CSR0_TMS = 0x10, CSR0_TCK = 0x20, CSR0_TDO = 0x40,
    CSR0_TXEN = 0x80, CSR0_MASKCOMP = 0x100
  };

  enum MPCMODE {READ, WRITE};


 private:
  //
  std::vector<unsigned long int> FIFOBLct0;
  std::vector<unsigned long int> FIFOBLct1;
  std::vector<unsigned long int> FIFOBLct2;
  //
  int TLK2501TxMode_;
  int TransparentModeSources_;
  int TMBDelayPattern_;
  int read(int btd,char *buf_p,
    unsigned long int add,
    int xfer_len,
    int *xfer_done_p);

  int write(int btd,char *buf_p,
    unsigned long int add,
    int xfer_len,
    int *xfer_done_p);

  // helper routine for the above two functions
  void writeToAddress(unsigned long int addr,
     char * bufp, int xfer_len);

  // only used so start routine can do different things.
  MPCMODE theMode;
};

#endif



#else

//-----------------------------------------------------------------------
// $Id: MPC.h,v 2.13 2006/01/13 10:07:00 mey Exp $
// $Log: MPC.h,v $
// Revision 2.13  2006/01/13 10:07:00  mey
// Got rid of virtuals
//
// Revision 2.12  2006/01/12 23:43:53  mey
// Update
//
// Revision 2.11  2006/01/12 22:35:50  mey
// UPdate
//
// Revision 2.10  2006/01/12 12:27:56  mey
// UPdate
//
// Revision 2.9  2006/01/11 13:26:37  mey
// Update
//
// Revision 2.8  2005/12/10 04:47:21  mey
// Fix bug
//
// Revision 2.7  2005/12/08 12:00:16  mey
// Update
//
// Revision 2.6  2005/11/25 23:42:02  mey
// Update
//
// Revision 2.5  2005/11/22 15:14:39  mey
// Update
//
// Revision 2.4  2005/11/21 17:38:45  mey
// Update
//
// Revision 2.3  2005/08/22 07:55:44  mey
// New TMB MPC injector routines and improved ALCTTiming
//
// Revision 2.2  2005/08/17 12:27:22  mey
// Updated FindWinner routine. Using FIFOs now
//
// Revision 2.1  2005/06/06 19:22:35  geurts
// additions for MPC/SP connectivity tests (Dan Holmes)
//
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#ifndef MPC_h
#define MPC_h
#include "VMEModule.h"
#include <string>

class MPCParser;

class MPC : public VMEModule {
 public:
  friend class MPCParser;


  MPC(int newCrate, int slot);
  ~MPC();
  void firmwareVersion();

  /// from the BOARDTYPE enum
  unsigned int boardType() const {return MPC_ENUM;} 
  //
  bool SelfTest();
  void configure();  
  void init();

  /// address is usually one of the above enums.  theBaseAddress
  /// defined in the constructor automatically added
  void read_fifo(char address, char * data);

  void read_fifos();
  inline void RedirectOutput(std::ostream * Output) { MyOutput_ = Output ; }
  void read_fifosA();

  // dump_fifos just writes read_fifos output to file

  /// control status register
  void read_csr0();
 
  //fg void read_date();

  void read_status();

  void setTLK2501TxMode(int);

  void SoftReset() ;

  /// used by GUIs
  void executeCommand(std::string command);

  void WriteRegister(int,int);
  int  ReadRegister(int);

  void enablePRBS();
  void disablePRBS();

  void initTestLinks(); // initialise the Links for injectSP() funct below:DH.
  void injectSP();      // injects some test patterns into fifos :dan Holmes
  void injectSP(char*); // as above you give me file with data in.:dan Holmes

  void setTransparentMode();
  void setTransparentMode(unsigned int);
  void setSorterMode();
  void setDelayFromTMB(unsigned char);

  /// MPC-SP02 interconnect validation procedure
  void interconnectTest();

 protected:
  /// MPC base address should always correspond to VME Slot 12 (=0x600000)
//  static const unsigned long int theBaseAddress=0x00600000;
  enum VMEAddresses {
    /// FIFOs Ax[a,b] correspond to TMB x, a=[15-0] and b=[31-16]
    FIFO_A1a = 0x80, FIFO_A1b = 0x82, FIFO_A2a = 0x84, FIFO_A2b = 0x86, 
    FIFO_A3a = 0x88, FIFO_A3b = 0x8A, FIFO_A4a = 0x8C, FIFO_A4b = 0x8E, 
    FIFO_A5a = 0x90, FIFO_A5b = 0x92, FIFO_A6a = 0x94, FIFO_A6b = 0x96, 
    FIFO_A7a = 0x98, FIFO_A7b = 0x9A, FIFO_A8a = 0x9C, FIFO_A8b = 0x9E, 
    FIFO_A9a = 0xA0, FIFO_A9b = 0xA2, 
    /// FIFOs Bx correspond to the x-th best selected LCT
    FIFO_B1  = 0xA4, FIFO_B2  = 0xA6, FIFO_B3  = 0xA8,
    /// Various Control & Status registers (CSR3 read only)
    CSR0 = 0x00, CSR1 = 0xAA, CSR2 = 0xAC,
    CSR3 = 0xAE, CSR4 = 0xB8, CSR5 = 0xBA,
    /// L1-accept counter (read only)
    L1ACC = 0xB0,
    /// Transmit 511 words from all FIFO_A in test-mode (write only)
    TXMIT511 = 0xB2,
    /// Send TxEn "0" pulse to all three TLK2501 transmitters (write only)
    TXENALL = 0xB6
    // DATE     = 0xAA, STATUS   = 0xAE
  };
  enum CSR0options {
    CSR0_FPGATestMode = 0x01, CSR0_FPGAResetLogic = 0x02,
    CSR0_FPGAHardReset = 0x04,
    CSR0_TDI = 0x08, CSR0_TMS = 0x10, CSR0_TCK = 0x20, CSR0_TDO = 0x40,
    CSR0_TXEN = 0x80, CSR0_MASKCOMP = 0x100
  };

  enum MPCMODE {READ, WRITE};


 private:
  std::ostream * MyOutput_ ;
  int TLK2501TxMode_;
  int TransparentModeSources_;
  int TMBDelayPattern_;
  int read(int btd,char *buf_p,
    unsigned long int add,
    int xfer_len,
    int *xfer_done_p);

  int write(int btd,char *buf_p,
    unsigned long int add,
    int xfer_len,
    int *xfer_done_p);

  // helper routine for the above two functions
  void writeToAddress(unsigned long int addr,
     char * bufp, int xfer_len);

  // only used so start routine can do different things.
  MPCMODE theMode;
};

#endif

#endif
