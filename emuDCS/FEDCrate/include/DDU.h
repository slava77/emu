/*****************************************************************************\
* $Id: DDU.h,v 3.15 2008/08/15 08:35:50 paste Exp $
*
* $Log: DDU.h,v $
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

		public:
		
			DDU(int myCrate, int mySlot); // Deprecated
			DDU(int mySlot);
			virtual ~DDU();

			inline unsigned int getKillFiber() { return killfiber_; }

			/// from the BOARDTYPE enum
			virtual unsigned int boardType() const {return DDU_ENUM;}
			virtual void end();
			
			void configure();
		
		// DDU commands initialization/reset
			void ddu_init();
			void ddu_reset();
			void ddu_l1calonoff();
			void ddu_vmel1a();
		// DDU register control
			void ddu_shfttst(int tst);
			void ddu_lowfpgastat();
			void ddu_hifpgastat();
			unsigned int ddu_checkFIFOa();
			unsigned int ddu_checkFIFOb();
			unsigned int ddu_checkFIFOc();
			void ddu_rdfibererr();
			void ddu_rdfiberok();
			long unsigned int ddu_rdkillfiber();
			void ddu_loadkillfiber(long int regval);
			int ddu_rdcrcerr();
			void ddu_rdl1aerr();
			int ddu_rdxmiterr();
			void ddu_rdtimesterr();
			void ddu_rdtimeewerr();
			void ddu_rdtimeeaerr();
			int ddu_rddmberr();
			int ddu_rdtmberr();
			int ddu_rdlieerr();
			void ddu_rdliderr();
			void ddu_rdpaferr();
			int ddu_rdfferr();
			unsigned int ddu_rderareg();
			unsigned int ddu_rderbreg();
			unsigned int ddu_rdercreg();
			unsigned int ddu_InRDstat();
			int  ddu_InC_Hist();
			int  ddu_dmblive();
			int  ddu_pdmblive();
			int  ddu_rd_WarnMon();
			void ddu_rd_verr_cnt();
			void ddu_rd_cons_cnt();
			void ddu_fifo0verr_cnt();
			void ddu_fifo1verr_cnt();
			void ddu_earlyVerr_cnt();
			void ddu_verr23cnt();
			void ddu_verr55cnt();
			unsigned int ddu_rdostat();
			void ddu_rdempty();
			void ddu_rdstuckbuf();
			unsigned long int ddu_rdscaler();
			unsigned long int ddu_int_rdscaler();
			int ddu_rdalcterr();
			void ddu_loadbxorbit(int regval);
			int ddu_rdbxorbit();
			void ddu_lvl1onoff();
			unsigned int ddu_rd_boardID();
			unsigned long int ddu_fpgastat();
			void ddu_occmon();
			void ddu_fpgatrap();
			//  void ddu_trap_decode();
			void ddu_maxTimeCount();
			unsigned short int ddu_code0,ddu_code1,ddu_shift0;
		
			// PGK Failed attempt at simplified commands
		// 	void init()
		// 		throw (FEDException);
			void reset(enum DEVTYPE dt)
				throw (FEDException);
			void setNormal(enum DEVTYPE dt)
				throw (FEDException);
		
			unsigned long int readReg(enum DEVTYPE dt, char reg, const unsigned int nbits)
				throw (FEDException);
		// 	unsigned long int writeReg(enum DEVTYPE dt, char reg, unsigned long int value, const unsigned int nbits)
		// 		throw (FEDException);
		
			// PGK Failed attempt at simplified universal commands
			long int readL1Scaler(enum DEVTYPE dt)
				throw (FEDException);
			unsigned long int readFPGAStat(enum DEVTYPE dt)
				throw (FEDException);
		
			// PGK Failed attempt at simplified DDUFPGA commands
			void toggleL1Cal()
				throw (FEDException);
			void vmeL1A()
				throw (FEDException);
			int checkFIFO(int fifo)
				throw (FEDException);
			long int readKillFiber()
				throw (FEDException);
		// 	void writeKillFiber(long int killFiber)
		// 		throw (FEDException);
			int readCRCError()
				throw (FEDException);
			int readXmitError()
				throw (FEDException);
			int readDMBError()
				throw (FEDException);
			int readTMBError()
				throw (FEDException);
			int readLIEError()
				throw (FEDException);
			int readFFError()
				throw (FEDException);
			int readEBReg(int reg)
				throw (FEDException);
			int readInRDStat()
				throw (FEDException);
			int readInCHistory()
				throw (FEDException);
			int readDMBLive()
				throw (FEDException);
			int readPermDMBLive()
				throw (FEDException);
			int readWarnMon()
				throw (FEDException);
		// 	int readVoteErrorCount()
		// 		throw (FEDException);
		// 	int readConsVoteErrorCount()
		// 		throw (FEDException);
		// 	int readFIFOVoteErrorCount(int fifo)
		// 		throw (FEDException);
		// 	int readEarlyVoteErrorCount()
		// 		throw (FEDException);
		// 	int readVoteError23Count()
		// 		throw (FEDException);
		// 	int readVoteError55Count()
		// 		throw (FEDException);
			int readOutputStat()
				throw (FEDException);
			int readALCTError()
				throw (FEDException);
		// 	int writeBXOrbit(int BXOrbit)
		// 		throw (FEDException);
			int readBXOrbit()
				throw (FEDException);
			int readRUI()
				throw (FEDException);
		// 	unsigned long int readFPGALongStat()
		// 		throw (FEDException);
		// 	unsigned long int readOccupancyMon()
		// 		throw (FEDException);
			int readMaxTimeoutCount()
				throw (FEDException);
		
			// INFPGA register control
			void infpga_shfttst(enum DEVTYPE dv,int tst);
			void infpga_reset(enum DEVTYPE dv);
			unsigned long int infpga_rdscaler(enum DEVTYPE dv);
			int  infpga_rd1scaler(enum DEVTYPE dv);
			void infpga_lowstat(enum DEVTYPE dv);
			void infpga_histat(enum DEVTYPE dv);
			unsigned long int infpgastat(enum DEVTYPE dv);
			int infpga_CheckFiber(enum DEVTYPE dv);
			int infpga_int_CheckFiber(enum DEVTYPE dv);
			void infpga_DMBsync(enum DEVTYPE dv);
			void infpga_FIFOstatus(enum DEVTYPE dv);
			void infpga_FIFOfull(enum DEVTYPE dv);
			void infpga_RxErr(enum DEVTYPE dv);
			void infpga_Timeout(enum DEVTYPE dv);
			void infpga_XmitErr(enum DEVTYPE dv);
			int infpga_WrMemActive(enum DEVTYPE dv,int ifiber);
			int infpga_DMBwarn(enum DEVTYPE dv);
			int infpga_MemAvail(enum DEVTYPE dv);
			int infpga_Min_Mem(enum DEVTYPE dv);
			void infpga_LostErr(enum DEVTYPE dv);
			int  infpga_CcodeStat(enum DEVTYPE dv);
			void infpga_StatA(enum DEVTYPE dv);
			void infpga_StatB(enum DEVTYPE dv);
			void infpga_StatC(enum DEVTYPE dv);
			void infpga_FiberDiagA(enum DEVTYPE dv);
			void infpga_FiberDiagB(enum DEVTYPE dv);
			void infpga_trap(enum DEVTYPE dv);
			unsigned short int infpga_code0,infpga_code1,infpga_shift0;
			unsigned long int fpga_lcode[10];
		
			// PGK Failed attempt at simplified INFPGA commands
			long int readL1Scaler1(enum DEVTYPE dv)
				throw (FEDException);
			int checkFiber(enum DEVTYPE dv)
				throw (FEDException);
			int readDMBSync(enum DEVTYPE dv)
				throw (FEDException);
			int readFIFOStat(enum DEVTYPE dv)
				throw (FEDException);
			int readFIFOFull(enum DEVTYPE dv)
				throw (FEDException);
			int readRxError(enum DEVTYPE dv)
				throw (FEDException);
			int readTimeout(enum DEVTYPE dv)
				throw (FEDException);
			int readTxError(enum DEVTYPE dv)
				throw (FEDException);
			int readWriteMemoryActive(enum DEVTYPE dv,int iFiber)
				throw (FEDException);
			int readDMBWarning(enum DEVTYPE dv)
				throw (FEDException);
			int readMemoryAvailable(enum DEVTYPE dv)
				throw (FEDException);
			int readMinMemory(enum DEVTYPE dv)
				throw (FEDException);
			int readLostError(enum DEVTYPE dv)
				throw (FEDException);
			int  readCCodeStat(enum DEVTYPE dv)
				throw (FEDException);
		// 	int readINFPGAStatusReg(enum DEVTYPE dv, int i)
		// 		throw (FEDException);
			long unsigned int readFiberDiagnostics(enum DEVTYPE dv, int i)
				throw (FEDException);
		
			// DDU Status Decode
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
		
			// PGK Failed attempt at simplified ID/user codes
			unsigned long int readFPGAUserCode(enum DEVTYPE dt)
				throw (FEDException);
			unsigned long int readPROMUserCode(enum DEVTYPE dt)
				throw (FEDException);
		
			// DDU parallel
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
		
			// PGK Failed attempt at simplified VME parallel
			int readParallel(int command)
				throw (FEDException);
			void writeParallel(int command, int var)
				throw (FEDException);
		
			int readFMMBusy()
				throw (FEDException);
			int readFMMFullWarning()
				throw (FEDException);
			int readFMMLostSync()
				throw (FEDException);
			int readFMMError()
				throw (FEDException);
			int readFMMReg()
				throw (FEDException);
			void writeFMMReg(int val)
				throw (FEDException);
			int readCSCStat()
				throw (FEDException);
			int readSwitches()
				throw (FEDException);
			int readParallelStat()
				throw (FEDException);
			int readInputReg(int iReg)
				throw (FEDException);
			void writeInputReg(int val)
				throw (FEDException);
			int readFakeL1Reg()
				throw (FEDException);
			void writeFakeL1Reg(int val)
				throw (FEDException);
			int readGbEPrescale()
				throw (FEDException);
			void writeGbEPrescale(int val)
				throw (FEDException);
			int readTestReg(int iReg)
				throw (FEDException);
			int readBusyHistory()
				throw (FEDException);
			int readWarningHistory()
				throw (FEDException);
		
			// DDU serial
			int read_status();
			int read_int_page1();
			int read_page1();
			void write_page1();
			int read_page3();
			void write_page3();
			void read_page4();
			void write_page4();
			void read_page5();
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
		
			// PGK Failed attempt at simplified VME serial
			unsigned long int readSerial(int command, const unsigned int nbits)
				throw (FEDException);
			void writeSerial(int command)
				throw (FEDException);
		
			char readSerialStat()
				throw (FEDException);
			int readFlashKillFiber()
				throw (FEDException);
			void writeFlashKillFiber(int val)
				throw (FEDException);
			int readFlashBoardID()
				throw (FEDException);
			void writeFlashBoardID(int val)
				throw (FEDException);
		// 	unsigned long int readFlashInFIFOThresholds()
		// 		throw (FEDException);
		// 	void writeFlashInFIFOThresholds(unsigned long int val)
		// 		throw (FEDException);
			unsigned long int readFlashGbEFIFOThresholds()
				throw (FEDException);
			void writeFlashGbEFIFOThresholds(int val1, int val2, int val3)
				throw (FEDException);
			int readFlashRUI()
				throw (FEDException);
			void writeFlashRUI(int val)
				throw (FEDException);
		// 	unsigned long int readInFIFOThreshold(int iFifo)
		// 		throw (FEDException);
		// 	void forceLoadFIFOsFromFlash()
		// 		throw (FEDException);
		
			// Voltages and Thermometers
			float adcplus(int ichp,int ichn);
			float adcminus(int ichp,int ichn);
			float readthermx(int it);
			unsigned int readADC(int ireg, int ichn);
			void read_therm();
			void read_voltages();
			
			// Unpack characters to integers
			unsigned int unpack_ival();
			
			// EPROM reprogramming (EXPERTS ONLY !)
			void epromload(char *design,enum DEVTYPE devnum,char *downfile,int writ,char *cbrdnum);
			void epromload(char *design,enum DEVTYPE devnum,char *downfile,int writ,char *cbrdnum,int ipass); // for broadcast
			void Parse(char *buf,int *Count,char **Word);
			
			/// sends commands by name
			void executeCommand(std::string command);
			
			// unpacks rcvbuf from FPGA operations
			unsigned long int unpack_ibrd() const;
			unsigned int unpack_ival() const;
		
			/* PGK Chamber routines */
			std::vector<Chamber *> getChambers();
			Chamber *getChamber(unsigned int fiberNumber);
			void addChamber(Chamber *chamber, unsigned int fiberNumber);
			void setChambers(std::vector<Chamber *> chamberVector);
		
		private:
		
			std::vector<Chamber *> chamberVector_;
			int skip_vme_load_;
			int gbe_prescale_;
			unsigned int killfiber_;

		};

	}
}

#endif

