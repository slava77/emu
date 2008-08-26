/*****************************************************************************\
* $Id: DCC.h,v 3.18 2008/08/26 13:40:08 paste Exp $
*
* $Log: DCC.h,v $
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

			/** @return the type of board this is, useful for dynamic casting. **/
			virtual unsigned int boardType() const {return DCC_ENUM;}
		
			/** @return the FIFOInUse parameter. **/
			inline int getFIFOInUse() { return fifoinuse_; }

			/** Configure the DCC by loading the FIFOInUse and SoftwareSwitch variables to
			*	the PROMs.
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
		
			// PGK Attempt at simplified DCC commands
			/** A general function to read from a given register via JTAG.
			*	
			*	@param dt the JTAG device from which to read.
			*	@param reg the register from which to read.
			*	@param nBits the number of bits to read back (?).  Not used.
			**/
			unsigned long int readReg(enum DEVTYPE dt, char reg, unsigned int nBits = 4)
				throw (FEDException);
			
			/** A general function to write to a given register via JTAG.
			*	@param dt the JTAG device to which the data will be written.
			*	@param reg the register to which the data will be written.
			*	@param value the value to write.
			**/
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
		
			/** @deprecated **/
			void executeCommand(std::string command);
		
			// PGK Simplified DCC commands
			/** Reset the crate through a TTC-override command. **/
			void crateHardReset();
			
			/** Sync reset the crate through a TTC-override command. **/
			void crateSyncReset();
			
			/** Return the slot number of the DDU corresponding to the given input
			*	fifo
			**/
			unsigned int getDDUSlotFromFIFO(unsigned int fifo);
		
		private:
			int fifoinuse_;
			int softsw_;

			void Parse(char *buf, int *Count, char **Word);
			void shuffle(char *a, char *b);
		};

	}
}

#endif

