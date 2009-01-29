/*****************************************************************************\
* $Id: DDU.h,v 3.25 2009/01/29 15:31:22 paste Exp $
*
* $Log: DDU.h,v $
* Revision 3.25  2009/01/29 15:31:22  paste
* Massive update to properly throw and catch exceptions, improve documentation, deploy new namespaces, and prepare for Sentinel messaging.
*
* Revision 3.24  2008/11/14 09:34:31  paste
* Updated IRQ thread handling to fix and abstract FMM enabling and disabling.
*
* Revision 3.23  2008/09/24 18:38:38  paste
* Completed new VME communication protocols.
*
* Revision 3.22  2008/09/22 14:31:53  paste
* /tmp/cvsY7EjxV
*
* Revision 3.20  2008/09/07 22:25:35  paste
* Second attempt at updating the low-level communication routines to dodge common-buffer bugs.
*
* Revision 3.19  2008/09/01 11:30:32  paste
* Added features to DDU, IRQThreads corresponding to new DDU firmware.
*
* Revision 3.18  2008/08/25 12:25:49  paste
* Major updates to VMEController/VMEModule handling of CAEN instructions.  Also, added version file for future RPMs.
*
* Revision 3.17  2008/08/15 10:40:20  paste
* Working on fixing CAEN controller opening problems
*
* Revision 3.15  2008/08/15 08:35:50  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#ifndef __DDU_H__
#define __DDU_H__

#include <vector>
#include <string>

#include "JTAG_constants.h"
#include "FEDException.h"

#include "VMEModule.h"

namespace emu {

	namespace fed {

		class Chamber;

		/** @class DDU A class representing the Detector-Dependent Unit boards in the EMU FED Crates. **/
		class DDU: public VMEModule
		{
			friend class DDUParser;
			friend class FEDCrateParser;

		public:

			/** @param slot the slot of the board for VME addressing purposes. **/
			DDU(int mySlot);

			/** Default destructor. **/
			virtual ~DDU();

			/** @returns the KillFiber bit-mask (LS 15 bits) and the options bits (MS 5 bits). **/
			inline uint16_t getKillFiber() { return killfiber_; }

			/** @returns the GBEPrescale setting. **/
			inline uint16_t getGbEPrescale() { return gbe_prescale_; }

			/** Configures the DDU. **/
			void configure()
			throw (OutOfBoundsException, CAENException, DevTypeException);
			
			/** Part of the suite of chamber methods.
			*	@returns a vector of chambers in fiber-order.
			**/
			std::vector<Chamber *> getChambers();

			/** Part of the suite of chamber methods.
			*	@param fiberNumber runs from 0-14.
			*	@returns the chamber at the given fiber input number.
			**/
			Chamber *getChamber(unsigned int fiberNumber)
			throw (OutOfBoundsException);

			/** Adds a chamber object to the DDU.
			*	@param chamber is the chamber being added.
			*	@param fiberNumber is the fiber slot of the chamber.
			**/
			void addChamber(Chamber *chamber, unsigned int fiberNumber)
			throw (OutOfBoundsException);

			/** Sets the vector of chamber objects in the DDU to some vector.
			*	@param chamberVector is a vector of chambers to copy to the internal vector.
			**/
			void setChambers(std::vector<Chamber *> chamberVector)
			throw (OutOfBoundsException);

			// PGK New interface
			// Read VME Parallel registers
			/** @returns the FMM register. **/
			uint16_t readFMM()
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns the combined error status of each fiber. **/
			uint16_t readCSCStatus()
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns the FMM busy status of each fiber. **/
			uint16_t readFMMBusy()
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns the FMM warning status of each fiber. **/
			uint16_t readFMMFullWarning()
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns the FMM sync lost status of each fiber. **/
			uint16_t readFMMLostSync()
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns the FMM error status of each fiber. **/
			uint16_t readFMMError()
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns the status of the hardware switches on-board. **/
			uint16_t readSwitches()
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns the VME parallel status register. **/
			uint16_t readParallelStatus()
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns the value of a given parallel input register. **/
			uint16_t readInputRegister(uint8_t iReg)
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns the value of the fake L1 register. **/
			uint16_t readFakeL1()
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns the value of the GbE prescale register. **/
			uint16_t readGbEPrescale()
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns the value of one of the (unused) test registers. **/
			uint16_t readTestRegister(uint8_t iReg)
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns which fibers have experienced a busy status since the last resync. **/
			uint16_t readBusyHistory()
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns which fibers have experienced a warning status since the last resync. **/
			uint16_t readWarningHistory()
			throw (OutOfBoundsException, CAENException, DevTypeException);

			// Write VME Parallel registers

			/** Writes a value to the FMM register (useful for TTS tests). **/
			void writeFMM(uint16_t value)
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** Writes a value to the fake L1 register. **/
			void writeFakeL1(uint16_t value)
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** Writes to the GbE prescale register. **/
			void writeGbEPrescale(uint8_t value)
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** Writes to one of the parallel input registers (for flash writing). **/
			void writeInputRegister(uint16_t value)
			throw (OutOfBoundsException, CAENException, DevTypeException);

			// Read VME Serial/Flash registers

			/** @returns the status of the serial path. **/
			uint8_t readSerialStatus()
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns reads back the kill fiber setting stored in flash. **/
			uint16_t readFlashKillFiber()
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns the board ID stored in flash. **/
			uint16_t readFlashBoardID()
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns the board RUI stored in flash. **/
			uint16_t readFlashRUI()
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns the GbE FIFO thresholds stored in flash. **/
			std::vector<uint16_t> readFlashGbEFIFOThresholds()
			throw (OutOfBoundsException, CAENException, DevTypeException);

			// Write VME Serial/Flash registers

			/** Writes a value to the kill fiber flash register. **/
			void writeFlashKillFiber(uint16_t value)
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** Writes a value to the board ID flash register. **/
			void writeFlashBoardID(uint16_t value)
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** Writes a value to the board RUI flash register. **/
			void writeFlashRUI(uint16_t value)
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** Writes a value to the GbE FIFO threshold flash register. **/
			void writeFlashGbEFIFOThresholds(std::vector<uint16_t> values)
			throw (OutOfBoundsException, CAENException, DevTypeException);

			// Read SADC registers

			/** @returns the calculated temperature of a given SADC sensor. **/
			float readTemperature(uint8_t sensor)
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns the calculated coltage of a given SADC sensor. **/
			float readVoltage(uint8_t sensor)
			throw (OutOfBoundsException, CAENException, DevTypeException);

			// Read DDUFPGA JTAG registers

			/** @returns the status of the DDUFPGA output to the INFPGAs. **/
			uint16_t readOutputStatus()
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns the status of a given DDUFPGA input FIFO. **/
			uint16_t readFIFOStatus(uint8_t fifo)
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns which fibers are experiencing a FIFO-full error. **/
			uint16_t readFFError()
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns which fibers are experiencing a CRC error. **/
			uint16_t readCRCError()
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns which fibers are experiencing a transmit error. **/
			uint16_t readXmitError()
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns the kill fiber mask that is loaded into the DDUFPGA. **/
			uint32_t readKillFiber()
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns which fibers report corruption in the DMB data. **/
			uint16_t readDMBError()
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns which fibers report corruption in the TMB data. **/
			uint16_t readTMBError()
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns which fibers report corruption in the ALCT data. **/
			uint16_t readALCTError()
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns which fibers are experiencing a lost-in-event error. **/
			uint16_t readLIEError()
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns the status of the paths out to the INFPGAs. **/
			uint16_t readInRDStat()
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns which fibers have ever experineced input corruption (?). **/
			uint16_t readInCHistory()
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns the infamous "Error B" register. **/
			uint16_t readEBRegister(uint8_t reg)
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns which fibers report communication with the DMB. **/
			uint16_t readDMBLive()
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns which fibers reported communication with the DMB when the first L1A came though. **/
			uint16_t readDMBLiveAtFirstEvent()
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns which fibers report a warning status. **/
			uint16_t readWarningMonitor()
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns the maximum timeout between the L1A and the data sent from the DMBs. **/
			uint16_t readMaxTimeoutCount()
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns the bunch-crossing orbit setting. **/
			uint16_t readBXOrbit()
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** Magically toggles the L1 calibration setting. **/
			void toggleL1Calibration()
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns the board RUI stored in the DDUFPGA. **/
			uint16_t readRUI()
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** Magically sends a fake L1A through the DDU. **/
			void sendFakeL1A()
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns the occupancies of the fiber it is point to, then increments its internal pointer to point at the next fiber. **/
			std::vector<uint32_t> readOccupancyMonitor()
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns some errors that escaped the DDUFPGA previously. **/
			uint16_t readAdvancedFiberErrors()
			throw (OutOfBoundsException, CAENException, DevTypeException);

			// Write DDUFPGA JTAG registers

			/** Writes to the kill fiber register on the DDUFPGA. **/
			void writeKillFiber(uint32_t value)
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** Writes to the bunch-crossing orbit register on the DDUFPGA. **/
			void writeBXOrbit(uint16_t value)
			throw (OutOfBoundsException, CAENException, DevTypeException);

			// Read INFPGA JTAG registers

			/** @returns the LS 32 bits of the L1 scaler. **/
			uint32_t readL1Scaler1(enum DEVTYPE dev)
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns the combined status of the fibers connected to the INFPGA. **/
			uint16_t readFiberStatus(enum DEVTYPE dev)
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns the DMB sync status of the fibers connected to the INFPGA. **/
			uint16_t readDMBSync(enum DEVTYPE dev)
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns the combined input FIFO status of a given INFPGA. **/
			uint16_t readFIFOStatus(enum DEVTYPE dev)
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns which fibers on a given INFPGA have a full input FIFO. **/
			uint16_t readFIFOFull(enum DEVTYPE dev)
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns which fibers on a given INFPGA have a receiving error. **/
			uint16_t readRxError(enum DEVTYPE dev)
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns which fibers on a given INFPGA have a timeout error. **/
			uint16_t readTimeout(enum DEVTYPE dev)
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns which fibers on a given INFPGA have a transmit error. **/
			uint16_t readTxError(enum DEVTYPE dev)
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns which memory modules are being written by a given fiber on a given INFPGA. **/
			uint16_t readActiveWriteMemory(enum DEVTYPE dev, uint8_t iFiber)
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns how many memory modules are available for a given INFPGA. **/
			uint16_t readAvailableMemory(enum DEVTYPE dev)
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns the minimum number of memory modules an INFPGA has had to use since the last resync. **/
			uint16_t readMinMemory(enum DEVTYPE dev)
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns which fibers on a given INFPGA have a lost-in-event error (?). **/
			uint16_t readLostError(enum DEVTYPE dev)
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns the infamous "C-Code" register. **/
			uint16_t  readCCodeStatus(enum DEVTYPE dev)
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns which fibers on a given INFPGA are experiencing a DMB warning condition. **/
			uint16_t readDMBWarning(enum DEVTYPE dev)
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns the fiber diagnostic register on a given INFPGA. **/
			uint32_t readFiberDiagnostics(enum DEVTYPE dev, uint8_t iDiagnostic)
			throw (OutOfBoundsException, CAENException, DevTypeException);

			// User and ID code reading

			/** @returns the user code of the given device. **/
			uint32_t readUserCode(enum DEVTYPE dev)
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns the ID code of the given device. **/
			uint32_t readIDCode(enum DEVTYPE dev)
			throw (OutOfBoundsException, CAENException, DevTypeException);

			// Reset FPGAs

			/** Sends a reset command to the given FPGA. **/
			void resetFPGA(enum DEVTYPE dev)
			throw (OutOfBoundsException, CAENException, DevTypeException);

			// Read Universal FPGA JTAG registers

			/** @returns the status register of a given FPGA. **/
			uint32_t readFPGAStatus(enum DEVTYPE dev)
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns the L1 scaler register of a given FPGA. **/
			uint32_t readL1Scaler(enum DEVTYPE dev)
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** @returns the debug trap from a given FPGA. **/
			std::vector<uint16_t> readDebugTrap(enum DEVTYPE dev)
			throw (OutOfBoundsException, CAENException, DevTypeException);

			// Misc. routines

			/** Loads the proper value in the FMM register to disable sending FMM signals. **/
			void disableFMM()
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** Loads the proper value in the FMM register to enable sending FMM signals for a short time. **/
			void enableFMM()
			throw (OutOfBoundsException, CAENException, DevTypeException);
			
		protected:

			// PGK New interface
			/** Reads an arbitrary number of bits from a given register on a given device.
			*
			*	@param dev is the device from which to read.
			*	@param myReg is the register on the device from which to read.
			*	@param @nBits is the number of bits to read.
			**/
			std::vector<uint16_t> readRegister(enum DEVTYPE dev, int myReg, unsigned int nBits)
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/** Writes an arbitrary number of bits to a given register on a given device.
			*
			*	@param dev is the device to which to write.
			*	@param myReg is the register on the device to which to write.
			*	@param @nBits is the number of bits to write.
			*	@param @myData is the data to write.
			**/
			std::vector<uint16_t> writeRegister(enum DEVTYPE dev, int myReg, unsigned int nBits, std::vector<uint16_t> myData)
			throw (OutOfBoundsException, CAENException, DevTypeException);

			/// The chambers that are plugged into this DDU, in fiber-order.
			std::vector<Chamber *> chamberVector_;

			/// The GbE prescale code as read from the configuration XML.
			int gbe_prescale_;

			/// The kill fiber mask as read from the configuration XML.
			unsigned long int killfiber_;
			
		};

	}
}

#endif

