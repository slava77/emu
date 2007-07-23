//----------------------------------------------------------------------
// $Id: VMEController.h,v 3.1 2007/07/23 05:02:23 gilmore Exp $
// $Log: VMEController.h,v $
// Revision 3.1  2007/07/23 05:02:23  gilmore
// major structural chages to improve multi-crate functionality
//
// Revision 3.0  2006/07/20 21:16:10  geurts
// *** empty log message ***
//
// Revision 1.3  2006/01/27 15:57:34  gilmore
// *** empty log message ***
//
// Revision 1.14  2004/07/22 18:52:38  tfcvs
// added accessor functions for DCS integration
//
//
//----------------------------------------------------------------------
#ifndef VMEController_h
#define VMEController_h

using namespace std;
#include <vector>
#include <iostream>
#include "JTAG_constants.h"
#include <string>

#include "VMEModule.h"
#include "Crate.h"
#include <cmath>
#include <string>
#include <stdio.h>
#include <iostream>
#include <unistd.h> // read and write
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <unistd.h>

#include <pthread.h>
#include "CAENVMElib.h"
#include "CAENVMEtypes.h"
#include "vmeIRQ.h"
#include "IRQThread.h"
#include <log4cplus/logger.h>
#include <xdaq/Application.h>

class VMEController {
public:
	VMEController(int Device, int Link);
	~VMEController();
	
	enum ENDIAN {SWAP, NOSWAP};
	enum {MAXLINE = 70000};
	
	
	int Device() const {return Device_;}
	int Link() const {return Link_;}
	
	void setCrate(int number);
	
	/// if not current modules, it stops current and starts new
	/// this base routine sends a signal consisting of the
	/// university and slot
	void start(VMEModule * module);
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
	void  scan_reset(int reg, const char *snd, int cnt2, char *rcv,int ird);
	void  sleep_vme(const char *outbuf);   // in usecs (min 16 usec)
	void  sleep_vme2(unsigned short int time); // time in usec
	void  long_sleep_vme2(float time);   // time in usec
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
	
	/// IRQ VME Interrupts
	IRQThread* thread() { return myThread; }
	int start_thread_on_init;
	void start_thread(long unsigned int runnumber);
	void end_thread();
	void kill_thread();
	bool thread_started() { return is_thread_started; }
	
	int udelay(long int itim);
	void sdly();
	void initDevice(int a);
	
	/// EPROM reprogramming (EXPERTS ONLY !)
	void epromload(char *design,enum DEVTYPE devnum,char *downfile,int writ,char *cbrdnum);
	void Parse(char *buf,int *Count,char **Word);
	long int theBHandle;
	inline int CAEN_err(void) { return caen_err; }
	
	VMEModule* getTheCurrentModule();
	
private:
	int Device_;
	int Link_;
	VMEModule * theCurrentModule;
	const ENDIAN indian;
	int idevo;
	int feuseo;
	int plev;
	int crateNumber;
	int caen_err;
	IRQThread* myThread;
	bool is_thread_started;
};

#endif

