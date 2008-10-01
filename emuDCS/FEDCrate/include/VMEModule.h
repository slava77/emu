/*****************************************************************************\
* $Id: VMEModule.h,v 3.18 2008/10/01 07:49:38 paste Exp $
*
* $Log: VMEModule.h,v $
* Revision 3.18  2008/10/01 07:49:38  paste
* Removed busyloop waiting in favor of less accurate but more resource-friendly usleep.
*
* Revision 3.17  2008/09/29 08:36:26  paste
* Removed references to extinct JTAGDevice.h
*
* Revision 3.16  2008/09/24 18:38:38  paste
* Completed new VME communication protocols.
*
* Revision 3.15  2008/09/22 14:31:53  paste
* /tmp/cvsY7EjxV
*
* Revision 3.14  2008/09/19 16:53:51  paste
* Hybridized version of new and old software.  New VME read/write functions in place for all DCC communication, some DDU communication.  New XML files required.
*
* Revision 3.13  2008/09/07 22:25:35  paste
* Second attempt at updating the low-level communication routines to dodge common-buffer bugs.
*
* Revision 3.12  2008/09/03 17:52:58  paste
* Rebuilt the VMEController and VMEModule classes from the EMULIB_V6_4 tagged versions and backported important changes in attempt to fix "high-bits" bug.
*
* Revision 3.11  2008/09/02 08:39:52  paste
* Better handling and display of new features in the DDU firmware.
*
* Revision 3.10  2008/09/01 23:46:24  paste
* Trying to fix what I broke...
*
* Revision 3.9  2008/08/31 21:18:27  paste
* Moved buffers from VMEController class to VMEModule class for more rebust communication.
*
* Revision 3.8  2008/08/30 14:49:04  paste
* Attempts to make VME work under the new design model where VMEModules take over for the VMEController.
*
* Revision 3.7  2008/08/25 12:25:49  paste
* Major updates to VMEController/VMEModule handling of CAEN instructions.  Also, added version file for future RPMs.
*
* Revision 3.6  2008/08/19 14:51:02  paste
* Update to make VMEModules more independent of VMEControllers.
*
* Revision 3.5  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#ifndef __VMEMODULE_H__
#define __VMEMODULE_H__

#include <pthread.h> // For mutexes

#include "JTAG_constants.h"
#include "FEDException.h"

#include "EmuFEDLoggable.h"
#include "JTAGElement.h"

/* VMEModule is a virtual class for DCC and DDU classes.
 * A VMEModule should be apathetic to the controller and crate,
 * and although this breaks down at the program execution level (where
 * any given read/write from/to the DCC and DDU requires access of the
 * controller), it shouldn't break down at the object level.
 * Therefore, I removed or modified all public methods that referenced
 * the crate or controller.  Use the FEDCrate and VMEController objects
 * for those methods, not the DDU and DCC objects.
 */

namespace emu {

	namespace fed {
		
		class VMEController;
	
		struct JTAGElement;
		
		class VMEModule: public EmuFEDLoggable
		{
		public:
			//enum {MAXLINE = 70000};
			
			VMEModule(int mySlot);
			virtual ~VMEModule() {};
			inline const int slot() {return slot_;}

			/*
			// should automatically start().  Here's what you do if
			// you want to end() by hand
			void endDevice();

			
			virtual void start();
			virtual void end();
			
			enum BOARDTYPE { DDU_ENUM=0, DCC_ENUM };
			virtual unsigned int boardType() const = 0;
			
			void setController(VMEController *controller);
			*/
			// Phil's new commands
			inline void setBHandle(int16_t myHandle) { BHandle_ = myHandle; }

			/** Parses and loads a given .svf file into a given PROM.
			 *
			 * @param dev the PROM to which to load the parsed .svf file.
			 * @param fileName the name on the local disk of the .svf file.
			 * @param startString if set will cause the loader to ignore all instructions until the line after the one matching it.
			 * @param stopString if set will cause the loader to stop immidately if it is found in the current line being read.  The line will not be loaded.
			 **/
			void loadPROM(enum DEVTYPE dev, char *fileName, std::string startString = "", std::string stopString = "")
				throw (FEDException);

			void loadPROM(enum DEVTYPE dev, const char *fileName, std::string startString = "", std::string stopString = "")
				throw (FEDException) {
					return loadPROM(dev, (char *) fileName, startString, stopString);
				}

			void loadPROM(enum DEVTYPE dev, std::string fileName, std::string startString = "", std::string stopString = "")
				throw (FEDException) {
					return loadPROM(dev, fileName.c_str(), startString, stopString);
				}

			std::vector<uint16_t> jtagWrite(enum DEVTYPE dev, unsigned int nBits, std::vector<uint16_t> myData, bool noRead = false)
				throw(FEDException);
			std::vector<uint16_t> jtagRead(enum DEVTYPE dev, unsigned int nBits)
				throw(FEDException);
		
		protected:

			/*
			void devdo(DEVTYPE dev,int ncmd,const char *cmd,int nbuf,const char *inbuf,char *outbuf,int irdsnd);
			void scan(int reg,const char *snd,int cnt2,char *rcv,int ird);
			//void RestoreIdle();
			void InitJTAG(int port);
			void CloseJTAG();
			void send_last();
			//void RestoreIdle_reset();
			void  scan_reset(int reg, const char *snd, int cnt2, char *rcv,int ird);
			void  sleep_vme(const char *outbuf);   // in usecs (min 16 usec)
			//void  sleep_vme2(unsigned short int time); // time in usec
			//void  long_sleep_vme2(float time);   // time in usec
			void handshake_vme();
			void flush_vme();
			void vmeser(const char *cmd,const char *snd,char *rcv);
			void vmepara(const char *cmd,const char *snd,char *rcv);
			void dcc(const char *cmd,char *rcv);
			void vme_adc(int ichp,int ichn,char *rcv);
			//void vme_controller(int irdwr,unsigned short int *ptr,unsigned short int *data,char *rcv);
			//void CAEN_close(void);
			//int CAEN_reset(void);
			int CAEN_read(unsigned long Address,unsigned short int *data);
			int CAEN_write(unsigned long Address,unsigned short int *data);
		
			//void sdly();
			//void initDevice(int a);
			/// used for calls to do_vme
			//enum FCN { VME_READ=1, VME_WRITE=2 };
			//enum WRT { LATER, NOW };
			//int theSlot;
		
			/// required for DDU/DCC communications
			char sndbuf[4096];
			char rcvbuf[4096];
			char rcvbuf2[4096];
			char cmd[4096];
			*/

			// Phil's new commands.

			std::map<enum DEVTYPE, JTAGChain> JTAGMap;

			void commandCycle(enum DEVTYPE dev, uint16_t myCommand)
				throw (FEDException);
			
			uint16_t readVME(uint32_t myAddress)
				throw (FEDException);
			
			void writeVME(uint32_t myAddress, uint16_t myData)
				throw (FEDException);

			std::vector<uint16_t> readCycle(uint32_t myAddress, unsigned int nBits)
				throw(FEDException);

			void writeCycle(uint32_t myAddress, unsigned int nBits, std::vector<uint16_t> myData)
				throw(FEDException);
			
			/** @deprecated Uses usleep instead **/
			void bogoDelay(uint64_t time);

		private:
			int slot_;
			//int idevo_;
			
			//VMEController *controller_;

			/*
			inline int pows(int n, int m) { int ret = 1; for (int i=0; i<m; i++) ret *= n; return ret; }
			inline void udelay(long int itim) { for (long int j=0; j<itim; j++) for (long int i=0; i<200; i++); }
			*/
			
			// Phil's new commands.
			int16_t BHandle_;
			uint32_t vmeAddress_;

			// Each board will be able to mutex out the other boards from reading and writing
			pthread_mutex_t mutex_;
			
		};

	}
}

#endif

