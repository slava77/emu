/*****************************************************************************\
* $Id: DDUDebugger.h,v 1.4 2009/07/08 15:42:55 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_DDUDEBUGGER_H__
#define __EMU_FED_DDUDEBUGGER_H__

#include <string>
#include <vector>
#include <map>
#include <utility>

#include "emu/fed/JTAG_constants.h"
#include "emu/fed/DDU.h"

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

		namespace DDUDebugger {
		
			/** Reads from method @sa DDU::readFPGAStat(DDUFPGA) **/
			std::map<std::string, std::string> DDUFPGAStat(const uint32_t &stat);
		
			/** Reads from method @sa DDU::readOutputStat **/
			std::map<std::string, std::string> OutputStat(const uint16_t &stat);
		
			/** Reads from method @sa DDU::readEBReg(1) **/
			std::map<std::string, std::string> EBReg1(const uint16_t &stat);
		
			/** Reads from method @sa DDU::readEBReg(2) **/
			std::map<std::string, std::string> EBReg2(const uint16_t &stat);
		
			/** Reads from method @sa DDU::readEBReg(3) **/
			std::map<std::string, std::string> EBReg3(const uint16_t &stat);
		
			/** Reads from method @sa DDU::checkFIFO(2) **/
			std::map<std::string, std::string> FIFO2(const uint16_t &stat);
		
			/** Reads from method @sa DDU::readFFError() **/
			std::map<std::string, std::string> FFError(const uint16_t &stat);
		
			/** Reads from method @sa DDU::readInCHistory() **/
			std::map<std::string, std::string> InCHistory(const uint16_t &stat);
		
			/** Reads from method @sa DDU::readWarnMon() **/
			std::map<std::string, std::string> WarnMon(const uint16_t &stat);
		
			/** Reads from method @sa DDU::readDebugTrap(DDUFPGA) **/
			std::vector<std::string> DDUDebugTrap(const std::vector<uint16_t> &lcode, DDU *thisDDU);
		
			/** Reads from method @sa DDU::readKillFiber() **/
			std::map<std::string, std::string> KillFiber(const uint32_t &stat);
		
			/** Reads from method @sa DDU::readFPGAStat(INFPGAX) **/
			std::map<std::string, std::string> InFPGAStat(const enum DEVTYPE &dt, const uint32_t &stat);
		
			/** Reads from method @sa DDU::readFIFOStat(INFPGAX) **/
			std::map<std::string, std::string> FIFOStat(const enum DEVTYPE &dt, const uint16_t &stat);
		
			/** Reads from method @sa DDU::readFIFOFull(INFPGAX) **/
			std::map<std::string, std::string> FIFOFull(const enum DEVTYPE &dt, const uint16_t &stat);
		
			/** Reads from method @sa DDU::readCCodeStat(INFPGAX) **/
			std::map<std::string, std::string> CCodeStat(const enum DEVTYPE &dt, const uint16_t &stat);
		
			/** Reads from method @sa DDU::readFiberDiagnostics(INFPGAX,Y) **/
			std::map<std::string, std::string> FiberDiagnostics(const enum DEVTYPE &dt, const uint16_t &reg, const uint32_t &stat);
		
			/** Reads from method @sa DDU::readWriteMemoryActive(INFPGAX) **/
			std::map<std::string, std::string> WriteMemoryActive(const enum DEVTYPE &dt, const uint16_t &iFiber, const uint16_t &stat);
		
			/** Reads from method @sa DDU::readDebugTrap(INFPGAX) **/
			std::vector<std::string> INFPGADebugTrap(const std::vector<uint16_t> &lcode, const enum DEVTYPE &dt);
		
			/** Reads from method @sa DDU::readParallelStat() **/
			std::map<std::string, std::string> ParallelStat(const uint16_t &stat);
		
			/** Reads from DDU::readFMM **/
			std::map<std::string, std::string> FMM(const uint16_t &stat);
		
			/** Reads from method @sa DDU::readGbEPrescale() **/
			std::map<std::string, std::string> GbEPrescale(const uint16_t &stat);
		
			/** Reads from method @sa DDU::readFakeL1Reg() **/
			std::map<std::string, std::string> FakeL1Reg(const uint16_t &stat);
		
			/** Reads from method @sa DDU::readFMMReg() **/
			std::map<std::string, std::string> F0EReg(const uint16_t &stat);

			/** Reads from DDU::readTemperature **/
			std::pair<std::string, std::string> Temperature(const float &temp, const enum DDU::TEMPSCALE &scale = DDU::CELSIUS);

			/** Reads from DDU::readVoltage **/
			std::pair<std::string, std::string> Voltage(const uint8_t &sensor, const float &voltage);
		
		}

	}
}


#endif
