/*****************************************************************************\
* $Id: DDU.cc,v 3.1 2011/10/28 18:13:37 liu Exp $
\*****************************************************************************/
#include "emu/pc/DDU.h"

#include <cmath>
#include <iomanip>
#include <sstream>

namespace emu {
  namespace pc {

DDU::DDU(Crate* theCrate,  int slot): VMEModule(theCrate, slot),EmuLogger()
{
  std::cout << "DDU: in crate=" << this->crate()
            << " slot=" << this->slot() << std::endl;
}

DDU::~DDU()
{  
}

bool DDU::SelfTest()
{
  return true;
}

void DDU::init()
{
}

void DDU::configure()
{
}

void DDU::writeRegister(unsigned reg, int value)
{
  write_now(reg, (unsigned short) (value&0xFFFF), NULL);
}

int DDU::readRegister(unsigned reg)
{
  int value;
  read_now(reg, (char *)(&value));
  return value;
} 


///////////////////////////////////////////////////////////////////////////////
// Read VME Parallel
///////////////////////////////////////////////////////////////////////////////
int DDU::readFMMBusy()
{
	return readRegister(vme_para(0));
}

int DDU::readFMMFullWarning()
{
	return readRegister(vme_para(1));
}

int DDU::readFMMLostSync()
{
	return readRegister(vme_para(2));
}

int DDU::readFMMError()
{
	return readRegister(vme_para(3));
}

int DDU::readCSCStatus()
{
	return readRegister(vme_para(4));
}

int DDU::readWarningHistory()
{
	return readRegister(vme_para(5));
}

int DDU::readBusyHistory()
{
	return readRegister(vme_para(6));
}

int DDU::readSwitches()
{
	return readRegister(vme_para(14));
}

int DDU::readParallelStatus()
{
	return readRegister(vme_para(15));
}

int DDU::readGbEPrescale()
{
	return readRegister(vme_para(9, 0));
}

int DDU::readFakeL1()
{
	return readRegister(vme_para(9, 5));
}

int DDU::readFMM()
{
	return readRegister(vme_para(9, 0x0f));
}

int DDU::readInputRegister(int iReg)
{
	if(iReg<0 || iReg>2) return -1;
	return readRegister(vme_para(8, iReg));
}

int DDU::readTestRegister(int iReg)
{
	if(iReg<0 || iReg>4) return -1;
	return readRegister(vme_para(8, iReg+3));
}

///////////////////////////////////////////////////////////////////////////////
// Write VME Parallel
///////////////////////////////////////////////////////////////////////////////

void DDU::writeFMM(int value)
{
	writeRegister(vme_para(9, 0x8f), value);
}

void DDU::writeFakeL1(int value)
{
	int myValue = value & 0x7;
	int complement = 0xf - myValue;
	int loadMe = (complement << 12) | (myValue << 8) | (complement << 4) | myValue;
	writeRegister(vme_para(9, 0x85), loadMe);
}

void DDU::writeGbEPrescale(int value)
{
	int myValue = value & 0xf;
	int complement = 0xf - myValue;
	int loadMe = (complement << 12) | (myValue << 8) | (complement << 4) | myValue;
	writeRegister(vme_para(9, 0x80), loadMe);
}

void DDU::writeInputRegister(int value)
{
	writeRegister(vme_para(8, 0x80), value);
}

///////////////////////////////////////////////////////////////////////////////
// Read VME Serial/Flash
///////////////////////////////////////////////////////////////////////////////

int DDU::readSerialStatus()
{
	return readRegister(vme_seri(4,0)) & 0xff;
}

int DDU::readFlashKillFiber()
{
	return readRegister(vme_seri(4, 1));
}

int DDU::readFlashBoardID()
{
	return readRegister(vme_seri(4, 3));
}

int DDU::readFlashRUI()
{
	return readRegister(vme_seri(4, 7));
}

///////////////////////////////////////////////////////////////////////////////
// Write VME Serial/Flash
///////////////////////////////////////////////////////////////////////////////

void DDU::writeFlashKillFiber(int value)
{
                // Input register needs to be written first before updating flash.
		writeInputRegister(value & 0xffff);
                // Bogus data for sending to the VMESERI path.
		writeRegister(vme_seri(4, 9), 0);
		// Flash needs to sleep after writing
		usleep(100000);
}

void DDU::writeFlashBoardID(int value)
{
		writeInputRegister(value);
		writeRegister(vme_seri(4, 0xB), 0);
		usleep(100000);
}

void DDU::writeFlashRUI(const int &value)
{
		writeInputRegister(value);
		writeRegister(vme_seri(4, 0x0f), 0);
		usleep(100000);
}

///////////////////////////////////////////////////////////////////////////////
// Read SADC
///////////////////////////////////////////////////////////////////////////////
std::vector<float> DDU::readTempsVolts()
{
     std::vector<float> readout;
     int dataw=0xff89, datar;
     float Vout, cval;

     readout.clear();
     for (unsigned i=0; i<8; i++)
     {
        writeRegister(0xd000, dataw);
        datar=readRegister(0xd004);
        if(datar!=0xbaad)
        {
            Vout=(datar&0xffff)*0.001;
            if(i<4) cval=1/(0.001049406423+0.0002133635468*log(65000.0/Vout-13000.0)+0.7522287E-7*pow(log(65000.0/Vout-13000.0),3.0))-273.15;
            else cval=Vout;
            readout.push_back(cval);
        }
        else
        {
            readout.push_back(0.);
        }
        dataw += 0x10;
     }
     return readout;
}

void DDU::disableFMM()
{
	return writeFMM(0xFED0);
}

void DDU::enableFMM()
{
	return writeFMM(0xFED8);
}

int DDU::readRealFMM()
{
	return (readParallelStatus() >> 8) & 0xf;
}

unsigned DDU::readUserIDCode(int dev, int code, int cnt)
{
        // implemented as direct VME sequence for now. will change to jtag scan later.
        unsigned id;
        int jbase= (dev&0xF) << 12;
        int jtag= jbase + ((cnt-1) <<8) + (7 <<2);   // SIR
        write_later(jtag, code);
        jtag= jbase + (15 <<8) + (1 <<2);   // SDR with header
        write_later(jtag, 0);
        jtag= jbase + (15 <<8) + (5 <<2);   // read
        read_later(jtag);
        jtag= jbase + (15 <<8) + (2 <<2);   // SDR with trailer
        write_later(jtag, 0);
        jtag= jbase + (15 <<8) + (5 <<2);   // read
        read_now(jtag, (char *)(&id));
        return id;
}

unsigned DDU::CtrlFpgaUserCode()
{
	return readUserIDCode(5, 0x3c8, 10);
}

unsigned DDU::CtrlFpgaIdCode()
{
	return readUserIDCode(5, 0x3c9, 10);
}

unsigned DDU::VmePromUserCode()
{
	return readUserIDCode(2, 0xFD, 8);
}

unsigned DDU::VmePromIdCode()
{
	return readUserIDCode(2, 0xFE, 8);
}


} // namespace emu::pc
} // namespace emu
