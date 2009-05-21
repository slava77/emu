/*****************************************************************************\
* $Id: DDUDebugger.h,v 1.2 2009/05/21 15:30:48 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_DDUDEBUGGER_H__
#define __EMU_FED_DDUDEBUGGER_H__

#include <string>
#include <vector>
#include <map>
#include <utility>

#include "emu/fed/JTAG_constants.h"

/** A namespace that reads particular variables from the DDU and returns debugging
*	information.  Everything is returned as a std::vector of std::pairs, the 2nd element
*	being the class of the particular debug message
*	(@sa LocalEmuApplication::CSS) and the 1st is the actual debug message.
*
*	@author Phillip Killewald
*	@author Jason Gilmore
**/

namespace emu {

	namespace fed {

		class DDU;

		namespace DDUDebugger {
		
			/** Reads from method @sa DDU::readFPGAStat(DDUFPGA) **/
			std::map<std::string, std::string> DDUFPGAStat(unsigned long int stat);
		
			/** Reads from method @sa DDU::readOutputStat **/
			std::map<std::string, std::string> OutputStat(int stat);
		
			/** Reads from method @sa DDU::readEBReg(1) **/
			std::map<std::string, std::string> EBReg1(int stat);
		
			/** Reads from method @sa DDU::readEBReg(2) **/
			std::map<std::string, std::string> EBReg2(int stat);
		
			/** Reads from method @sa DDU::readEBReg(3) **/
			std::map<std::string, std::string> EBReg3(int stat);
		
			/** Reads from method @sa DDU::checkFIFO(2) **/
			std::map<std::string, std::string> FIFO2(int stat);
		
			/** Reads from method @sa DDU::readFFError() **/
			std::map<std::string, std::string> FFError(int stat);
		
			/** Reads from method @sa DDU::readInCHistory() **/
			std::map<std::string, std::string> InCHistory(int stat);
		
			/** Reads from method @sa DDU::readWarnMon() **/
			std::map<std::string, std::string> WarnMon(int stat);
		
			/** Reads from method @sa DDU::readDebugTrap(DDUFPGA) **/
			std::vector<std::string> DDUDebugTrap(std::vector<uint16_t> lcode, DDU *thisDDU);
		
			/** Reads from method @sa DDU::readKillFiber() **/
			std::map<std::string, std::string> KillFiber(long int stat);
		
			/** Reads from method @sa DDU::readFPGAStat(INFPGAX) **/
			std::map<std::string, std::string> InFPGAStat(enum DEVTYPE dt, unsigned long int stat);
		
			/** Reads from method @sa DDU::readFIFOStat(INFPGAX) **/
			std::map<std::string, std::string> FIFOStat(enum DEVTYPE dt, int stat);
		
			/** Reads from method @sa DDU::readFIFOFull(INFPGAX) **/
			std::map<std::string, std::string> FIFOFull(enum DEVTYPE dt, int stat);
		
			/** Reads from method @sa DDU::readCCodeStat(INFPGAX) **/
			std::map<std::string, std::string> CCodeStat(enum DEVTYPE dt, int stat);
		
			/** Reads from method @sa DDU::readFiberDiagnostics(INFPGAX,Y) **/
			std::map<std::string, std::string> FiberDiagnostics(enum DEVTYPE dt, int reg, unsigned long int stat);
		
			/** Reads from method @sa DDU::readWriteMemoryActive(INFPGAX) **/
			std::map<std::string, std::string> WriteMemoryActive(enum DEVTYPE dt, int iFiber, int stat);
		
			/** Reads from method @sa DDU::readDebugTrap(INFPGAX) **/
			std::vector<std::string> INFPGADebugTrap(std::vector<uint16_t> lcode, enum DEVTYPE dt);
		
			/** Reads from method @sa DDU::readParallelStat() **/
			std::map<std::string, std::string> ParallelStat(int stat);
		
			/** Reads from DDU::readFMM **/
			std::map<std::string, std::string> FMM(uint16_t stat);
		
			/** Reads from method @sa DDU::readGbEPrescale() **/
			std::map<std::string, std::string> GbEPrescale(int stat);
		
			/** Reads from method @sa DDU::readFakeL1Reg() **/
			std::map<std::string, std::string> FakeL1Reg(int stat);
		
			/** Reads from method @sa DDU::readFMMReg() **/
			std::map<std::string, std::string> F0EReg(int stat);

			/** Reads from DDU::readTemperature **/
			std::pair<std::string, std::string> Temperature(float temp);

			/** Reads from DDU::readVoltage **/
			std::pair<std::string, std::string> Voltage(uint8_t sensor, float voltage);
		
		}

	}
}


#endif
