/*****************************************************************************\
* $Id: VMEController.h,v 3.11 2008/08/19 14:51:02 paste Exp $
*
* $Log: VMEController.h,v $
* Revision 3.11  2008/08/19 14:51:02  paste
* Update to make VMEModules more independent of VMEControllers.
*
* Revision 3.10  2008/08/15 16:14:50  paste
* Fixed threads (hopefully).
*
* Revision 3.9  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#ifndef __VMECONTROLLER_H__
#define __VMECONTROLLER_H__

#include <iostream>

#include "JTAG_constants.h"
#include "FEDException.h"

#include "JTAGDevice.h"
#include "EmuFEDLoggable.h"

namespace emu {

	namespace fed {

		class VMEModule;

		class VMEController: public EmuFEDLoggable, public JTAGDevice
		{
		public:
			VMEController(int Device, int Link)
				throw (FEDException);
			~VMEController();
			
			//enum ENDIAN {SWAP, NOSWAP};
			//enum {MAXLINE = 70000};
			
			
			inline int getDevice() {return Device_;}
			inline int getLink() {return Link_;}
			inline int32_t getBHandle() { return BHandle_; }
			
			/* void setCrate(int number); */
			
			/**	I am doing something smart here.  Instead of making the BHandles extern,
			*	I am going to have the original constructor set its own BHanlde to a
			*	crazy value in case of an error.  Thus, if the device is already open, I
			*	will be able to check the BHandle to figure that out, then simply replace
			*	the BHandle with the correct value once I know it.
			**/
			inline void setBHandle(int32_t BHandle) { BHandle_ = BHandle; }

			/// if not current modules, it stops current and starts new
			/// this base routine sends a signal consisting of the
			/// university and slot
			// void start(VMEModule * module);
			/* void start(int slot); */
			/// ends whatever module is current
			/* void end(); */
			
			/// JTAG stuff
			/* void devdo(DEVTYPE dev,int ncmd,const char *cmd,int nbuf,const char *inbuf,char *outbuf,int irdsnd); */
			/* void scan(int reg,const char *snd,int cnt2,char *rcv,int ird); */
			/* void RestoreIdle(); */
			/* void InitJTAG(int port); */
			/* void CloseJTAG(); */
			/* void send_last(); */
			/* void RestoreIdle_reset(); */
			/* void scan_reset(int reg, const char *snd, int cnt2, char *rcv,int ird); */
			/* void sleep_vme(const char *outbuf);   // in usecs (min 16 usec) */
			/* void sleep_vme2(unsigned short int time); // time in usec */
			/* void long_sleep_vme2(float time);   // time in usec */
			/* void handshake_vme(); */
			/* void flush_vme(); */
			/* void vmeser(const char *cmd,const char *snd,char *rcv); */
			/* void vmepara(const char *cmd,const char *snd,char *rcv); */
			/* void dcc(const char *cmd,char *rcv); */
			/* void vme_adc(int ichp,int ichn,char *rcv); */
			/*void vme_controller(int irdwr,unsigned short int *ptr,unsigned short int *data,char *rcv); */
			/* int CAEN_read(unsigned long Address,unsigned short int *data); */
			/* int CAEN_write(unsigned long Address,unsigned short int *data); */
			
			// Keep me
			/* void CAEN_close(); */
			/* int CAEN_reset(void); */
			/* void CAEN_err_reset(void); */
			
			/* int udelay(long int itim); */
			/* void sdly(); */
			/* void initDevice(int a); */
			
			// EPROM reprogramming (EXPERTS ONLY !)
			// void epromload(char *design,enum DEVTYPE devnum,char *downfile,int writ,char *cbrdnum);
			// void Parse(char *buf,int *Count,char **Word);
			
			// inline int CAEN_err(void) { return caen_err; }
			
			//inline VMEModule* getCurrentModule() { return currentModule_; }
			
		private:
			
			int Device_;
			int Link_;
			int32_t BHandle_;
			/* VMEModule *currentModule_; */
			//const ENDIAN endian_;
			/* int idevo_; */
			/* int feuseo; */
			/* int plev; */
			/* int crateNumber; */
			/* int caen_err_; */

		};

	}
}

#endif

