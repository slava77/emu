/*****************************************************************************\
* $Id: DCC.cc,v 3.23 2008/09/19 16:53:52 paste Exp $
*
* $Log: DCC.cc,v $
* Revision 3.23  2008/09/19 16:53:52  paste
* Hybridized version of new and old software.  New VME read/write functions in place for all DCC communication, some DDU communication.  New XML files required.
*
* Revision 3.22  2008/09/07 22:25:36  paste
* Second attempt at updating the low-level communication routines to dodge common-buffer bugs.
*
* Revision 3.21  2008/09/03 17:52:58  paste
* Rebuilt the VMEController and VMEModule classes from the EMULIB_V6_4 tagged versions and backported important changes in attempt to fix "high-bits" bug.
*
* Revision 3.20  2008/09/01 23:46:24  paste
* Trying to fix what I broke...
*
* Revision 3.19  2008/08/31 21:18:27  paste
* Moved buffers from VMEController class to VMEModule class for more rebust communication.
*
* Revision 3.18  2008/08/25 12:25:49  paste
* Major updates to VMEController/VMEModule handling of CAEN instructions.  Also, added version file for future RPMs.
*
* Revision 3.17  2008/08/19 14:51:02  paste
* Update to make VMEModules more independent of VMEControllers.
*
* Revision 3.16  2008/08/15 16:14:51  paste
* Fixed threads (hopefully).
*
* Revision 3.15  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#include "DCC.h"

#include <iostream>
#include <vector>
//#include <fstream>
#include <stdio.h>
#include <cmath>
//#include <unistd.h>

#include <sys/time.h> // POSIX gettimeofday routine for microsecond timers

#include "JTAGElement.h"

emu::fed::DCC::DCC(int crate, int slot) :
		VMEModule(slot),
		fifoinuse_(0x3fe),
		softsw_(0)
{
	std::cerr << "Deprecated constructor.  Use DCC(int) instead of DCC(int,int)." << std::endl;
}

emu::fed::DCC::DCC(int slot) :
		VMEModule(slot),
		fifoinuse_(0x3fe),
		softsw_(0)
{
	// Build the JTAG chains

	JTAGElement *elementMPROM = new JTAGElement("MPROM", 2, MPROM_BYPASS_L | (MPROM_BYPASS_H << 8), 16, 0x00002000, false, NONE);
	JTAGMap[MPROM] = elementMPROM;
	
	JTAGElement *elementINPROM = new JTAGElement("INPROM", 3, PROM_BYPASS, 8, 0x00003000, false, NONE);
	JTAGMap[INPROM] = elementINPROM;

	JTAGElement *elementINCTRL1 = new JTAGElement("INCTRL1", 4, PROM_BYPASS, 8, 0x00004000, false, NONE);
	JTAGMap[INCTRL1] = elementINCTRL1;

	JTAGElement *elementINCTRL2 = new JTAGElement("INCTRL2", 4, PROM_BYPASS, 8, 0x00004000, false, INCTRL1);
	JTAGMap[INCTRL2] = elementINCTRL2;

	JTAGElement *elementINCTRL3 = new JTAGElement("INCTRL3", 4, PROM_BYPASS, 8, 0x00004000, false, INCTRL2);
	JTAGMap[INCTRL3] = elementINCTRL3;

	JTAGElement *elementINCTRL4 = new JTAGElement("INCTRL4", 4, PROM_BYPASS, 8, 0x00004000, false, INCTRL3);
	JTAGMap[INCTRL4] = elementINCTRL4;

	JTAGElement *elementINCTRL5 = new JTAGElement("INCTRL5", 4, PROM_BYPASS, 8, 0x00004000, false, INCTRL4);
	JTAGMap[INCTRL5] = elementINCTRL5;

	JTAGElement *elementMCTRL = new JTAGElement("MCTRL", 11, PROM_BYPASS, 10, 0x00000000, true, NONE);
	JTAGMap[MCTRL] = elementMCTRL;

	JTAGElement *elementRESET1 = new JTAGElement("RESET1", 12, PROM_BYPASS, 8, 0x0000fffe, false, NONE);
	JTAGMap[RESET1] = elementRESET1;

	JTAGElement *elementRESET2 = new JTAGElement("RESET2", 12, PROM_BYPASS, 8, 0x0000fffe, false, RESET1);
	JTAGMap[RESET2] = elementRESET2;
	
}


emu::fed::DCC::~DCC()
{
	// std::cout << "Killing DCC" << std::endl;
}

/*
void emu::fed::DCC::end()
{
	//  std::cout << "calling emu::fed::DCC::end" << std::endl;
	//send_last();
	//VMEModule::end();
}
*/

void emu::fed::DCC::configure()
{

	//printf(" *********************** DCC configure is called \n");
	//printf(" DCC slot %d fifoinuse %d \n",slot(),fifoinuse_);
	if (slot() < 21) {
		writeFIFOInUseAdvanced(fifoinuse_);
		writeSoftwareSwitchAdvanced(softsw_);
	}
}


/*
unsigned long int  emu::fed::DCC::inprom_userid()
{
	enum DEVTYPE dv;
	//printf(" inpromuser entered \n");
	dv = INPROM;
	cmd[0] = PROM_USERCODE;
	sndbuf[0] = 0xFF;
	sndbuf[1] = 0xFF;
	sndbuf[2] = 0xFF;
	sndbuf[3] = 0xFF;
	sndbuf[4] = 0xFF;
	devdo(dv, 8, cmd, 32, sndbuf, rcvbuf, 1);
	printf(" The PROM Chip USER CODE is %02x%02x%02x%02x \n", 0xff&rcvbuf[3], 0xff&rcvbuf[2], 0xff&rcvbuf[1], 0xff&rcvbuf[0]);
	unsigned long int ibrd = 0x00000000;
	ibrd = (rcvbuf[0] & 0xff) | ((rcvbuf[1] & 0xff) << 8) | ((rcvbuf[2] & 0xff) << 16) | ((rcvbuf[3] & 0xff) << 24) | ibrd;
	cmd[0] = PROM_BYPASS;
	sndbuf[0] = 0;
	devdo(dv, 8, cmd, 0, sndbuf, rcvbuf, 0);
	return ibrd;
}
*/


/*
unsigned long int  emu::fed::DCC::mprom_userid()
{
	enum DEVTYPE dv;
	dv = MPROM;
	cmd[0] = MPROM_USERCODE_L;
	cmd[1] = MPROM_USERCODE_H;
	sndbuf[0] = 0xFF;
	sndbuf[1] = 0xFF;
	sndbuf[2] = 0xFF;
	sndbuf[3] = 0xFF;
	sndbuf[4] = 0xFF;
	devdo(dv, 16, cmd, 32, sndbuf, rcvbuf, 1);
	printf(" The MPROM Chip USER CODE is %02x%02x%02x%02x \n", 0xff&rcvbuf[3], 0xff&rcvbuf[2], 0xff&rcvbuf[1], 0xff&rcvbuf[0]);
	unsigned long int ibrd = 0x00000000;
	ibrd = (rcvbuf[0] & 0xff) | ((rcvbuf[1] & 0xff) << 8) | ((rcvbuf[2] & 0xff) << 16) | ((rcvbuf[3] & 0xff) << 24) | ibrd;
	cmd[0] = MPROM_BYPASS_L;
	cmd[1] = MPROM_BYPASS_H;
	sndbuf[0] = 0;
	devdo(dv, 16, cmd, 0, sndbuf, rcvbuf, 0);
	return ibrd;
}
*/

/*
unsigned long int  emu::fed::DCC::inprom_chipid()
{
	enum DEVTYPE dv;
	printf(" inpromuser entered \n");
	dv = INPROM;
	cmd[0] = PROM_IDCODE;
	sndbuf[0] = 0xFF;
	sndbuf[1] = 0xFF;
	sndbuf[2] = 0xFF;
	sndbuf[3] = 0xFF;
	sndbuf[4] = 0xFF;
	devdo(dv, 8, cmd, 32, sndbuf, rcvbuf, 1);
	//  printf(" The PROM Chip USER CODE is %02x%02x%02x%02x \n",0xff&rcvbuf[3],0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
	cmd[0] = PROM_BYPASS;
	sndbuf[0] = 0;
	devdo(dv, 8, cmd, 0, sndbuf, rcvbuf, 0);
	unsigned long int ibrd = 0x00000000;
	ibrd = (rcvbuf[0] & 0xff) | ((rcvbuf[1] & 0xff) << 8) | ((rcvbuf[2] & 0xff) << 16) | ((rcvbuf[3] & 0xff) << 24) | ibrd;
	return ibrd;

}
*/

/*
unsigned long int  emu::fed::DCC::mprom_chipid()
{
	enum DEVTYPE dv;
	dv = MPROM;
	cmd[0] = MPROM_IDCODE_L;
	cmd[1] = MPROM_IDCODE_H;
	sndbuf[0] = 0xFF;
	sndbuf[1] = 0xFF;
	sndbuf[2] = 0xFF;
	sndbuf[3] = 0xFF;
	sndbuf[4] = 0xFF;
	devdo(dv, 16, cmd, 32, sndbuf, rcvbuf, 1);
	printf(" The MPROM Chip ID CODE is %02x%02x%02x%02x \n", 0xff&rcvbuf[3], 0xff&rcvbuf[2], 0xff&rcvbuf[1], 0xff&rcvbuf[0]);
	cmd[0] = MPROM_BYPASS_L;
	cmd[1] = MPROM_BYPASS_H;
	sndbuf[0] = 0;
	devdo(dv, 16, cmd, 0, sndbuf, rcvbuf, 0);
	unsigned long int ibrd = 0x00000000;
	ibrd = (rcvbuf[0] & 0xff) | ((rcvbuf[1] & 0xff) << 8) | ((rcvbuf[2] & 0xff) << 16) | ((rcvbuf[3] & 0xff) << 24) | ibrd;
	return ibrd;
}
*/

/*
void emu::fed::DCC::mctrl_bxr()
{
	cmd[0] = 0x00;  // fcn 0x00-write 0x01-read
	cmd[1] = 0x00;  // vme add
	cmd[2] = 0xFF;  // data h
	cmd[3] = 0x02;  // data l
	devdo(MCTRL, 4, cmd, 0, sndbuf, rcvbuf, 1);
}
*/

/*
void emu::fed::DCC::mctrl_evnr()
{
	cmd[0] = 0x00;  // fcn 0x00-write 0x01-read
	cmd[1] = 0x00;  // vme add
	cmd[2] = 0xFF;  // data h
	cmd[3] = 0x01;  // data l
	devdo(MCTRL, 4, cmd, 0, sndbuf, rcvbuf, 1);
}
*/

/*
void emu::fed::DCC::mctrl_fakeL1A(char rate, char num)
{
	cmd[0] = 0x00;  // fcn 0x00-write 0x01-read
	cmd[1] = 0x04;  // vme add
	cmd[2] = rate;  // data h
	cmd[3] = num;  // data l
	devdo(MCTRL, 4, cmd, 0, sndbuf, rcvbuf, 1);
}
*/

/*
void emu::fed::DCC::mctrl_fifoinuse(unsigned short int fifo)
{
	unsigned short int tmp;
	tmp = (fifo & 0x07FF);
	printf(" fifo in use %04x \n", tmp);
	cmd[0] = 0x00;  // fcn 0x00-write 0x01-read
	cmd[1] = 0x03;  // vme add
	cmd[2] = (tmp >> 8) & 0xff;  // data h
	cmd[3] = tmp & 0xff;  // data l
	devdo(MCTRL, 4, cmd, 0, sndbuf, rcvbuf, 1);
}
*/

/*
void emu::fed::DCC::mctrl_reg(char *c)
{
	printf(" register entered \n");
	cmd[0] = 0x01;  // fcn 0x00-write 0x01-read
	cmd[1] = 0x01;  // vme add
	cmd[2] = 0x02;  // data h
	cmd[3] = 0x03;  // data l
	devdo(MCTRL, 4, cmd, 0, sndbuf, rcvbuf, 1);
	if (cmd[0] == 0x01) {
		printf(" rcvbuf %02x %02x \n", rcvbuf[0]&0xff, rcvbuf[1]&0xff);
	}
}
*/

/*
void emu::fed::DCC::mctrl_swset(unsigned short int swset)
{
	unsigned short int tmp;
	tmp = (swset & 0xffff);
	printf(" Set switch register to: %04x \n", tmp);
	cmd[0] = 0x00;  // fcn 0x00-write 0x01-read
	cmd[1] = 0x07;  // vme add
	cmd[2] = (tmp >> 8) & 0xff;  // data h
	cmd[3] = tmp & 0xff;  // data l
	devdo(MCTRL, 4, cmd, 0, sndbuf, rcvbuf, 1);
}
*/

/*
unsigned short int  emu::fed::DCC::mctrl_swrd()
{
	unsigned short int swrd = 0;
	cmd[0] = 0x01;  // fcn 0x00-write 0x01-read
	cmd[1] = 0x1f;  // vme add
	cmd[2] = 0xff;  // data h
	cmd[3] = 0xff;  // data l
	devdo(MCTRL, 4, cmd, 0, sndbuf, rcvbuf, 1);
	printf(" Current switch register readback %02x %02x \n", rcvbuf[1]&0xff, rcvbuf[0]&0xff);
	swrd = ((rcvbuf[1] << 8) & 0xff00) | (rcvbuf[0] & 0x00ff);
	return swrd;
}
*/

/*
void emu::fed::DCC::mctrl_fmmset(unsigned short int fmmset)
{
	unsigned short int tmp;
	tmp = (fmmset & 0xffff);
	printf(" Set FMM register to: %04x \n", tmp);
	cmd[0] = 0x00;  // fcn 0x00-write 0x01-read
	cmd[1] = 0x08;  // vme add
	cmd[2] = (tmp >> 8) & 0xff;  // data h
	cmd[3] = tmp & 0xff;  // data l
	devdo(MCTRL, 4, cmd, 0, sndbuf, rcvbuf, 1);
}
*/

/*
unsigned short int  emu::fed::DCC::mctrl_fmmrd()
{
	unsigned short int fmmrd = 0;
	cmd[0] = 0x01;  // fcn 0x00-write 0x01-read
	cmd[1] = 0x1e;  // vme add
	cmd[2] = 0xff;  // data h
	cmd[3] = 0xff;  // data l
	devdo(MCTRL, 4, cmd, 0, sndbuf, rcvbuf, 1);
	printf(" Current FMM register readback: %02x %02x \n", rcvbuf[1]&0xff, rcvbuf[0]&0xff);
	fmmrd = ((rcvbuf[1] << 8) & 0xff00) | (rcvbuf[0] & 0x00ff);
	return fmmrd;
}
*/

/*
unsigned short int  emu::fed::DCC::mctrl_stath()
{
	unsigned short int rcvr = 0;
	cmd[0] = 0x01;  // fcn 0x00-write 0x01-read
	cmd[1] = 0x02;  // vme add
	cmd[2] = 0xff;  // data h
	cmd[3] = 0xff;  // data l
	devdo(MCTRL, 4, cmd, 0, sndbuf, rcvbuf, 1);
	//printf(" status high %02x %02x \n",rcvbuf[1]&0xff,rcvbuf[0]&0xff);
	rcvr = ((rcvbuf[1] << 8) & 0xff00) | (rcvbuf[0] & 0x00ff);
	return rcvr;
}
*/

/*
unsigned short int emu::fed::DCC::mctrl_statl()
{
	unsigned short int rcvr = 0;
	cmd[0] = 0x01;  // fcn 0x00-write 0x01-read
	cmd[1] = 0x01;  // vme add
	cmd[2] = 0xff;  // data h
	cmd[3] = 0xff;  // data l
	devdo(MCTRL, 4, cmd, 0, sndbuf, rcvbuf, 1);
	//printf(" status low %02x %02x \n",rcvbuf[1]&0xff,rcvbuf[0]&0xff);
	rcvr = ((rcvbuf[1] << 8) & 0xff00) | (rcvbuf[0] & 0x00ff);
	return rcvr;
}
*/

/*
unsigned short int emu::fed::DCC::mctrl_ratemon(int vaddress)
{
	unsigned short int rcvr = 0;
	cmd[0] = 0x01;  // fcn 0x00-write 0x01-read
	cmd[1] = (0x10) + (vaddress & 0x0f);  // vme add
	cmd[2] = 0xff;  // data h
	cmd[3] = 0xff;  // data l
	devdo(MCTRL, 4, cmd, 0, sndbuf, rcvbuf, 1);
	printf(" Data rate %02x %02x \n", rcvbuf[1]&0xff, rcvbuf[0]&0xff);
	rcvr = ((rcvbuf[1] << 8) & 0xff00) | (rcvbuf[0] & 0x00ff);
	return rcvr;
}
*/

/*
void emu::fed::DCC::mctrl_ttccmd(unsigned short int ctcc)
{
	unsigned short int tmp;
	tmp = (ctcc << 2) & 0xfc;
	cmd[0] = 0x00;  // fcn 0x00-write 0x01-read
	cmd[1] = 0x00;  // vme add
	cmd[2] = 0xFF;  // data h
	cmd[3] = tmp & 0xff;  // data l
	devdo(MCTRL, 4, cmd, 0, sndbuf, rcvbuf, 1);
}
*/

/*
unsigned short int  emu::fed::DCC::mctrl_rd_fifoinuse()
{
	unsigned short int rcvr = 0;
	cmd[0] = 0x01;  // fcn 0x00-write 0x01-read
	cmd[1] = 0x06;  // vme add
	cmd[2] = 0xff;  // data h
	cmd[3] = 0xff;  // data l
	devdo(MCTRL, 4, cmd, 0, sndbuf, rcvbuf, 1);
	//printf(" FIFO_in_use register %02x%02x\n",rcvbuf[1]&0xff,rcvbuf[0]&0xff);
	//printf(" FIFO_in_Use %02x%02x \n",rcvbuf[1]&0x03,rcvbuf[0]&0xff);
	rcvr = ((rcvbuf[1] << 8) & 0xff00) | (rcvbuf[0] & 0x00ff);
	return rcvr;
}
*/

/*
unsigned short int  emu::fed::DCC::mctrl_rd_ttccmd()
{
	unsigned short int rcvr = 0;
	cmd[0] = 0x01;  // fcn 0x00-write 0x01-read
	cmd[1] = 0x05;  // vme add
	cmd[2] = 0xff;  // data h
	cmd[3] = 0xff;  // data l
	devdo(MCTRL, 4, cmd, 0, sndbuf, rcvbuf, 1);
	printf(" TTC_cmd register %02x%02x \n", rcvbuf[1]&0xff, rcvbuf[0]&0xff);
	printf(" TTC_CMD %02x \n", (rcvbuf[0] >> 2) &0x3f);
	rcvr = ((rcvbuf[1] << 8) & 0xff00) | (rcvbuf[0] & 0x00ff);
	return rcvr;
}
*/

/*
void emu::fed::DCC::hdrst_in(void)
{
	enum DEVTYPE dv;
	printf(" InFOGA hardreset by inprom CF \n");
	dv = INPROM;
	cmd[0] = 0xEE; // Pulse CF low, for XCF32, the code is 0x00EE, 16-bits
	sndbuf[0] = 0xFF;
	devdo(dv, 8, cmd, 0, sndbuf, rcvbuf, 2);
	cmd[0] = PROM_BYPASS;
	sndbuf[0] = 0;
	devdo(dv, 8, cmd, 0, sndbuf, rcvbuf, 0);
	sleep((unsigned int) 1);
}
*/

/*
void emu::fed::DCC::hdrst_main(void)
{
	enum DEVTYPE dv;
	printf(" MainFOGA hardreset by Main_prom CF \n");
	printf(" MainPROM %d \n", RESET);
	dv = RESET;
	cmd[0] = 0xEE; //(Pulse CF low, 16-bit SIR for SCF32P)
	cmd[1] = 0x00;
	sndbuf[0] = 0xFF;
	devdo(dv, 16, cmd, 0, sndbuf, rcvbuf, 2);
	cmd[0] = 0xFF; //PROM_BYPASS;
	cmd[1] = 0xFF;
	sndbuf[0] = 0;
	devdo(dv, 16, cmd, 0, sndbuf, rcvbuf, 0);
	sleep((unsigned int) 1);
}
*/


void emu::fed::DCC::Parse(char *buf, int *Count, char **Word)
{
	*Word = buf;
	*Count = 0;
	while (*buf != '\0') {
		while ((*buf == ' ') || (*buf == '\t') || (*buf == '\n') || (*buf == '"')) * (buf++) = '\0';
		if ((*buf != '\n') && (*buf != '\0')) {
			Word[(*Count) ++] = buf;
		}
		while ((*buf != ' ') && (*buf != '\0') && (*buf != '\n') && (*buf != '\t') && (*buf != '"')) {
			buf++;
		}
	}
	*buf = '\0';
}



void emu::fed::DCC::epromload(char *design, enum DEVTYPE devnum, char *downfile, int writ)
{
	enum DEVTYPE devstp, dv;
	char *devstr;
	FILE *dwnfp, *fpout;
	char buf[8192], buf2[256];
	char *Word[256], *lastn;
	int Count, i, j, id, nbits, nbytes, pause, xtrbits, looppause;
	int tmp, cmpflag;
	//int tstusr;
	int nowrit = 1; //?
	char snd[5000], expect[5000], rmask[5000], smask[5000], cmpbuf[5000];
	char sndbuf[1024], rcvbuf[1024];
	//printf(" epromload %d \n",devnum);

	/*
	if(devnum==ALL){
		devnum=F1PROM;
		devstp=F5PROM;
	} else {
		devstp=devnum;
	}
	*/
	devstp = devnum;
	for (id = devnum;id <= devstp;id++) {
		dv = (DEVTYPE) id;
		xtrbits = geo[dv].sxtrbits;
		//printf(" ************************** xtrbits %d geo[dv].sxtrbits %d \n",xtrbits,geo[dv].sxtrbits);
		devstr = geo[dv].nam;
		dwnfp    = fopen(downfile, "r");
		fpout = fopen("eprom.bit", "w");
		//printf("Programming Design %s (%s) with %s\n",design,devstr,downfile);

		while (fgets(buf, 256, dwnfp) != NULL) {
			if ((buf[0] == '/' && buf[1] == '/') || buf[0] == '!') {
				//printf("%s",buf);
				std::cerr << buf << std::flush;
			} else {
				if (strrchr(buf, ';') == 0) {
					do {
						lastn = strrchr(buf, '\n');
						if (lastn != 0) lastn[0] = '\0';
						if (fgets(buf2, 256, dwnfp) != NULL) {
							strcat(buf, buf2);
						} else {
							//printf("End of File encountered.  Quiting\n");
							return;
						}
					} while (strrchr(buf, ';') == 0);
				}
				std::cerr << buf << std::flush;
				for (i = 0;i < 1024;i++) {
					cmpbuf[i] = 0;
					sndbuf[i] = 0;
					rcvbuf[i] = 0;
				}
				Parse(buf, &Count, & (Word[0]));
				//count=count+1;
				//printf(" count %d \n",count);
				if (strcmp(Word[0], "SDR") == 0) {
					cmpflag = 0;  //disable the comparison for no TDO SDR
					sscanf(Word[1], "%d", &nbits);
					nbytes = (nbits - 1) / 8 + 1;
					for (i = 2;i < Count;i += 2) {
						if (strcmp(Word[i], "TDI") == 0) {
							for (j = 0;j < nbytes;j++) {
								sscanf(&Word[i+1][2*(nbytes-j-1) +1], "%2hhX", &snd[j]);
							}

						}
						if (strcmp(Word[i], "SMASK") == 0) {
							for (j = 0;j < nbytes;j++) {
								sscanf(&Word[i+1][2*(nbytes-j-1) +1], "%2hhX", &smask[j]);
							}
						}
						if (strcmp(Word[i], "TDO") == 0) {
							cmpflag = 1;
							for (j = 0;j < nbytes;j++) {
								sscanf(&Word[i+1][2*(nbytes-j-1) +1], "%2hhX", &expect[j]);
							}
						}
						if (strcmp(Word[i], "MASK") == 0) {
							for (j = 0;j < nbytes;j++) {
								sscanf(&Word[i+1][2*(nbytes-j-1) +1], "%2hhX", &rmask[j]);
							}
						}
					}
					for (i = 0;i < nbytes;i++) {
						//sndbuf[i]=snd[i]&smask[i];
						sndbuf[i] = snd[i] & 0xff;
					}
					//printf("D%04d",nbits+xtrbits);
					// for(i=0;i<(nbits+xtrbits)/8+1;i++)printf("%02x",sndbuf[i]&0xff);printf("\n");
					if (nowrit == 0) {
						if ((geo[dv].jchan == 12)) {
							scan_reset(DATA_REG, sndbuf, nbits + xtrbits, rcvbuf, 0);
						} else {
							scan(DATA_REG, sndbuf, nbits + xtrbits, rcvbuf, 0);
						}
					} else if (writ == 1) {
						if ((geo[dv].jchan == 12)) {
							scan_reset(DATA_REG, sndbuf, nbits + xtrbits, rcvbuf, 0);
						} else {
							scan(DATA_REG, sndbuf, nbits + xtrbits, rcvbuf, 0);
						}
					}

					//Data readback comparison here:
					for (i = 0;i < nbytes;i++) {
						tmp = (rcvbuf[i] >> 3) & 0x1F;
						rcvbuf[i] = tmp | (rcvbuf[i+1] << 5 & 0xE0);
						//if (((rcvbuf[i]^expect[i]) & (rmask[i]))!=0 && cmpflag==1)
						//printf("read back wrong, at i %02d  rdbk %02X  expect %02X  rmask %02X\n",i,rcvbuf[i]&0xFF,expect[i]&0xFF,rmask[i]&0xFF); */
					}
					if (cmpflag == 1) {
						for (i = 0;i < nbytes;i++) {
							fprintf(fpout, " %02X", rcvbuf[i]&0xFF);
							if (i % 4 == 3) fprintf(fpout, "\n");
						}
					}
				} else if (strcmp(Word[0], "SIR") == 0) {
					nowrit = 0;
					sscanf(Word[1], "%d", &nbits);
					nbytes = (nbits - 1) / 8 + 1;
					for (i = 2;i < Count;i += 2) {
						if (strcmp(Word[i], "TDI") == 0) {
							for (j = 0;j < nbytes;j++) {
								sscanf(&Word[i+1][2*(nbytes-j-1) +1], "%2hhX", &snd[j]);
							}
							if (nbytes == 1) {
								if (0xfd == (snd[0]&0xff)) nowrit = 1;
							} // nowrit=1
						} else if (strcmp(Word[i], "SMASK") == 0) {
							for (j = 0;j < nbytes;j++) {
								sscanf(&Word[i+1][2*(nbytes-j-1) +1], "%2hhX", &smask[j]);
							}
						}
						if (strcmp(Word[i], "TDO") == 0) {
							for (j = 0;j < nbytes;j++) {
								sscanf(&Word[i+1][2*(nbytes-j-1) +1], "%2hhX", &expect[j]);
							}
						} else if (strcmp(Word[i], "MASK") == 0) {
							for (j = 0;j < nbytes;j++) {
								sscanf(&Word[i+1][2*(nbytes-j-1) +1], "%2hhX", &rmask[j]);
							}
						}
					}
					for (i = 0;i < nbytes;i++) {
						//sndbuf[i]=snd[i]&smask[i];
						sndbuf[i] = snd[i];
					}
					//printf("I%04d",nbits);
					//for (i=0;i<nbits/8+1;i++) printf("%02x",sndbuf[i]&0xff);
					//printf("\n");
					// JRG, brute-force way to download UNALTERED PromUserCode from SVF file to
					//  DDU prom, but screws up CFEB/DMB program method:      nowrit=0;
					if (nowrit == 0) {
						devdo(dv, nbits, sndbuf, 0, sndbuf, rcvbuf, 0);
					} else {
						if (writ == 1) devdo(dv, nbits, sndbuf, 0, sndbuf, rcvbuf, 0);
						if (writ == 0) printf(" ***************** nowrit %02x \n", sndbuf[0]);
					}

					//printf("send %2d instr bits %02X %02X %02X %02X %02X\n",nbits,sndbuf[4]&0xFF,sndbuf[3]&0xFF,sndbuf[2]&0xFF,sndbuf[1]&0xFF,sndbuf[0]&0xFF);
					//printf("expect %2d instr bits %02X %02X %02X %02X %02X\n",nbits,expect[4]&0xFF,expect[3]&0xFF,expect[2]&0xFF,expect[1]&0xFF,expect[0]&0xFF);
				} else if (strcmp(Word[0], "RUNTEST") == 0) {
					sscanf(Word[1], "%d", &pause);
					//printf("RUNTEST = %d\n",pause);
					//ipd=83*pause;
					//sleep(1);
					//t1=(double) clock()/(double) CLOCKS_PER_SEC;
					//for (i=0;i<ipd;i++);
					//t2=(double) clock()/(double) CLOCKS_PER_SEC;
					//if (pause>1000) printf("pause = %f s  while erasing\n",t2-t1);
					//for (i=0;i<pause/100;i++)
					//devdo(dv,-1,sndbuf,0,sndbuf,rcvbuf,2);
					//fpause=pause;
					//pause=pause/2;
					if (pause > 65535) {
						sndbuf[0] = 255;
						sndbuf[1] = 255;
						for (looppause = 0;looppause < pause / 65536;looppause++) devdo(dv, -99, sndbuf, 0, sndbuf, rcvbuf, 0);
						pause = 65535;
					}
					sndbuf[0] = pause - (pause / 256) * 256; // Seriously?
					sndbuf[1] = pause / 256;
					//printf(" sndbuf %02x %02x %d \n",sndbuf[1],sndbuf[0],pause);
					devdo(dv, -99, sndbuf, 0, sndbuf, rcvbuf, 2);
					//fpause=fpause*1.5+100;
					//pause=fpause;
					//flush_vme();
					//usleep(pause);
					//printf(" send sleep \n");
				} else if ((strcmp(Word[0], "STATE") == 0) && (strcmp(Word[1], "RESET") == 0) && (strcmp(Word[2], "IDLE;") == 0)) {
					//printf("goto reset idle state\n");
					devdo(dv, -1, sndbuf, 0, sndbuf, rcvbuf, 2);
				} else if (strcmp(Word[0], "TRST") == 0) {

				} else if (strcmp(Word[0], "ENDIR") == 0) {

				} else if (strcmp(Word[0], "ENDDR") == 0) {

				}
			}
		}
		fclose(fpout);
		fclose(dwnfp);
	}
	//flush_vme();
	//send_last();
}



unsigned long int emu::fed::DCC::readReg(enum DEVTYPE dt, char reg, unsigned int nBits)
	throw (FEDException)
{
	// reads are always 16 bits from the DCC

	if (dt != INPROM && dt != MCTRL) {
		XCEPT_RAISE(FEDException,"Can't read from that device");
	}

	//char cmd[4];
	//char sndbuf[4];
	//char rcvbuf[4];
	
	cmd[0] = 0x01; // For read function
	cmd[1] = reg; // vme address
	cmd[2] = 0xff; // data h
	cmd[3] = 0xff; // data l

	// This is it.
	devdo(dt, nBits, cmd, 0, sndbuf, rcvbuf, 1);

	return ((rcvbuf[3] << 24) & 0xff000000) | ((rcvbuf[2] << 16) & 0xff0000) | ((rcvbuf[1] << 8) & 0xff00) | (rcvbuf[0] & 0xff);
}





void emu::fed::DCC::writeReg(enum DEVTYPE dt, char reg, unsigned long int value)
	throw (FEDException)
{
	// reads are always 16 bits from the DCC

	if (dt != INPROM && dt != MCTRL) {
		XCEPT_RAISE(FEDException,"Can't read from that device");
	}

	//char cmd[4];
	//char sndbuf[4];
	//char rcvbuf[4];
	
	cmd[0] = 0x00; // For write function
	cmd[1] = reg; // vme address
	cmd[2] = (value >> 8) & 0xff; // data h
	cmd[3] = value & 0xff; // data l

	// This is it.
	devdo(dt, 4, cmd, 0, sndbuf, rcvbuf, 1);
}



unsigned int emu::fed::DCC::readStatusHigh()
	throw (FEDException)
{
	try {
		return readReg(MCTRL, 0x02);
	} catch (FEDException &e) { throw; }
}



unsigned int emu::fed::DCC::readStatusLow()
	throw (FEDException)
{
	try {
		return readReg(MCTRL, 0x01);
	} catch (FEDException &e) { throw; }
}



unsigned int emu::fed::DCC::readFIFOInUse()
	throw (FEDException)
{
	try {
		return readReg(MCTRL, 0x06);
	} catch (FEDException &e) { throw; }
}



void emu::fed::DCC::setFIFOInUse(unsigned int value)
	throw (FEDException)
{
	try {
		writeReg(MCTRL, 0x03, value);
	} catch (FEDException &e) { throw; }
}


unsigned int emu::fed::DCC::readRate(unsigned int fifo)
	throw (FEDException)
{
	if (fifo > 11) XCEPT_RAISE(FEDException, "there are only 12 FIFOs to check [0-11]");
	try {
		return readReg(MCTRL, 0x10 + fifo);
	} catch (FEDException &e) { throw; }
}


unsigned int emu::fed::DCC::readSoftwareSwitch()
	throw (FEDException)
{
	try {
		return readReg(MCTRL, 0x1f);
	} catch (FEDException &e) { throw; }
}


void emu::fed::DCC::setSoftwareSwitch(unsigned int value)
	throw (FEDException)
{
	try {
		return writeReg(MCTRL, 0x07, value);
	} catch (FEDException &e) { 
		// This always causes an error for some reason.
		//throw;
	}
}


unsigned int emu::fed::DCC::readFMM()
	throw (FEDException)
{
	try {
		return readReg(MCTRL, 0x1e);
	} catch (FEDException &e) { throw; }
}


void emu::fed::DCC::setFMM(unsigned int value)
	throw (FEDException)
{
	try {
		return writeReg(MCTRL, 0x08, value);
	} catch (FEDException &e) { throw; }
}


unsigned int emu::fed::DCC::readTTCCommand()
	throw (FEDException)
{
	try {
		return readReg(MCTRL, 0x05);
	} catch (FEDException &e) { throw; }
}


void emu::fed::DCC::setTTCCommand(unsigned int value)
	throw (FEDException)
{
	try {
		return writeReg(MCTRL, 0x00, 0Xff00 | ((value << 2) & 0xfc));
	} catch (FEDException &e) { throw; }
}


void emu::fed::DCC::resetBX()
	throw (FEDException)
{
	try {
		return writeReg(MCTRL, 0x00, 0xff02);
	} catch (FEDException &e) { throw; }
}


void emu::fed::DCC::resetEvents()
	throw (FEDException)
{
	try {
		return writeReg(MCTRL, 0x00, 0xff01);
	} catch (FEDException &e) { throw; }
}


void emu::fed::DCC::setFakeL1A(unsigned int value)
	throw (FEDException)
{
	try {
		return writeReg(MCTRL, 0x04, value);
	} catch (FEDException &e) { throw; }
}


unsigned long int emu::fed::DCC::readIDCode(enum DEVTYPE dt)
	throw (FEDException)
{
	unsigned int nBits = 0;
	//char cmd[2];
	//char sndbuf[5];
	//char rcvbuf[5];
	
	if (dt == INPROM) {
		cmd[0] = PROM_IDCODE;
		nBits = 8;
	} else if (dt == MPROM) {
		cmd[0] = MPROM_IDCODE_L;
		cmd[1] = MPROM_IDCODE_H;
		nBits = 16;
	} else {
		XCEPT_RAISE(FEDException, "Can only read IDCode from DEVTYPEs INPROM or MPROM");
	}
	
	// special case:  no readReg.
	sndbuf[0] = 0xFF;
	sndbuf[1] = 0xFF;
	sndbuf[2] = 0xFF;
	sndbuf[3] = 0xFF;
	sndbuf[4] = 0xFF;
	devdo(dt, nBits, cmd, 32, sndbuf, rcvbuf, 1);
	
	if (dt == INPROM) {
		cmd[0] = PROM_BYPASS;
	} else if (dt == MPROM) {
		cmd[0] = MPROM_BYPASS_L;
		cmd[1] = MPROM_BYPASS_H;
	}

	sndbuf[0] = 0;
	devdo(dt, nBits, cmd, 0, sndbuf, rcvbuf, 0);

	return (rcvbuf[0] & 0xff) | ((rcvbuf[1] & 0xff) << 8) | ((rcvbuf[2] & 0xff) << 16) | ((rcvbuf[3] & 0xff) << 24);
}



unsigned long int emu::fed::DCC::readUserCode(enum DEVTYPE dt)
	throw (FEDException)
{
	
	unsigned int nBits = 0;
	//char cmd[2];
	//char sndbuf[5];
	//char rcvbuf[5];
	
	if (dt == INPROM) {
		cmd[0] = PROM_USERCODE;
		nBits = 8;
	} else if (dt == MPROM) {
		cmd[0] = MPROM_USERCODE_L;
		cmd[1] = MPROM_USERCODE_H;
		nBits = 16;
	} else {
		XCEPT_RAISE(FEDException, "Can only read UserID from DEVTYPEs INPROM or MPROM");
	}
	
	// special case:  no readReg.
	sndbuf[0] = 0xFF;
	sndbuf[1] = 0xFF;
	sndbuf[2] = 0xFF;
	sndbuf[3] = 0xFF;
	sndbuf[4] = 0xFF;
	devdo(dt, nBits, cmd, 32, sndbuf, rcvbuf, 1);
	
	if (dt == INPROM) {
		cmd[0] = PROM_BYPASS;
	} else if (dt == MPROM) {
		cmd[0] = MPROM_BYPASS_L;
		cmd[1] = MPROM_BYPASS_H;
	}
	
	sndbuf[0] = 0;
	devdo(dt, nBits, cmd, 0, sndbuf, rcvbuf, 0);
	
	return (rcvbuf[0] & 0xff) | ((rcvbuf[1] & 0xff) << 8) | ((rcvbuf[2] & 0xff) << 16) | ((rcvbuf[3] & 0xff) << 24);
	
}



unsigned int emu::fed::DCC::getDDUSlotFromFIFO(unsigned int fifo) {
	if (slot() == 8 && fifo < 10) {
		return (fifo % 2) ? 13 - fifo/2 : fifo/2 + 3;
	} else if (slot() == 17 && fifo < 8 && fifo >= 2) {
		return (fifo % 2) ? 20 - fifo/2 : fifo/2 + 14;
	} else {
		return 0;
	}
}


void emu::fed::DCC::crateHardReset()
{
	uint16_t switchCache = readSoftwareSwitchAdvanced();
	writeSoftwareSwitchAdvanced(0x1000);
	writeTTCCommandAdvanced(0x34);
	sleep((unsigned int) 2);
	writeSoftwareSwitchAdvanced(switchCache);
}

void emu::fed::DCC::crateSyncReset()
{
	uint16_t switchCache = readSoftwareSwitchAdvanced();
	writeSoftwareSwitchAdvanced(0x1000);
	writeTTCCommandAdvanced(0x3);
	sleep((unsigned int) 1);
	writeSoftwareSwitchAdvanced(switchCache);
}



std::vector<int16_t> emu::fed::DCC::readRegAdvanced(enum DEVTYPE dev, char myRegister, unsigned int nBits)
throw (FEDException)
{
	// The information about the element being written
	JTAGElement *element = JTAGMap[dev];
	
	//std::clog << "Attempting to read from " << element->name << " register " << std::hex << (unsigned int) myRegister << " bits " << std::dec << nBits << std::endl;
		
	// Direct VME reads are different
	if (element->directVME) {

		// The address for MCTRL is special, as it also contains the command code.
		int32_t myAddress = (myRegister << 2) | element->bitCode;
		//std::cout << "address " << std::hex << myAddress << std::dec << std::endl;

		return readCycle(myAddress,nBits);

	// Everything else is a JTAG command?
	} else {

		// Open the appropriate register with an initialization command.
		commandCycle(dev, myRegister);

		// Read the register out
		// Make me a bogus bunch of bits to shove into the register
		unsigned int nWords = (nBits == 0) ? 0 : (nBits - 1)/16 + 1;
		std::vector<int16_t> bogoBits(nWords,0xFFFF);

		// Shove in (and read out)
		std::vector<int16_t> result = jtagReadWrite(dev, nBits, bogoBits);

		// Finally, set the bypass
		commandCycle(dev, element->bypassCommand);

		return result;
		
	}
	
}



std::vector<int16_t> emu::fed::DCC::writeRegAdvanced(enum DEVTYPE dev, char myRegister, unsigned int nBits, std::vector<int16_t> myData)
throw (FEDException)
{
	
	// The information about the element being written
	JTAGElement *element = JTAGMap[dev];
	
	//std::cout << "Attempting to write to " << element->name << " register " << std::hex << (unsigned int) myRegister << " bits " << std::dec << nBits << " values (low to high) ";
	//for (std::vector<int16_t>::iterator iData = myData.begin(); iData != myData.end(); iData++) {
	//std::cout << std::showbase << std::hex << (*iData) << std::dec << " ";
	//}
	//std::cout << std::endl;
	
	// Direct VME writes are different
	if (element->directVME) {
		
		// The address for MCTRL is special, as it also contains the command code.
		int32_t myAddress = (myRegister << 2) | element->bitCode;
		//std::cout << "address " << std::hex << myAddress << std::dec << std::endl;
		
		writeCycle(myAddress, nBits, myData);

		// This sort of write does not read back, so return an empty vector.
		std::vector<int16_t> bogoBits;
		return bogoBits;
		
	// Everything else is a JTAG command?
	} else {
		
		// Open the appropriate register with an initialization command.
		commandCycle(dev, myRegister);
		
		// Shove in (and read out)
		std::vector<int16_t> result = jtagReadWrite(dev, nBits, myData);
		
		// Finally, set the bypass
		commandCycle(dev, element->bypassCommand);
		
		return result; // The value that used to be in the register.
		
	}
	
}



uint16_t emu::fed::DCC::readStatusHighAdvanced()
throw (FEDException)
{
	try {
		return readRegAdvanced(MCTRL, 0x02, 16)[0];
	} catch (FEDException &e) {
		throw;
	}
}



uint16_t emu::fed::DCC::readStatusLowAdvanced()
throw (FEDException)
{
	try {
		return readRegAdvanced(MCTRL, 0x01, 16)[0];
	} catch (FEDException &e) {
		throw;
	}
}



uint16_t emu::fed::DCC::readFIFOInUseAdvanced()
throw (FEDException)
{
	try {
		return readRegAdvanced(MCTRL, 0x06, 16)[0];
	} catch (FEDException &e) {
		throw;
	}
}



void emu::fed::DCC::writeFIFOInUseAdvanced(uint16_t value)
throw (FEDException)
{
	try {
		std::vector<int16_t> myData(1,value & 0x07FF);
		writeRegAdvanced(MCTRL, 0x03, 16, myData);
		return;
	} catch (FEDException &e) {
		throw;
	}
}



uint16_t emu::fed::DCC::readRateAdvanced(unsigned int fifo)
throw (FEDException)
{
	if (fifo > 11) XCEPT_RAISE(FEDException, "there are only 12 FIFOs to check [0-11]");
	try {
		return readRegAdvanced(MCTRL, 0x10 + fifo, 16)[0];
	} catch (FEDException &e) {
		throw;
	}
}



uint16_t emu::fed::DCC::readSoftwareSwitchAdvanced()
throw (FEDException)
{
	try {
		return readRegAdvanced(MCTRL, 0x1f, 16)[0];
	} catch (FEDException &e) {
		throw;
	}
}



void emu::fed::DCC::writeSoftwareSwitchAdvanced(uint16_t value)
throw (FEDException)
{
	try {
		std::vector<int16_t> myData(1, value);
		writeRegAdvanced(MCTRL, 0x07, 16, myData);
		return;
	} catch (FEDException &e) {
		throw;
	}
}



uint16_t emu::fed::DCC::readFMMAdvanced()
throw (FEDException)
{
	try {
		return readRegAdvanced(MCTRL, 0x1e, 16)[0];
	} catch (FEDException &e) {
		throw;
	}
}



void emu::fed::DCC::writeFMMAdvanced(uint16_t value)
throw (FEDException)
{
	try {
		std::vector<int16_t> myData(1, value);
		writeRegAdvanced(MCTRL, 0x08, 16, myData);
		return;
	} catch (FEDException &e) {
		throw;
	}
}



uint16_t emu::fed::DCC::readTTCCommandAdvanced()
throw (FEDException)
{
	try {
		return readRegAdvanced(MCTRL,0x05,16)[0];
	} catch (FEDException) {
		throw;
	}
}



void emu::fed::DCC::writeTTCCommandAdvanced(int8_t value)
throw (FEDException)
{
	try {
		// The first two bits are special.
		std::vector<int16_t> myData(1, 0xff00 | ((value << 2) & 0xfc));
		writeRegAdvanced(MCTRL,0x00,16,myData);
		return;
	} catch (FEDException) {
		throw;
	}
}



void emu::fed::DCC::resetBXAdvanced()
throw (FEDException)
{
	try {
		std::vector<int16_t> myData(1, 0x02);
		writeRegAdvanced(MCTRL,0x00,16,myData);
		return;
	} catch (FEDException) {
		throw;
	}
}



void emu::fed::DCC::resetEventsAdvanced()
throw (FEDException)
{
	try {
		std::vector<int16_t> myData(1, 0x01);
		writeRegAdvanced(MCTRL,0x00,16,myData);
		return;
	} catch (FEDException) {
		throw;
	}
}



void emu::fed::DCC::writeFakeL1AAdvanced(uint16_t value)
throw (FEDException)
{
	try {
		std::vector<int16_t> myData(1, value);
		writeRegAdvanced(MCTRL,0x04,16,myData);
		return;
	} catch (FEDException) {
		throw;
	}
}



uint32_t emu::fed::DCC::readIDCodeAdvanced(enum DEVTYPE dev)
throw (FEDException)
{
	if (dev != MPROM && dev != INPROM)
		XCEPT_RAISE(FEDException, "must supply a PROM device as an argument");
	try {
		std::vector<int16_t> result = readRegAdvanced(dev,PROM_IDCODE,32);
		return result[0] | (result[1] << 16);
	} catch (FEDException) {
		throw;
	}
}



uint32_t emu::fed::DCC::readUserCodeAdvanced(enum DEVTYPE dev)
throw (FEDException)
{
	if (dev != MPROM && dev != INPROM)
		XCEPT_RAISE(FEDException, "must supply a PROM device as an argument");
	try {
		std::vector<int16_t> result = readRegAdvanced(dev,PROM_USERCODE,32);
		return result[0] | (result[1] << 16);
	} catch (FEDException) {
		throw;
	}
}





