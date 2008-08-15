/*****************************************************************************\
* $Id: VMEController.h,v 3.9 2008/08/15 08:35:51 paste Exp $
*
* $Log: VMEController.h,v $
* Revision 3.9  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#ifndef __VMECONTROLLER_H__
#define __VMECONTROLLER_H__

#include <iostream>
#include "JTAG_constants.h"

#include "JTAGDevice.h"
#include "EmuFEDLoggable.h"

namespace emu {

	namespace fed {

		class VMEModule;

		class VMEController: public EmuFEDLoggable, public JTAGDevice
		{
		public:
			VMEController(int Device, int Link);
			~VMEController();
			
			//enum ENDIAN {SWAP, NOSWAP};
			//enum {MAXLINE = 70000};
			
			
			inline int Device() {return Device_;}
			inline int Link() {return Link_;}
			inline int32_t getBHandle() { return BHandle_; }
			
			void setCrate(int number);
			
			/// if not current modules, it stops current and starts new
			/// this base routine sends a signal consisting of the
			/// university and slot
			// void start(VMEModule * module);
			void start(int slot);
			/// ends whatever module is current
			void end();
			
			/// JTAG stuff
			void devdo(DEVTYPE dev,int ncmd,const char *cmd,int nbuf,const char *inbuf,char *outbuf,int irdsnd);
			void scan(int reg,const char *snd,int cnt2,char *rcv,int ird);
			void RestoreIdle();
			void InitJTAG(int port);
			void CloseJTAG();
			void send_last();
			void RestoreIdle_reset();
			void scan_reset(int reg, const char *snd, int cnt2, char *rcv,int ird);
			void sleep_vme(const char *outbuf);   // in usecs (min 16 usec)
			void sleep_vme2(unsigned short int time); // time in usec
			void long_sleep_vme2(float time);   // time in usec
			void handshake_vme();
			void flush_vme();
			void vmeser(const char *cmd,const char *snd,char *rcv);
			void vmepara(const char *cmd,const char *snd,char *rcv);
			void dcc(const char *cmd,char *rcv);
			void vme_adc(int ichp,int ichn,char *rcv);
			void vme_controller(int irdwr,unsigned short int *ptr,unsigned short int *data,char *rcv);
			void CAEN_close(void);
			int CAEN_reset(void);
			void CAEN_err_reset(void);
			int CAEN_read(unsigned long Address,unsigned short int *data);
			int CAEN_write(unsigned long Address,unsigned short int *data);
			
			int udelay(long int itim);
			void sdly();
			void initDevice(int a);
			
			/// EPROM reprogramming (EXPERTS ONLY !)
			void epromload(char *design,enum DEVTYPE devnum,char *downfile,int writ,char *cbrdnum);
			void Parse(char *buf,int *Count,char **Word);
			
			inline int CAEN_err(void) { return caen_err; }
			
			//inline VMEModule* getCurrentModule() { return currentModule_; }
			
		private:
			int Device_;
			int Link_;
			int32_t BHandle_;
			VMEModule *currentModule_;
			//const ENDIAN endian_;
			int idevo_;
			int feuseo;
			int plev;
			int crateNumber;
			int caen_err;
		
			// Constants from the old vme_cmd.h
			int ife;
			
			unsigned long vmeadd; /* vme base address */
			unsigned long vmeadd_tmp;
			unsigned long add_i,add_d,add_dh,add_ds,add_dt,add_sw,add_sr,add_rst,add_r;
			unsigned long add_reset;
			unsigned long add_control_r;
			unsigned long add_vmepara;
			unsigned long add_vmeser;
			unsigned long add_dcc_r,add_dcc_w;
			unsigned long add_adcr,add_adcw,add_adcrbb,add_adcws,add_adcrs;
			
			unsigned long msk00;
			unsigned long msk01;
			unsigned long msk02;
			unsigned long msk03;
			unsigned long msk04;
			unsigned long msk05;
			unsigned long msk06;
			unsigned long msk07;
			unsigned long msk08;
			unsigned long msk09;
			unsigned long msk0d;
			unsigned long msk7f;
			unsigned long msk0f;
			
			unsigned long msk_clr;
			unsigned long msk_rst;
			unsigned long msk_i;
			unsigned long msk_d;
			unsigned long msk_dh;
			unsigned long msk_ds;
			unsigned long msk_dt;
			unsigned long msk_sw;
			unsigned long msk_sr;
			unsigned long msk_r;
			unsigned long msk_control_r;
			unsigned long msk_vmeser;
			unsigned long msk_vmepara;
			unsigned long msk_dcc_r;
			unsigned long msk_dcc_w;
			unsigned long msk_adcr;
			unsigned long msk_adcw;
			unsigned long msk_adcrbb;
			unsigned long msk_adcws;
			unsigned long msk_adcrs;

			int delay_type;
			//long OpenBHandle[4][4];

			inline int pows(int n, int m) { int ret = 1; for (int i=0; i<m; i++) ret *= n; return ret; }

		};

	}
}

#endif

