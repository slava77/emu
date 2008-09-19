/*****************************************************************************\
* $Id: DCC.h,v 3.20 2008/09/19 16:53:51 paste Exp $
*
* $Log: DCC.h,v $
* Revision 3.20  2008/09/19 16:53:51  paste
* Hybridized version of new and old software.  New VME read/write functions in place for all DCC communication, some DDU communication.  New XML files required.
*
* Revision 3.19  2008/09/07 22:25:35  paste
* Second attempt at updating the low-level communication routines to dodge common-buffer bugs.
*
* Revision 3.18  2008/08/26 13:40:08  paste
* Updating and adding documentation
*
* Revision 3.17  2008/08/25 12:25:49  paste
* Major updates to VMEController/VMEModule handling of CAEN instructions.  Also, added version file for future RPMs.
*
* Revision 3.16  2008/08/15 10:40:20  paste
* Working on fixing CAEN controller opening problems
*
* Revision 3.14  2008/08/15 08:35:50  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#ifndef __DCC_H__
#define __DCC_H__

#include <string>

#include "JTAG_constants.h"
#include "FEDException.h"

#include "VMEModule.h"

namespace emu {

	namespace fed {

		/** @class DCC A class for talking to DCC boards. **/
		class DCC: public VMEModule
		{
		public:
			friend class DCCParser;
		
			/** @deprecated Please use the DCC(int) constructor instead. **/
			DCC(int, int);

			/** @param slot the slot of the board for VME addressing purposes. **/
			DCC(int slot);

			virtual ~DCC();

			/** @return the type of board this is, useful for dynamic casting.
			 *
			 * @deprecated Not actually used any more?
			 **/
			virtual unsigned int boardType() const {return DCC_ENUM;}
		
			/** @return the FIFOInUse parameter. **/
			inline int getFIFOInUse() { return fifoinuse_; }

			/** Configure the DCC by loading the FIFOInUse and SoftwareSwitch variables to
			 * the PROMs.
			**/
			void configure();
		
			// DCC commands
			/*
			unsigned long int inprom_userid();
			unsigned long int mprom_userid();
			unsigned long int inprom_chipid();
			unsigned long int mprom_chipid();
			void inpromuser(enum DEVTYPE devnum, char *c);
			void mctrl_bxr();
			void mctrl_evnr();
			void mctrl_fakeL1A(char rate, char num);
			void mctrl_fifoinuse(unsigned short int fifo);
			void mctrl_reg(char *c);
			void mctrl_swset(unsigned short int swset);
			unsigned short int  mctrl_swrd();
			void mctrl_fmmset(unsigned short int fmmset);
			unsigned short int  mctrl_fmmrd();
			unsigned short int  mctrl_stath();
			unsigned short int  mctrl_statl();
			unsigned short int  mctrl_ratemon(int address);
			unsigned short int  mctrl_rd_fifoinuse();
			unsigned short int  mctrl_rd_ttccmd();
			void mctrl_ttccmd(unsigned short int ctcc);
			*/
		
			// PGK First attempt at simplified DCC commands
			unsigned long int readReg(enum DEVTYPE dt, char reg, unsigned int nBits = 4)
				throw (FEDException);
			

			void writeReg(enum DEVTYPE dt, char reg, unsigned long int value)
				throw (FEDException);
		
			unsigned int readStatusHigh()
				throw (FEDException);
			unsigned int readStatusLow()
				throw (FEDException);
			unsigned int readFIFOInUse()
				throw (FEDException);
			void setFIFOInUse(unsigned int value)
				throw (FEDException);
			unsigned int readRate(unsigned int fifo)
				throw (FEDException);
			unsigned int readSoftwareSwitch()
				throw (FEDException);
			void setSoftwareSwitch(unsigned int value)
				throw (FEDException);
			unsigned int readFMM()
				throw (FEDException);
			void setFMM(unsigned int value)
				throw (FEDException);
			unsigned int readTTCCommand()
				throw (FEDException);
			void setTTCCommand(unsigned int value)
				throw (FEDException);
			void resetBX()
				throw (FEDException);
			void resetEvents()
				throw (FEDException);
			void setFakeL1A(unsigned int value)
				throw (FEDException);
			unsigned long int readIDCode(enum DEVTYPE dt)
				throw (FEDException);
			unsigned long int readUserCode(enum DEVTYPE dt)
				throw (FEDException);
		
			// EPROM reprogramming (EXPERTS ONLY !)
			/* void hdrst_main(void); */
			/* void hdrst_in(void); */
			void epromload(char *design, enum DEVTYPE devnum, char *downfile, int writ);
		
			// PGK Simplified DCC commands
			/** Hard reset the crate through a TTC-override command. **/
			void crateHardReset();
			
			/** Sync reset the crate through a TTC-override command. **/
			void crateSyncReset();
			
			/** @returns the slot number of the DDU corresponding to the given input
			 * fifo
			 *
			 * @param fifo the input fifo number from which to calculate the DDU slot
			 **/
			unsigned int getDDUSlotFromFIFO(unsigned int fifo);

			// PGK New interface

			/** @returns the high 16-bits of the DCC status register **/
			uint16_t readStatusHighAdvanced()
				throw (FEDException);

			/** @returns the low 16-bits of the DCC status register
			 *
			 * @note These 16 bits are actually independent of the value returned from
			 * readStatusHighAdvanced.  The name is purely historical.
			 **/
			uint16_t readStatusLowAdvanced()
				throw (FEDException);

			/** @returns the input FIFOs (DDUs) that are being read in
			 * to the DCC as a binary number where a 1 in the nth bit means that
			 * FIFO n is being read in.
			 *
			 * @note There are only 10 fifos, but the first 11 bits are not ignored.
			 * 
			 * @sa getDDUSlotFromFIFO
			 **/
			uint16_t readFIFOInUseAdvanced()
				throw (FEDException);

			/** Write a new value to the FIFOInUse register.
			 *
			 * @param value the new FIFOInUse register value to write.
			 *
			 * @note There are only 10 FIFOs, but only the high 5 bits of value are ignored.
			 *
			 * @sa getDDUSlotFromFIFO
			 **/
			void writeFIFOInUseAdvanced(uint16_t value)
				throw (FEDException);

			/** @returns the current read-in or read-out data rate (in bytes/sec) of
			 * the given FIFO (DDU or S-Link).
			 *
			 * @param fifo is the FIFO from which to read the data rate.  fifo==0
			 * corresponds to S-Link 0, fifo==[1,5] correspond to the DDU input FIFOs
			 * 1-5, fifo==6 corresponds to S-Link 1, and fifo==[7,11] correspond to
			 * the DDU input FIFOs 6-10.
			 **/
			uint16_t readRateAdvanced(unsigned int fifo)
				throw (FEDException);

			/** @returns the current setting of the omni-purpose software switch.
			 *
			 * @note Some of the software switch values are documented in
			 * EmuFCrateHyperDAQ.cc.  The first 8 bits are decoded from the DCC
			 * manual as follows:
			 * bits 0-2:
			 * 111 == DCM on control FPGA will be reset
			 * ??1 == Immediately upon switching bit 0 from low to high, one L1A is sent
			 * 11? == L1As will be sent continuously with ~1.6 ms intervals between each
			 * bits 3-4:
			 * 00 == The INFPGA will generate (fake?) data on every L1A
			 * 01 == Normal operation
			 * 10 == Disable propagation of S-Link backpressure to DDUs
			 * 11 == Serial loopback on the custom backplane RocketIO
			 * bits 5-7: select the xilinx platform revision, which determines the
			 * S-Link (FMM) IDs.
			 **/
			uint16_t readSoftwareSwitchAdvanced()
				throw (FEDException);

			/** Set the omni-purpose software switch.
			 *
			 * @param value the value to which to set the software switch.
			 * 
			 * @sa readSoftwareSwitchAdvanced()
			 **/
			void writeSoftwareSwitchAdvanced(uint16_t value)
				throw (FEDException);

			/** @return the value of the FMM register. **/
			uint16_t readFMMAdvanced()
				throw (FEDException);

			/** Write a custom value to the FMM register.
			 *
			 * @param value the FMM value to set.
			 *
			 * @note useful for TTS tests.
			 **/
			void writeFMMAdvanced(uint16_t value)
				throw (FEDException);

			/** @return the last TTC command sent. **/
			uint16_t readTTCCommandAdvanced()
				throw (FEDException);

			/** Immediately sends a TTC command to the FED crate.
			 *
			 * @param value the command to send.  Useful values are 0x34 (FED
			 * crate hard reset) and 0x03 (FED crate sync reset).  Note that bit
			 * 12_0 of the software switch must be set high for these commands to
			 * work properly.
			 *
			 * @note This routine takes care of the unused 2 bits at the beginning
			 * of the TTC command byte for the user.  These 2 bits can be sent using
			 * the commands resetBXAdvanced() and resetEventsAdvanced().
			 **/
			void writeTTCCommandAdvanced(int8_t value)
				throw (FEDException);

			/** Send the TTC command to reset BX values in the FED crate. **/
			void resetBXAdvanced()
				throw (FEDException);

			/** Send the TTC command to reset event counters in the FED crate. **/
			void resetEventsAdvanced()
				throw (FEDException);

			/** Set the fake L1A generation register.
			 *
			 * @param value the low 8 bits are used to set the number of L1As to generate,
			 * the high 8 bits are used to set the rate at which they are generated.
			 **/
			void writeFakeL1AAdvanced(uint16_t value)
				throw (FEDException);

			/** @return the ID code from the given PROM chip.
			 *
			 * @param dev the device from which to read the ID code.
			 **/
			uint32_t readIDCodeAdvanced(enum DEVTYPE dev)
				throw (FEDException);

			/** @return the User code programmed into the given PROM chip.
			 *
			 * @param dev the device from which to read the User code.
			 **/
			uint32_t readUserCodeAdvanced(enum DEVTYPE dev)
				throw (FEDException);

			
		protected:
			int fifoinuse_;
			int softsw_;

			// PGK New interface
			
			std::vector<int16_t> readRegAdvanced(enum DEVTYPE dev, char myReg, unsigned int nBits)
				throw (FEDException);

			std::vector<int16_t> writeRegAdvanced(enum DEVTYPE dev, char myReg, unsigned int nBits, std::vector<int16_t>)
				throw (FEDException);
			
			void Parse(char *buf, int *Count, char **Word);
			void shuffle(char *a, char *b);
		};

	}
}

#endif

