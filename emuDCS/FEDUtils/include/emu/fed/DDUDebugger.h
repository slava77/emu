/*****************************************************************************\
* $Id: DDUDebugger.h,v 1.10 2012/08/27 14:45:11 cvuosalo Exp $
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
*	(LocalEmuApplication::CSS) and the 1st is the actual debug message.
*
*	@author Phillip Killewald
*	@author Jason Gilmore
**/

namespace emu {

	namespace fed {

		namespace DDUDebugger {

			/** Reads from method DDU::readFPGAStatus() **/
			std::map<std::string, std::string> FPGAStatus(const enum DEVTYPE &dev, const uint32_t &stat);

			/** Reads from method DDU::readOutputStatus() **/
			std::map<std::string, std::string> OutputStatus(const uint16_t &stat);
		
			/** Reads from method DDU::readEBRegister() **/
			std::map<std::string, std::string> EBRegister(const uint8_t &reg, const uint16_t &stat);

			/** Reads from method DDU::readDebugTrap(DDUFPGA) **/
			std::vector<std::string> DDUDebugTrap(const std::vector<uint16_t> &lcode, DDU *thisDDU);

			unsigned int readErrors(DDU *const thisDDU, std::vector<std::string> &out, std::stringstream &outStream);
		
			/** Reads from method DDU::readKillFiber() **/
			std::map<std::string, std::string> KillFiber(const uint32_t &stat);

			/** Reads from method DDU::readDebugTrap(INFPGAX) **/
			std::vector<std::string> INFPGADebugTrap(const enum DEVTYPE &dt, const std::vector<uint16_t> &lcode);

			/** Reads from DDU::readParallelStatus **/
			std::map<std::string, std::string> ParallelStatus(const uint16_t &stat);
			
			/** Reads from DDU::readFMM **/
			std::map<std::string, std::string> FMM(const uint16_t &stat);
			
			/** Reads from DDU::readRealFMM **/
			std::pair<std::string, std::string> RealFMM(const uint8_t &stat);
		
			/** Reads from method DDU::readGbEPrescale() **/
			std::map<std::string, std::string> GbEPrescale(const uint16_t &stat);
		
			/** Reads from method DDU::readFakeL1Reg() **/
			std::map<std::string, std::string> FakeL1(const uint16_t &stat);

			/** Reads from DDU::readTemperature() **/
			std::pair<std::string, std::string> Temperature(const float &temp, const enum DDU::TEMPSCALE &scale = DDU::CELSIUS);

			/** Reads from DDU::readVoltage() **/
			std::pair<std::string, std::string> Voltage(const uint8_t &sensor, const float &voltage);
			
			/** Reads a 16-bit value and spits out the names of the fibers associated with the DDU for each bit high in the number.  **/
			std::map<std::string, std::string> DebugFiber(DDU *ddu, const uint16_t &value, const std::string &className = "none");
			
			/** Reads from DDU::readInRDStatus **/
			std::map<std::string, std::string> InRDStatus(const uint16_t &stat);

			/** Spits out a space-deliminated string of "fibers X-Y" based on the 4-bit input. **/
			std::string decodeInRD(const uint8_t &stat);
			
			/** Spits out a space-deliminated string of fiber names based on the input INFPGA, the DDU, and the 8-bit status. **/
			std::string decodeINFPGAFibers(const enum DEVTYPE &dev, const uint8_t &stat, DDU *duu);
			
			/** Reads from method DDI::readInCHistory() **/
			std::map<std::string, std::string> InCHistory(const uint16_t &stat);
			
			/** Reads from method DDU::readFFError() **/
			std::map<std::string, std::string> FFError(const uint16_t &stat);
			
			/** Reads from method DDU::readMaxTimeoutCount() **/
			std::map<std::string, std::string> MaxTimeoutCount(const uint16_t &stat);
			
			/** Reads from method DDU::readFIFOStatus(FIFO) **/
			std::map<std::string, std::string> FIFOStatus(const uint8_t &fifo, const uint16_t &stat);
			
			/** Reads from method DDU::readFIFOStatus(DEVTYPE) **/
			std::map<std::string, std::string> FIFOStatus(const enum DEVTYPE &dev, const uint8_t &fifo, const uint16_t &stat);
			
			/** Reads from method DDU::readFIFOStatus(DEVTYPE) **/
			std::map<std::string, std::string> FIFOStatus(const enum DEVTYPE &dev, const uint16_t &stat, DDU *ddu);

			/** Reads from method DDU::readSerialStatus() **/
			std::pair<std::string, std::string> SerialStatus(const uint8_t &stat);
			
			/** Reads from method DDU::readFiberStatus(DEVTYPE) **/
			std::map<std::string, std::string> FiberStatus(const enum DEVTYPE &dev, const uint16_t &stat, DDU *ddu);
			
			/** Reads from method DDU::readDMBSync(DEVTYPE) **/
			std::map<std::string, std::string> DMBSync(const enum DEVTYPE &dev, const uint16_t &stat, DDU *ddu);
			
			/** Reads from method DDU::readFIFOFull(DEVTYPE) **/
			std::map<std::string, std::string> FIFOFull(const enum DEVTYPE &dev, const uint16_t &stat, DDU *ddu);
			
			/** Reads from method DDU::readRxError(DEVTYPE) **/
			std::map<std::string, std::string> RxError(const enum DEVTYPE &dev, const uint16_t &stat, DDU *ddu);
			
			/** Reads from method DDU::readTxError(DEVTYPE) **/
			std::map<std::string, std::string> TxError(const enum DEVTYPE &dev, const uint16_t &stat, DDU *ddu);
			
			/** Reads from method DDU::readTimeout(DEVTYPE) **/
			std::map<std::string, std::string> Timeout(const enum DEVTYPE &dev, const uint16_t &stat, DDU *ddu);
			
			/** Reads from method DDU::readActiveWriteMemeory(DEVTYPE) **/
			std::map<std::string, std::string> ActiveWriteMemory(const enum DEVTYPE &dev, const uint8_t &reg, const uint16_t &stat, DDU *ddu);
		
			/** Reads from method DDU::readAvailableMemeory(DEVTYPE) **/
			std::map<std::string, std::string> AvailableMemory(const enum DEVTYPE &dev, const uint16_t &stat);
			
			/** Reads from method DDU::readLostError(DEVTYPE) **/
			std::map<std::string, std::string> LostError(const enum DEVTYPE &dev, const uint16_t &stat, DDU *ddu);
			
			/** Reads from method DDU::readCCodeStatus(DEVTYPE) **/
			std::map<std::string, std::string> CCodeStatus(const enum DEVTYPE &dev, const uint16_t &stat);
			
			/** Reads from method DDU::readDMBWarning(DEVTYPE) **/
			std::map<std::string, std::string> DMBWarning(const enum DEVTYPE &dev, const uint16_t &stat, DDU *ddu);
			
			/** Converts firmware verson code into a readable text string **/
			std::string FirmwareDecode(const uint32_t &code);
		}

	}
}


#endif
