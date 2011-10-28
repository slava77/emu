// $Id: DDU.h,v 1.1 2011/10/28 18:13:37 liu Exp $
// $Log: DDU.h,v $
// Revision 1.1  2011/10/28 18:13:37  liu
// add DDU class
//

/* names of the variables and functions are directly copied from FEDCore */

#ifndef DDU_h
#define DDU_h
#include "emu/pc/VMEModule.h"
#include "emu/pc/EmuLogger.h"
#include <string>

namespace emu {
  namespace pc {

class Crate;

class DDU : public VMEModule, public EmuLogger 
{
  public:

    DDU(Crate * , int slot);
    ~DDU();

    unsigned int boardType() const {return VMEModule::DDU_ENUM;}
      
    bool SelfTest();
    void init();
    void configure();

    int readFMMBusy();
    int readFMMFullWarning();
    int readFMMLostSync();
    int readFMMError();
    int readCSCStatus();
    int readWarningHistory();
    int readBusyHistory();
    int readSwitches();
    int readParallelStatus();
    int readGbEPrescale();
    int readFakeL1();
    int readFMM();
    int readInputRegister(int iReg);
    int readTestRegister(int iReg);
    void writeFMM(int value);
    void writeFakeL1(int value);
    void writeGbEPrescale(int value);
    void writeInputRegister(int value);
    int readSerialStatus();
    int readFlashKillFiber();
    int readFlashBoardID();
    int readFlashRUI();
    void writeFlashKillFiber(int value);
    void writeFlashBoardID(int value);
    void writeFlashRUI(const int &value);
    void disableFMM();
    void enableFMM();
    int readRealFMM();
    std::vector<float> readTempsVolts();

    unsigned readUserIDCode(int dev, int code, int cnt);
    unsigned CtrlFpgaUserCode();
    unsigned CtrlFpgaIdCode();
    unsigned VmePromUserCode();
    unsigned VmePromIdCode();
    
    
  protected:
    
    inline unsigned vme_para(int dev, int cmd=0) { return (3<<16) + ((dev&0xF)<<12) + ((cmd&0xFF)<<2); }
    
    inline unsigned vme_seri(int dev, int cmd=0) { return (4<<16) + ((dev&0xF)<<12) + ((cmd&0xF)<<2); }
    
    void writeRegister(unsigned reg, int value);
              
    int readRegister(unsigned reg);
              
};
    
  }
}
#endif
