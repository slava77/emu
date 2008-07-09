/* New motherboard VTX2 not VTX  so MCTRL,6  not MCTRL,5 */
#include "DCC.h"
#include "VMEController.h"
#include <fstream>
#include <stdio.h>
#include <cmath>
#include <unistd.h>
#include "geom_def.h"

using namespace std;

#ifndef debugV //silent mode
#define PRINT(x)
#define PRINTSTRING(x)
#else //verbose mode
#define PRINT(x) cout << #x << ":\t" << x << endl;
#define PRINTSTRING(x) cout << #x << endl;
#endif

extern char filename[100];
extern unsigned int hexval;
extern short int intval;
extern short int intval2;

// declarations
void Parse(char *buf, int *Count, char **Word);
void shuffle(char *a, char *b);

DCC::DCC(int crate, int slot) :
		VMEModule(slot),
		fifoinuse_(1022),
		softsw_(0)
{
	// cout<<"DCC \n";
}

DCC::DCC(int slot) :
		VMEModule(slot),
		fifoinuse_(1022),
		softsw_(0)
{
	// cout<<"DCC \n";
}


DCC::~DCC()
{
	// cout << "Killing DCC" << endl;
}


void DCC::end()
{
	//  cout << "calling DCC::end" << endl;
	send_last();
	VMEModule::end();
}


void DCC::configure()
{

	//printf(" *********************** DCC configure is called \n");
	//printf(" DCC slot %d fifoinuse %d \n",slot(),fifoinuse_);
	if (slot() < 21) {
		mctrl_fifoinuse(fifoinuse_);
		mctrl_swset(softsw_);
	}
}

unsigned long int  DCC::inprom_userid()
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

unsigned long int  DCC::mprom_userid()
{
	enum DEVTYPE dv;;
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
unsigned long int  DCC::inprom_chipid()
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

unsigned long int  DCC::mprom_chipid()
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



void DCC::mctrl_bxr()
{
	cmd[0] = 0x00;  // fcn 0x00-write 0x01-read
	cmd[1] = 0x00;  // vme add
	cmd[2] = 0xFF;  // data h
	cmd[3] = 0x02;  // data l
	devdo(MCTRL, 4, cmd, 0, sndbuf, rcvbuf, 1);
}

void DCC::mctrl_evnr()
{
	cmd[0] = 0x00;  // fcn 0x00-write 0x01-read
	cmd[1] = 0x00;  // vme add
	cmd[2] = 0xFF;  // data h
	cmd[3] = 0x01;  // data l
	devdo(MCTRL, 4, cmd, 0, sndbuf, rcvbuf, 1);
}

void DCC::mctrl_fakeL1A(char rate, char num)
{
	cmd[0] = 0x00;  // fcn 0x00-write 0x01-read
	cmd[1] = 0x04;  // vme add
	cmd[2] = rate;  // data h
	cmd[3] = num;  // data l
	devdo(MCTRL, 4, cmd, 0, sndbuf, rcvbuf, 1);
}


void DCC::mctrl_fifoinuse(unsigned short int fifo)
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

void DCC::mctrl_reg(char *c)
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


void DCC::mctrl_swset(unsigned short int swset)
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

unsigned short int  DCC::mctrl_swrd()
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
void DCC::mctrl_fmmset(unsigned short int fmmset)
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

unsigned short int  DCC::mctrl_fmmrd()
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

unsigned short int  DCC::mctrl_stath()
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


unsigned short int DCC::mctrl_statl()
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

unsigned short int DCC::mctrl_ratemon(int vaddress)
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


void DCC::mctrl_ttccmd(unsigned short int ctcc)
{
	unsigned short int tmp;
	tmp = (ctcc << 2) & 0xfc;
	cmd[0] = 0x00;  // fcn 0x00-write 0x01-read
	cmd[1] = 0x00;  // vme add
	cmd[2] = 0xFF;  // data h
	cmd[3] = tmp & 0xff;  // data l
	devdo(MCTRL, 4, cmd, 0, sndbuf, rcvbuf, 1);
}

unsigned short int  DCC::mctrl_rd_fifoinuse()
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

unsigned short int  DCC::mctrl_rd_ttccmd()
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


void DCC::hdrst_in(void)
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

void DCC::hdrst_main(void)
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



void DCC::Parse(char *buf, int *Count, char **Word)
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


void DCC::epromload(char *design, enum DEVTYPE devnum, char *downfile, int writ, char *cbrdnum)
{
	enum DEVTYPE devstp, dv;
	char *devstr;
	FILE *dwnfp, *fpout;
	char buf[8192], buf2[256];
	char *Word[256], *lastn;
	int Count, i, j, id, nbits, nbytes, pause, xtrbits, looppause;
	int tmp, cmpflag;
	int tstusr;
	int nowrit;
	char snd[5000], expect[5000], rmask[5000], smask[5000], cmpbuf[5000];
	extern struct GEOM geo[];
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
			//printf("%s",buf);
			if ((buf[0] == '/' && buf[1] == '/') || buf[0] == '!') {
				//printf("%s",buf);
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
//JRG, new selective way to download UNALTERED PromUserCode from SVF to
//  ANY prom:  just set cbrdnum[3,2,1,0]=0 in calling routine!
//  was  if(nowrit==1){
							if (nowrit == 1 && (cbrdnum[0] | cbrdnum[1] | cbrdnum[2] | cbrdnum[3]) != 0) {
								tstusr = 0;
								snd[0] = cbrdnum[0];
								snd[1] = cbrdnum[1];
								snd[2] = cbrdnum[2];
								snd[3] = cbrdnum[3];
								//printf(" snd %02x %02x %02x %02x \n",snd[0],snd[1],snd[2],snd[3]);
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
					} else {
						if (writ == 1) {

							if ((geo[dv].jchan == 12)) {
								scan_reset(DATA_REG, sndbuf, nbits + xtrbits, rcvbuf, 0);
							} else {
								scan(DATA_REG, sndbuf, nbits + xtrbits, rcvbuf, 0);
							}
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
					sndbuf[0] = pause - (pause / 256) * 256;
					sndbuf[1] = pause / 256;
					//printf(" sndbuf %02x %02x %d \n",sndbuf[1],sndbuf[0],pause);
					devdo(dv, -99, sndbuf, 0, sndbuf, rcvbuf, 2);
					//fpause=fpause*1.5+100;
					//pause=fpause;
					flush_vme();
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
	flush_vme();
	send_last();
}



void DCC::executeCommand(string command)
{
}


void DCC::crateHardReset()
{
	mctrl_swset(0x1000);
	mctrl_ttccmd(0x34);
	sleep((unsigned int) 3);
	mctrl_swset(0x0000);
}

void DCC::crateSyncReset()
{
	mctrl_swset(0x1000);
	mctrl_ttccmd(0x3);
	sleep((unsigned int) 1);
	mctrl_swset(0x0000);
}
