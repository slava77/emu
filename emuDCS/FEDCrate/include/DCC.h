/*****************************************************************************\
* $Id: DCC.h,v 3.22 2008/10/09 11:21:19 paste Exp $
*
* $Log: DCC.h,v $
* Revision 3.22  2008/10/09 11:21:19  paste
* Attempt to fix DCC MPROM load.  Added debugging for "Global SOAP death" bug.  Changed the debugging interpretation of certain DCC registers.  Added inline SVG to EmuFCrateManager page for future GUI use.
*
* Revision 3.21  2008/09/24 18:38:38  paste
* Completed new VME communication protocols.
*
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
		
			/** @param slot the slot of the board for VME addressing purposes. **/
			DCC(int slot);

			virtual ~DCC();

			/** @return the type of board this is, useful for dynamic casting.
			 *
			 * @deprecated Not actually used any more?
			 **/
			//virtual unsigned int boardType() const {return DCC_ENUM;}
		
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
		
			// EPROM reprogramming (EXPERTS ONLY !)
			/* void hdrst_main(void); */
			/* void hdrst_in(void); */
			/* void epromload(char *design, enum DEVTYPE devnum, char *downfile, int writ); */
		
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
			uint16_t readStatusHigh()
				throw (FEDException);

			/** @returns the low 16-bits of the DCC status register
			 *
			 * @note These 16 bits are actually independent of the value returned from
			 * readStatusHigh.  The name is purely historical.
			 **/
			uint16_t readStatusLow()
				throw (FEDException);

			/** @returns the input FIFOs (DDUs) that are being read in
			 * to the DCC as a binary number where a 1 in the nth bit means that
			 * FIFO n is being read in.
			 *
			 * @note There are only 10 fifos, but the first 11 bits are not ignored.
			 * 
			 * @sa getDDUSlotFromFIFO
			 **/
			uint16_t readFIFOInUse()
				throw (FEDException);

			/** Write a new value to the FIFOInUse register.
			 *
			 * @param value the new FIFOInUse register value to write.
			 *
			 * @note There are only 10 FIFOs, but only the high 5 bits of value are ignored.
			 *
			 * @sa getDDUSlotFromFIFO
			 **/
			void writeFIFOInUse(uint16_t value)
				throw (FEDException);

			/** @returns the current read-in or read-out data rate (in bytes/sec) of
			 * the given FIFO (DDU or S-Link).
			 *
			 * @param fifo is the FIFO from which to read the data rate.  fifo==0
			 * corresponds to S-Link 0, fifo==[1,5] correspond to the DDU input FIFOs
			 * 1-5, fifo==6 corresponds to S-Link 1, and fifo==[7,11] correspond to
			 * the DDU input FIFOs 6-10.
			 **/
			uint16_t readRate(unsigned int fifo)
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
			uint16_t readSoftwareSwitch()
				throw (FEDException);

			/** Set the omni-purpose software switch.
			 *
			 * @param value the value to which to set the software switch.
			 * 
			 * @sa readSoftwareSwitch()
			 **/
			void writeSoftwareSwitch(uint16_t value)
				throw (FEDException);

			/** @return the value of the FMM register. **/
			uint16_t readFMM()
				throw (FEDException);

			/** Write a custom value to the FMM register.
			 *
			 * @param value the FMM value to set.
			 *
			 * @note useful for TTS tests.
			 **/
			void writeFMM(uint16_t value)
				throw (FEDException);

			/** @return the last TTC command sent. **/
			uint16_t readTTCCommand()
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
			 * the commands resetBX() and resetEvents().
			 **/
			void writeTTCCommand(uint8_t value)
				throw (FEDException);

			/** Send the TTC command to reset BX values in the FED crate. **/
			void resetBX()
				throw (FEDException);

			/** Send the TTC command to reset event counters in the FED crate. **/
			void resetEvents()
				throw (FEDException);

			/** Set the fake L1A generation register.
			 *
			 * @param value the low 8 bits are used to set the number of L1As to generate,
			 * the high 8 bits are used to set the rate at which they are generated.
			 **/
			void writeFakeL1A(uint16_t value)
				throw (FEDException);

			/** @return the ID code from the given PROM chip.
			 *
			 * @param dev the device from which to read the ID code.
			 **/
			uint32_t readIDCode(enum DEVTYPE dev)
				throw (FEDException);

			/** @return the User code programmed into the given PROM chip.
			 *
			 * @param dev the device from which to read the User code.
			 **/
			uint32_t readUserCode(enum DEVTYPE dev)
				throw (FEDException);
				
			/** Resets an FPGA through the given PROM.
			*
			*	@param dev the PROM through which the signal should be sent.
			*
			*	@note The RESET device will reset the MCTRL FPGA.
			**/
			void resetPROM(enum DEVTYPE dev)
				throw (FEDException);

			
		protected:
			int fifoinuse_;
			int softsw_;

			// PGK New interface
			
			std::vector<uint16_t> readRegister(enum DEVTYPE dev, char myReg, unsigned int nBits)
				throw (FEDException);

			std::vector<uint16_t> writeRegister(enum DEVTYPE dev, char myReg, unsigned int nBits, std::vector<uint16_t>)
				throw (FEDException);
			
			/* void Parse(char *buf, int *Count, char **Word); */
			/* void shuffle(char *a, char *b); */
		};

	}
}

#endif

