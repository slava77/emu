/*
     S. Durkin      1/25/05

 */
#ifndef DCC_h
#define DCC_h

using namespace std;
#include <iostream>
#include <vector>
#include <string>
#include "VMEModule.h"
#include "JTAG_constants.h"

#include "FEDException.h"

class DCC: public VMEModule
{
public:
	friend class DCCParser;

	// standard routines
	/// construct with all the standard CFEBs and BuckeyeChips
	DCC(int, int);
	DCC(int slot);
	virtual ~DCC();
	/// from the BOARDTYPE enum
	virtual unsigned int boardType() const {return DCC_ENUM;}
	virtual void end();

	int fifoinuse_;
	int softsw_;

	void configure();

	// DCC commands
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

	// PGK Attempt at simplified DCC commands
	unsigned long int readReg(enum DEVTYPE dt, char reg)
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
	

	// EPROM reprogramming (EXPERTS ONLY !)
	void hdrst_main(void);
	void hdrst_in(void);
	void epromload(char *design, enum DEVTYPE devnum, char *downfile, int writ, char *cbrdnum);
	void Parse(char *buf, int *Count, char **Word);

	/// sends commands by name
	void executeCommand(string command);

	// PGK Simplified DCC commands
	/** Reset the crate through a TTC-override command.	**/
	void crateHardReset();
	/** Sync reset the crate through a TTC-override command. **/
	void crateSyncReset();
	/** Return the slot number of the DDU corresponding to the given input
	*	fifo
	**/
	unsigned int getDDUSlotFromFIFO(unsigned int fifo);

	// unpacks rcvbuf from FPGA operations
	unsigned long int unpack_ibrd() const;
	unsigned int unpack_ival() const;

};

#endif

