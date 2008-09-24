/*****************************************************************************\
* $Id: DDU.h,v 3.23 2008/09/24 18:38:38 paste Exp $
*
* $Log: DDU.h,v $
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

		class DDU: public VMEModule
		{
			friend class DDUParser;
			friend class FEDCrateParser;

		public:
		
			DDU(int mySlot);
			virtual ~DDU();

			inline unsigned int getKillFiber() { return killfiber_; }
			inline unsigned int getGbEPrescale() { return gbe_prescale_; }

			/// from the BOARDTYPE enum
			//virtual inline unsigned int boardType() const {return DDU_ENUM;}
			/* virtual void end(); */
			
			void configure();
		
		// DDU commands initialization/reset
			/* void ddu_init(); */
			/* void ddu_reset(); */
			/* void ddu_l1calonoff(); */
			/* void ddu_vmel1a(); */
		// DDU register control
			/* void ddu_shfttst(int tst); */
			/* void ddu_lowfpgastat(); */
			/* void ddu_hifpgastat(); */
			/* unsigned int ddu_checkFIFOa(); */
			/* unsigned int ddu_checkFIFOb(); */
			/* unsigned int ddu_checkFIFOc(); */
			/* void ddu_rdfibererr(); */
			/* void ddu_rdfiberok(); */
			/* long unsigned int ddu_rdkillfiber(); */
			/* void ddu_loadkillfiber(long int regval); */
			/* int ddu_rdcrcerr(); */
			/* void ddu_rdl1aerr(); */
			/* int ddu_rdxmiterr(); */
			/* void ddu_rdtimesterr(); */
			/* void ddu_rdtimeewerr(); */
			/* void ddu_rdtimeeaerr(); */
			/* int ddu_rddmberr(); */
			/* int ddu_rdtmberr(); */
			/* int ddu_rdlieerr(); */
			/* void ddu_rdliderr(); */
			/* void ddu_rdpaferr(); */
			/* int ddu_rdfferr(); */
			/* unsigned int ddu_rderareg(); */
			/* unsigned int ddu_rderbreg(); */
			/* unsigned int ddu_rdercreg(); */
			/* unsigned int ddu_InRDstat(); */
			/* unsigned long int  ddu_InC_Hist(); */
			/* int  ddu_dmblive(); */
			/* int  ddu_pdmblive(); */
			/* int  ddu_rd_WarnMon(); */
			/* void ddu_rd_verr_cnt(); */
			/* void ddu_rd_cons_cnt(); */
			/* void ddu_fifo0verr_cnt(); */
			/* void ddu_fifo1verr_cnt(); */
			/* void ddu_earlyVerr_cnt(); */
			/* void ddu_verr23cnt(); */
			/* void ddu_verr55cnt(); */
			/* unsigned int ddu_rdostat(); */
			/* void ddu_rdempty(); */
			/* void ddu_rdstuckbuf(); */
			/* unsigned long int ddu_rdscaler(); */
			/* unsigned long int ddu_int_rdscaler(); */
			/* int ddu_rdalcterr(); */
			/* void ddu_loadbxorbit(int regval); */
			/* int ddu_rdbxorbit(); */
			/* void ddu_lvl1onoff(); */
			/* unsigned int ddu_rd_boardID(); */
			/* unsigned long int ddu_fpgastat(); */
			/* std::vector<unsigned long int> ddu_occmon(); */
			/* std::vector<unsigned long int> ddu_fpgatrap(); */
			//  void ddu_trap_decode();
			/* void ddu_maxTimeCount(); */
			//unsigned short int ddu_code0,ddu_code1,ddu_shift0;
		

		
			// PGK Failed attempt at simplified DDUFPGA commands

		
			// INFPGA register control
			/* void infpga_shfttst(enum DEVTYPE dv,int tst); */
			/* void infpga_reset(enum DEVTYPE dv); */
			/* unsigned long int infpga_rdscaler(enum DEVTYPE dv); */
			/* unsigned long int infpga_rd1scaler(enum DEVTYPE dv); */
			/* void infpga_lowstat(enum DEVTYPE dv); */
			/* void infpga_histat(enum DEVTYPE dv); */
			/* unsigned long int infpgastat(enum DEVTYPE dv); */
			/* int infpga_CheckFiber(enum DEVTYPE dv); */
			/* int infpga_int_CheckFiber(enum DEVTYPE dv); */
			/* void infpga_DMBsync(enum DEVTYPE dv); */
			/* void infpga_FIFOstatus(enum DEVTYPE dv); */
			/* void infpga_FIFOfull(enum DEVTYPE dv); */
			/* void infpga_RxErr(enum DEVTYPE dv); */
			/* void infpga_Timeout(enum DEVTYPE dv); */
			/* void infpga_XmitErr(enum DEVTYPE dv); */
			/* int infpga_WrMemActive(enum DEVTYPE dv,int ifiber); */
			/* int infpga_DMBwarn(enum DEVTYPE dv); */
			/* unsigned long int infpga_MemAvail(enum DEVTYPE dv); */
			/* unsigned long int infpga_Min_Mem(enum DEVTYPE dv); */
			/* void infpga_LostErr(enum DEVTYPE dv); */
			/* unsigned long int infpga_CcodeStat(enum DEVTYPE dv); */
			/* void infpga_StatA(enum DEVTYPE dv); */
			/* void infpga_StatB(enum DEVTYPE dv); */
			/* void infpga_StatC(enum DEVTYPE dv); */
			/* void infpga_FiberDiagA(enum DEVTYPE dv); */
			/* void infpga_FiberDiagB(enum DEVTYPE dv); */
			/* std::vector<unsigned long int> infpga_trap(enum DEVTYPE dv); */
			//unsigned short int infpga_code0,infpga_code1,infpga_shift0;
			//unsigned long int fpga_lcode[10];
		
			// DDU Status Decode
			/*
			void ddu_status_decode(int long code);
			void ddu_ostatus_decode(int long code);
			void ddu_era_decode(int long code);
			void ddu_erb_decode(int long code);
			void ddu_erc_decode(int long code);
			void ddu5status_decode(int long code);
			void ddu5ostatus_decode(int long code);
			void ddu5begin_decode(int long code);
			void ddu5vmestat_decode(int long code);
			void in_Ccode_decode(int long code);
			void in_stat_decode(int long code);
			
			// DDU FPGA id/user codes
			unsigned long int ddufpga_idcode();
			unsigned long int infpga_idcode0();
			unsigned long int infpga_idcode1();
			unsigned long int ddufpga_usercode();
			unsigned long int infpga_usercode0();
			unsigned long int infpga_usercode1();
			unsigned long int inprom_idcode0();
			unsigned long int inprom_idcode1();
			unsigned long int vmeprom_idcode();
			unsigned long int dduprom_idcode0();
			unsigned long int dduprom_idcode1();
			
			unsigned long int inprom_usercode0();
			unsigned long int inprom_usercode1();
			unsigned long int vmeprom_usercode();
			
			unsigned long int dduprom_usercode0();
			unsigned long int dduprom_usercode1();
			void all_chip_info();
			*/

			// DDU parallel
			/*
			unsigned short int vmepara_busy();
			unsigned short int vmepara_fullwarn();
			unsigned short int vmepara_CSCstat();
			unsigned short int vmepara_lostsync();
			unsigned short int vmepara_error();
			unsigned short int  vmepara_switch();
			unsigned short int vmepara_status();
			unsigned short int vmepara_rd_inreg0();
			unsigned short int vmepara_rd_inreg1();
			unsigned short int vmepara_rd_inreg2();
			void vmepara_wr_inreg(unsigned short int par_val);
			void  vmepara_wr_fmmreg(unsigned short int par_val);
			unsigned short int  vmepara_rd_fmmreg();
			void vmepara_wr_fakel1reg(unsigned short int par_val);
			unsigned short int vmepara_rd_fakel1reg();
			void vmepara_wr_GbEprescale(unsigned short int par_val);
			unsigned short int vmepara_rd_GbEprescale();
			unsigned short int vmepara_rd_testreg0();
			unsigned short int vmepara_rd_testreg1();
			unsigned short int vmepara_rd_testreg2();
			unsigned short int vmepara_rd_testreg3();
			unsigned short int vmepara_rd_testreg4();
			unsigned short int vmepara_busyhist();
			unsigned short int vmepara_warnhist();
			*/

		
			// DDU serial
			/*
			int read_status();
			int read_int_page1();
			int read_page1();
			void write_page1();
			int read_page3();
			void write_page3();
			void read_page4();
			void write_page4();
			std::vector<int> read_page5();
			void write_page5();
			int read_page7();
			void write_page7();
			void read_vmesd0();
			void read_vmesd1();
			void read_vmesd2();
			void read_vmesd3();
			void write_vmesdF();
			char snd_serial[6];
			char rcv_serial[6];
			*/

			// Voltages and Thermometers
			/*
			float adcplus(int ichp,int ichn);
			float adcminus(int ichp,int ichn);
			float readthermx(int it);
			unsigned int readADC(int ireg, int ichn);
			*/
			/*
			void read_therm();
			void read_voltages();
			*/

			// Unpack characters to integers
			/* unsigned int unpack_ival(); */
			
			// EPROM reprogramming (EXPERTS ONLY !)
			/*
			void epromload(char *design,enum DEVTYPE devnum,char *downfile,int writ,char *cbrdnum);
			void epromload(char *design,enum DEVTYPE devnum,char *downfile,int writ,char *cbrdnum,int ipass); // for broadcast
			void Parse(char *buf,int *Count,char **Word);
			*/
			
			// PGK Chamber routines
			std::vector<Chamber *> getChambers();
			Chamber *getChamber(unsigned int fiberNumber);
			void addChamber(Chamber *chamber, unsigned int fiberNumber);
			void setChambers(std::vector<Chamber *> chamberVector);

			// PGK New interface
			// Read VME Parallel registers

			uint16_t readFMM()
				throw (FEDException);

			uint16_t readCSCStatus()
				throw (FEDException);

			uint16_t readFMMBusy()
				throw (FEDException);

			uint16_t readFMMFullWarning()
				throw (FEDException);

			uint16_t readFMMLostSync()
				throw (FEDException);

			uint16_t readFMMError()
				throw (FEDException);

			uint16_t readSwitches()
				throw (FEDException);
			
			uint16_t readParallelStatus()
				throw (FEDException);
			
			uint16_t readInputRegister(uint8_t iReg)
				throw (FEDException);

			uint16_t readFakeL1()
				throw (FEDException);

			uint16_t readGbEPrescale()
				throw (FEDException);

			uint16_t readTestRegister(uint8_t iReg)
				throw (FEDException);
			
			uint16_t readBusyHistory()
				throw (FEDException);
			
			uint16_t readWarningHistory()
				throw (FEDException);

			// Write VME Parallel registers

			void writeFMM(uint16_t value)
				throw (FEDException);

			void writeFakeL1(uint16_t value)
				throw (FEDException);
			
			void writeGbEPrescale(uint8_t value)
				throw (FEDException);
			
			void writeInputRegister(uint16_t value)
				throw (FEDException);

			// Read VME Serial/Flash registers
			
			uint8_t readSerialStatus()
				throw (FEDException);

			uint16_t readFlashKillFiber()
				throw (FEDException);

			uint16_t readFlashBoardID()
				throw (FEDException);

			uint16_t readFlashRUI()
				throw (FEDException);

			std::vector<uint16_t> readFlashGbEFIFOThresholds()
				throw (FEDException);

			// Write VME Serial/Flash registers

			void writeFlashKillFiber(uint16_t value)
				throw (FEDException);
			
			void writeFlashBoardID(uint16_t value)
				throw (FEDException);

			void writeFlashRUI(uint16_t value)
				throw (FEDException);

			void writeFlashGbEFIFOThresholds(std::vector<uint16_t> values)
				throw (FEDException);

			// Read SADC registers

			float readTemperature(uint8_t sensor)
				throw (FEDException);

			float readVoltage(uint8_t sensor)
				throw (FEDException);

			// Read DDUFPGA JTAG registers

			uint16_t readOutputStatus()
				throw (FEDException);

			uint16_t readFIFOStatus(uint8_t fifo)
				throw (FEDException);

			uint16_t readFFError()
				throw (FEDException);

			uint16_t readCRCError()
				throw (FEDException);

			uint16_t readXmitError()
				throw (FEDException);

			uint32_t readKillFiber()
				throw (FEDException);

			uint16_t readDMBError()
				throw (FEDException);

			uint16_t readTMBError()
				throw (FEDException);

			uint16_t readALCTError()
				throw (FEDException);

			uint16_t readLIEError()
				throw (FEDException);

			uint16_t readInRDStat()
				throw (FEDException);

			uint16_t readInCHistory()
				throw (FEDException);

			uint16_t readEBRegister(uint8_t reg)
				throw (FEDException);

			uint16_t readDMBLive()
				throw (FEDException);

			uint16_t readDMBLiveAtFirstEvent()
				throw (FEDException);

			uint16_t readWarningMonitor()
				throw (FEDException);

			uint16_t readMaxTimeoutCount()
				throw (FEDException);

			uint16_t readBXOrbit()
				throw (FEDException);

			void toggleL1Calibration()
				throw (FEDException);

			uint16_t readRUI()
				throw (FEDException);

			void sendFakeL1A()
				throw (FEDException);

			std::vector<uint32_t> readOccupancyMonitor()
				throw (FEDException);

			uint16_t readAdvancedFiberErrors()
				throw (FEDException);

			// Write DDUFPGA JTAG registers

			void writeKillFiber(uint32_t value)
				throw (FEDException);

			void writeBXOrbit(uint16_t value)
				throw (FEDException);

			// Read INFPGA JTAG registers

			uint32_t readL1Scaler1(enum DEVTYPE dev)
				throw (FEDException);
			
			uint16_t readFiberStatus(enum DEVTYPE dev)
				throw (FEDException);
			
			uint16_t readDMBSync(enum DEVTYPE dev)
				throw (FEDException);
			
			uint16_t readFIFOStatus(enum DEVTYPE dev)
				throw (FEDException);
			
			uint16_t readFIFOFull(enum DEVTYPE dev)
				throw (FEDException);
			
			uint16_t readRxError(enum DEVTYPE dev)
				throw (FEDException);
			
			uint16_t readTimeout(enum DEVTYPE dev)
				throw (FEDException);
			
			uint16_t readTxError(enum DEVTYPE dev)
				throw (FEDException);
			
			uint16_t readActiveWriteMemory(enum DEVTYPE dev,uint8_t iFiber)
				throw (FEDException);
			
			uint16_t readAvailableMemory(enum DEVTYPE dev)
				throw (FEDException);
			
			uint16_t readMinMemory(enum DEVTYPE dev)
				throw (FEDException);
			
			uint16_t readLostError(enum DEVTYPE dev)
				throw (FEDException);
			
			uint16_t  readCCodeStatus(enum DEVTYPE dev)
				throw (FEDException);

			uint16_t readDMBWarning(enum DEVTYPE dev)
				throw (FEDException);
			
			uint32_t readFiberDiagnostics(enum DEVTYPE dev, uint8_t iDiagnostic)
				throw (FEDException);

			// User and ID code reading

			uint32_t readUserCode(enum DEVTYPE dev)
				throw (FEDException);

			uint32_t readIDCode(enum DEVTYPE dev)
				throw (FEDException);

			// Reset FPGAs

			void resetFPGA(enum DEVTYPE dev)
				throw (FEDException);

			// Read Universal FPGA JTAG registers

			uint32_t readFPGAStatus(enum DEVTYPE dev)
				throw (FEDException);

			uint32_t readL1Scaler(enum DEVTYPE dev)
				throw (FEDException);

			std::vector<uint16_t> readDebugTrap(enum DEVTYPE dev)
				throw (FEDException);
			
		protected:
		
			std::vector<Chamber *> chamberVector_;
			int gbe_prescale_;
			unsigned long int killfiber_;

			// PGK New interface
			std::vector<uint16_t> readRegister(enum DEVTYPE dev, int myReg, unsigned int nBits)
				throw (FEDException);
			
			std::vector<uint16_t> writeRegister(enum DEVTYPE dev, int myReg, unsigned int nBits, std::vector<uint16_t>)
				throw (FEDException);
			
		};

	}
}

#endif

