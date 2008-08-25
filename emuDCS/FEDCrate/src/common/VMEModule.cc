/*****************************************************************************\
* $Id: VMEModule.cc,v 3.8 2008/08/25 12:25:49 paste Exp $
*
* $Log: VMEModule.cc,v $
* Revision 3.8  2008/08/25 12:25:49  paste
* Major updates to VMEController/VMEModule handling of CAEN instructions.  Also, added version file for future RPMs.
*
* Revision 3.7  2008/08/19 14:51:03  paste
* Update to make VMEModules more independent of VMEControllers.
*
* Revision 3.6  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#include "VMEModule.h"

//#include <cmath>
#include <string>
#include <sstream>
//#include <stdio.h>
#include <iostream>
//#include <unistd.h>

#include "CAENVMElib.h"
#include "CAENVMEtypes.h"

#define DELAY3 16.384

emu::fed::VMEModule::VMEModule(int mySlot):
	//vmeController_(0),
	slot_(mySlot),
	BHandle_(-1)
{
	vmeadd_ = slot_ << 19;
}

/*
void emu::fed::VMEModule::start() {
	// vmeadd_=0x00000000|(slot_<<19);
	vmeController_->start(slot_);
}
*/

/*
void emu::fed::VMEModule::end() {
	idevo_ = 0;
}
*/


void emu::fed::VMEModule::CAEN_read(unsigned long Address,unsigned short int *data)
	throw (FEDException)
{
	CVAddressModifier AM = cvA24_U_DATA;
	CVDataWidth DW = cvD16;
	int err = CAENVME_ReadCycle(BHandle_,Address,data,AM,DW);
	if (err) {
		std::ostringstream error;
		error << "CAENVME read error " << err;
		XCEPT_RAISE(FEDException, error.str());
	}
}



int16_t emu::fed::VMEModule::CAEN_read(unsigned long Address)
throw (FEDException)
{
	int16_t *readData;
	CVAddressModifier AM = cvA24_U_DATA;
	CVDataWidth DW = cvD16;
	int err = CAENVME_ReadCycle(BHandle_,Address,readData,AM,DW);
	if (err) {
		std::ostringstream error;
		error << "CAENVME read error " << err;
		XCEPT_RAISE(FEDException, error.str());
	}
	return *readData;
}



void emu::fed::VMEModule::CAEN_write(unsigned long Address,unsigned short int *data)
	throw (FEDException)
{
	CVAddressModifier AM = cvA24_U_DATA;
	CVDataWidth DW = cvD16;
	int err = CAENVME_WriteCycle(BHandle_, Address, (char *)data, AM, DW);
	if (err) {
		std::ostringstream error;
		error << "CAENVME write error " << err;
		XCEPT_RAISE(FEDException, error.str());
	}
}



void emu::fed::VMEModule::CAEN_write(unsigned long Address, int16_t data)
	throw (FEDException)
{
	CVAddressModifier AM = cvA24_U_DATA;
	CVDataWidth DW = cvD16;
	int err = CAENVME_WriteCycle(BHandle_, Address, &data, AM, DW);
	if (err) {
		std::ostringstream error;
		error << "CAENVME write error " << err;
		XCEPT_RAISE(FEDException, error.str());
	}
}



void emu::fed::VMEModule::vme_controller(int irdwr, unsigned short int address, unsigned short int data, char *rcv) {

	// irdwr:
	// 0 bufread
	// 1 bufwrite 
	// 2 bufread snd  
	// 3 bufwrite snd 
	// 4 flush to VME
	// 5 loop back 
	// 6 delay

	switch (irdwr) {
	
	case 0:
	case 2:
		//char rdata[2];
		//CAEN_read((long unsigned int) ptr,(unsigned short int *) rdata);
		int readData = CAEN_read(address);
		rcv[0] = readData & 0xff;
		rcv[1] = (readData & 0xff00) >> 8;
		break;
		
	case 1:
	case 3:
		//CAEN_write((long unsigned int) ptr,data);
		CAEN_write(address, data);
		break;
		
	case 6:
		udelay((long int) (data * DELAY3));
		break;
	
	default:
		break;
	}

}



void  emu::fed::VMEModule::sleep_vme(const char *outbuf)   // in usecs (min 16 usec)
{
	unsigned short int delay = (unsigned short int) ((((outbuf[1]<<8)&0xff00) + (outbuf[0]&0xff)) / 16.0 + 1);
	
	udelay((long int) (delay * DELAY3));
}



void emu::fed::VMEModule::devdo(DEVTYPE dev,int ncmd,const char *cmd,int nbuf, const char *inbuf,char *outbuf,int irdsnd)
	throw (FEDException)
{
	
	//int kbit,kbybit;
	//char kbypass;
	
	//int ppnt,pcmd,pow2;
	//int idev,i,k,m;
	//int ncmd2,nbcmd2,nbuf2;
	
	//int init;
	
	//unsigned short int ishft,temp;
	//unsigned long int vmeaddo;
	//static int feuse;
	
	// irdsnd for jtag
	// irdsnd = 0 send immediately, no read
	// irdsnd = 1 send immediately, read
	// irdsnd = 2 send in buffer, no read

	// dev == 99 is a repeat command
	//bool init = false;
	//if (dev != 99) {
	int idev = geo[dev].jchan;
	//init = true;
	//} else {
	//	idev = idevo_;
	//	if (idev>4 && idev!=12) return;
	//}
	//idevo_ = idev;
	
	// printf(" enter devdo %d %d \n",dev,idev);
	
	// printf(" idev idevo_ dev %d %d %d \n",idev,idevo_,dev);
	// check we have same old device otherwise we need to initialize
	//if (idev != idevo_ || vmeadd != vmeaddo) {
	//init=1;
	//}
	//idevo_ = idev;
	//vmeaddo = vmeadd;
	
	//  printf(" about to initialize plev idve devo init %d %d %d %d \n",plev,idev,idevo_,init);
	
	/////////////  JTAG initialize ///////////////////
	/////////////  immediate instruction nonJTAG /////
	
	switch (idev) {
		
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 12:
		
		// no breaks
		
		int ncmd2;
		int nbuf2;
		char cmd2[100];
		
		if (ncmd > 0) {
			// stan jtag kludge kludge for serial devices
			if (geo[dev].nser) {
				int ppnt = 0;
				cmd2[0] = 0x00;
				cmd2[1] = 0x00;
				int m = geo[dev].nser;
				int pcmd = 0;
				char tmp = 0;
				for (int i=0; i < geo[dev].nser; i++) {
					if (geo[dev].seri[m-i-1] < 0) {
						tmp = cmd[0];
						pcmd = geo[-1*geo[dev].seri[m-i-1]].kbit;
					} else {
						tmp = geo[geo[dev].seri[m-i-1]].kbypass;
						pcmd = geo[geo[dev].seri[m-i-1]].kbit;
					}
					//printf(" i %d tmp[0] %04x pcmd %d  \n",i,tmp,pcmd);
					//printf(" cmd[0] %02x \n",cmd[0]);
					for(int k = 0; k < pcmd; k++) {
						ppnt++;
						if ( (tmp >> k) & 0x01 ) {
							if (ppnt < 9) {
								cmd2[0] += pows_(2,ppnt-1);
								//printf(" k cmd %d %02x %d %d \n",k,cmd2[0],ppnt,pow2);
							}
							if(ppnt > 8) {
								cmd2[1] += pows_(2,ppnt-9);
							}
						}
					}
				}
				
				ncmd2 = ppnt;
				//nbcmd2 = ncmd2/8+1;
				nbuf2 = geo[dev].sxtrbits;
				if (nbuf > 0) {
					nbuf2 += nbuf;
				} else {
					nbuf2 = 0;
				}
				//kbit = geo[dev].kbit;
				//kbybit = geo[dev].kbybit;
				//kbypass = geo[dev].kbypass;
				//printf(" final ncmd %d cmd %04x %04x \n",ncmd,cmd[1],cmd[0]);
				//printf(" final nbuf %d nbuf %d \n",nbuf2,nbuf);
			} else {
				nbuf2 = nbuf;
				ncmd2 = ncmd;
				int k = ncmd2/8+1;
				if (k > 100) {
					XCEPT_RAISE(FEDException, "CATASTROPHIC ERROR IN DEVDO!");
				}
				for(int i = 0; i < k ; i++){
					cmd2[i]=cmd[i];
				}
			}
			// printf(" ********** %s dev prev_dev %d %d \n",geo[dev].nam,dev,prev_dev);
			// end stan kludge
		}
		
		if (ncmd == -99) {
			sleep_vme(cmd);
			break;
		} else if (ncmd < 0) {
			if (idev == 12) RestoreIdle(RESET);
			else RestoreIdle(NONE);
			break;
		} else if (ncmd > 0) {
			if(nbuf > 0){
				//if (idev == 12) scan_reset(INSTR_REG,cmd2,ncmd2,outbuf,0);
				//else 
				scan(dev,INSTR_REG,cmd2,ncmd2,outbuf,0);
			} else {
				//if (idev == 12) scan_reset(INSTR_REG,cmd2,ncmd2,outbuf,irdsnd);
				//else
				scan(dev,INSTR_REG,cmd2,ncmd2,outbuf,irdsnd);
			}
		}
		if (nbuf > 0) {
			if (idev == 12) scan(dev,DATA_REG,inbuf,nbuf,outbuf,irdsnd);
			else scan(dev,DATA_REG,inbuf,nbuf2,outbuf,irdsnd);
		}
		if (idev == 12) break;
		
		if (irdsnd == 1 && nbuf2%16) {
			int ishft = 16 - nbuf2%16;
			int temp = ((outbuf[nbuf2/8+1]<<8) & 0xff00) | (outbuf[nbuf2/8] & 0xff);
			temp = (temp>>ishft);
			outbuf[nbuf2/8+1] = (temp&0xff00)>>8;
			outbuf[nbuf2/8] = temp&0x00ff;
		}
		
		break;
		
	case 9:
		vmepara(cmd,inbuf,outbuf);
		break;
		
	case 10:
		vmeser(cmd,inbuf,outbuf);
		break;
		
	case 11:
		dcc(cmd,outbuf); 
		break;
		
	case 13:
		vme_adc(cmd[0],cmd[1],outbuf);
		break;
		
	default:
		break;
	
	}
	
}


void emu::fed::VMEModule::scan(DEVTYPE dev, int reg, const char *snd, int cnt, char *rcv, int ird)
	throw (FEDException)
{
	//int i;
	//int cnt2;
	
	//int byte,bit;
	//unsigned short int tmp[2]={0x0000};
	//unsigned short int *data;
	
	//unsigned short int *ptr_i;
	//unsigned short int *ptr_d;
	//unsigned short int *ptr_dh;
	//unsigned short int *ptr_ds;
	//unsigned short int *ptr_dt;
	//unsigned short int *ptr_r;
	
	// For later...
	unsigned short int tird[3] = {1,1,3};
	//unsigned short int tmp = 0;
	
	if (cnt == 0) return;
	if (ird < 0 || ird > 2) XCEPT_RAISE(FEDException, "ird out-of-bounds!");
	
	int cnt2 = cnt - 1;
	// printf(" ****** cnt cnt2 %d %d \n",cnt,cnt2);
	// printf(" reg ird %d %d \n",reg,ird);
	unsigned short int *data = (unsigned short int *) snd;
	
	// instr
	
	int idev = geo[dev].jchan;
	
	if (idev == 12) { // RESET
		unsigned long int add_reset = vmeadd_ | 0x0000fffe;
		//unsigned short int *ptr = (unsigned short int *) add_reset;
		
		if (reg == 0) {
			vme_controller(1,add_reset,0,rcv);
			vme_controller(1,add_reset,0,rcv);
			vme_controller(1,add_reset,1,rcv);
			vme_controller(1,add_reset,1,rcv);
			vme_controller(1,add_reset,0,rcv);
			vme_controller(1,add_reset,0,rcv);
		}
		
		//data
		
		else if (reg == 1) {
			vme_controller(1,add_reset,0,rcv);
			vme_controller(1,add_reset,0,rcv);
			vme_controller(1,add_reset,1,rcv);
			vme_controller(1,add_reset,0,rcv);
			vme_controller(1,add_reset,0,rcv);
		}
		
		int byte = cnt/16;
		int bit = cnt%16;
		int ival2 = 0;
		for (int i = 0; i < byte; i++) {
			for (int j = 0; j < 16; j++) {
				ival2 = ((*data) >> j) & 0x01;
				if (i != byte-1 || bit != 0 || j != 15) {
					if (ival2 == 0) {
						vme_controller(1,add_reset,0,rcv);
					}
					else if (ival2 == 1) {
						vme_controller(1,add_reset,2,rcv);
					}
				} else {
					if (ival2 == 0) {
						vme_controller(1,add_reset,1,rcv);
					} else if (ival2 == 1) {
						vme_controller(1,add_reset,3,rcv);
					}
				}
			}
			data++;
		}
		for (int j = 0; j < bit; j++) {
			ival2 = ((*data) >> j) & 0x01;
			if ( j < bit-1 ) {
				if (ival2 == 0) {
					vme_controller(1,add_reset,0,rcv);
				} else if (ival2 == 1) {
					vme_controller(1,add_reset,2,rcv);
				}
			} else {
				if (ival2 == 0) {
					vme_controller(1,add_reset,0,rcv);
				} else if (ival2 == 1) {
					vme_controller(1,add_reset,0,rcv);
				}
			}
		}
		vme_controller(1,add_reset,1,rcv);
		vme_controller(3,add_reset,0,rcv);
		return;
	}
	
	else if (reg == 0) {
		unsigned long int add_i = (vmeadd_ | (idev << 12) | 0x0000001c) & 0xfffff0ff;
		add_i |= (cnt2<<8);
		//unsigned short int *ptr_i = (unsigned short int*) add_i;
		//bit = cnt;
		// xif(bit>8)*ptr_i=*data;
		// xif(bit<=8)*ptr_i=((*data)>>8);
		// if(bit<=8)*data=((*data)>>8);
		// printf(" 1 VME W: %08x %04x \n",ptr_i,*data);
		vme_controller(tird[ird],add_i,*data,rcv);
		return;
	}
		
	//data 
	
	else if (reg == 1) {
		int byte = cnt/16;
		int bit = cnt%16;
		// printf(" bit byte %d %d \n",bit,byte);
		if (byte == 0 || (byte == 1 & bit == 0)) {
			unsigned long int add_d = (vmeadd_ | (idev << 12) | 0x0000000c) & 0xfffff0ff;
			add_d |= (cnt2<<8);
			//unsigned short int *ptr_d = (unsigned short int *) add_d; 
			// printf(" 2 VME W: %08x %04x \n",ptr_d,*data);
			// xif(bit>8|byte==1)*ptr_d=*data;
			// xif(bit<=8&byte!=1)*ptr_d=((*data)>>8);
			// if(bit<=8&byte!=1)*data=((*data)>>8);
			vme_controller(tird[ird],add_d,*data,rcv);
			//  printf("2 VME W: %08x %04x \n",ptr_dh,*data);
			if(ird == 1) {
				unsigned long int add_r = vmeadd_ | (idev << 12) | 0x00000014;
				//unsigned short int *ptr_r = (unsigned short int *) add_r;
				// x*data2=*ptr_r;
				// printf(" R %08x \n",ptr_r);
				vme_controller(2,add_r,0,rcv);
			}
			return;
		}
		
		
		unsigned long int add_dh = ((vmeadd_ | (idev << 12) | 0x00000004) & 0xfffff0ff) | 0x0f00;
		//unsigned short int *ptr_dh = (unsigned short int *) add_dh;
		// printf(" 3 VME W: %08x %04x \n",ptr_dh,*data);
		vme_controller(1,add_dh,*data,rcv);
		// x*ptr_dh=*data;
		data++;
		
		if (ird == 1) {
			unsigned long int add_r = vmeadd_ | (idev << 12) | 0x00000014;
			//unsigned short int *ptr_r = (unsigned short int *) add_r;
			// printf("3 R %08x \n",ptr_r);
			vme_controller(0,add_r,0,rcv);
			// x*data2=*ptr_r; 
			// printf(" rddata %04x \n",*data2);
		}
		
		for (int i=0; i < byte-1; i++){
			if (i == byte-2 && bit==0) {
				
				unsigned long int add_dt = ((vmeadd_ | (idev << 12) | 0x00000008) & 0xfffff0ff) | 0x0f00;
				//unsigned short int *ptr_dt = (unsigned short int *) add_dt;
				// printf("4 VME W: %08x %04x \n",ptr_dt,*data);
				vme_controller(tird[ird],add_dt,*data,rcv);
				// x*ptr_dt=*data;
				if (ird == 1) {
					unsigned long int add_r = vmeadd_ | (idev << 12) | 0x00000014;
					//unsigned short int *ptr_r = (unsigned short int *) add_r;
					//  printf("4 R %08x \n",ptr_r);
					vme_controller(2,add_r,*data,rcv);
					// x*data2=*ptr_r;  
					// printf(" rddata %04x \n",*data2);
				}
				return;
				
			} else {
				unsigned long int add_ds = ((vmeadd_ | (idev << 12) | 0x00000000) & 0xfffff0ff) | 0x0f00;
				//unsigned short int *ptr_ds = (unsigned short int *) add_ds;
				// printf("5 VME W: %08x %04x \n",ptr_ds,*data);
				vme_controller(1,add_ds,*data,rcv);
				// x*ptr_ds=*data;
				data++;
				
				if (ird == 1) {
					unsigned long int add_r = vmeadd_ | (idev << 12) | 0x00000014;
					//unsigned short int *ptr_r = (unsigned short int *) add_r;
					// printf(" R %08x \n",ptr_r);
					vme_controller(0,add_r,0,rcv);
					// x*data2=*ptr_r; 
					// printf(" rddata %04x \n",*data2);
				}
			}
		}
		
		cnt2 = bit-1;
		unsigned long int add_dt = (vmeadd_ | (idev << 12) | 0x00000008) & 0xfffff0ff;
		add_dt |= (cnt2 << 8);
		//unsigned short int *ptr_dt = (unsigned short int *) add_dt;
		// printf("6 VME W: %08x %04x \n",ptr_dt,*data);
		// xif(bit>8)*ptr_dt=*data;
		// xif(bit<=8)*ptr_dt=*data>>8;
		// if(bit<=8)*data=*data>>8;
		vme_controller(tird[ird],add_dt,*data,rcv);
		if (ird == 1) {
			unsigned long int add_r = vmeadd_ | (idev << 12) | 0x00000014;
			//unsigned short int *ptr_r = (unsigned short int *) add_r;
			// printf(" R %08x \n",ptr_r);
			vme_controller(2,add_r,0,rcv);
			// x*data2=*ptr_r; 
			// printf(" rddata %04x \n",*data2);
		}
		
		return;
	}
}


/*
void emu::fed::VMEModule::handshake_vme()
{
	unsigned long int add_control_r = 0x00082800;
	unsigned short int *ptr = (unsigned short int *) add_control_r;
	vme_controller(4,ptr,0,0); // flush
	vme_controller(5,ptr,0,0); // handshake
}
*/

/*
void emu::fed::VMEModule::flush_vme()
{
	unsigned short int *ptr;
	// printf(" flush buffers to VME \n");
	vme_controller(4,ptr,0,0); // flush
}
*/


void emu::fed::VMEModule::vmeser(const char *cmd,const char *snd,char *rcv)
{

	// For later
	short int seri[16] = {2,2,0,2,4,6,0,2,0,2,0,0,4,6,0,2};
	
// 	void emu::fed::VMEController::vme_controller(int irdwr,unsigned short int *ptr,unsigned short int *data,char *rcv)
// 	irdwr:   
// 		0 bufread
// 		1 bufwrite 
// 		2 bufread snd  
// 		3 bufwrite snd 
// 		4 flush to VME
// 		5 loop back 
// 		6 delay

	unsigned long int add_vmesert = (vmeadd_ | 0x00040000) + ((cmd[0] & 0x000f) << 12) + ((cmd[1] & 0x000f) << 2);
	//unsigned short int *ptr = (unsigned short int *) add_vmesert;
	
	unsigned short int icmd = cmd[1] & 0x000f; //DDU command
	unsigned short int iadr = cmd[0] & 0x000f; //DDU device
	
	if ((icmd > 8 && iadr == 4) || (iadr >= 8)) { //write, but ignore snd data
		vme_controller(3,add_vmesert,0,rcv);
		
	} else if (icmd < 9 && iadr == 4) { //read
		int nt = seri[icmd]/2;
		for (int i = 0; i < nt-1; i++) {
			vme_controller(0,add_vmesert,0,rcv);
		}

		vme_controller(2,add_vmesert,0,rcv);
		
		// printf(" scan vmeser: return from read, nrcv=%d,",nrcv);
		for (int i = 0; i < nt; i++) {
			// Endian swap
			char tr = rcv[2*i];
			rcv[2*i] = rcv[(2*i)+1];
			rcv[2*i+1] = tr;
			// printf(" %02x %02x",rcv[2*i]&0xff,rcv[(2*i)+1]&0xff);
		}
		
	} else if (iadr < 4) { //read
		vme_controller(0,add_vmesert,0,rcv);
		vme_controller(2,add_vmesert,0,rcv);

		// printf(" scan vmeser: return from read, nrcv=%d,",nrcv);
		for (int i = 0; i < 2; i++) {
			// Endian swap
			char tr = rcv[2*i];
			rcv[2*i] = rcv[(2*i)+1];
			rcv[2*i+1] = tr;
			// printf(" %02x %02x",rcv[2*i]&0xff,rcv[(2*i)+1]&0xff);
		}
	}
	return;
}



void emu::fed::VMEModule::vmepara(const char *cmd, const char *snd, char *rcv)
{

	unsigned short int icmd = cmd[1] & 0x00ff;  //0-127 read, >=128 write
	unsigned short int idev = cmd[0] & 0x000f;  //0-7 CMD ignored, >=8 CMD req'd.
	unsigned long int add_vmepart = (vmeadd_ | 0x00030000) + (idev << 12) + (icmd << 2);
	//unsigned short int *ptr = (unsigned short int *) add_vmepart;
	
	//JRG, added Write case:
	if (icmd > 127 && idev >= 8) {  //Write
/* The code that was here was so hilariously ineffective (as in, it did nothing
at all) that, instead of forcing you to suffer by reading it, I will instead
give you a picture of DANGERMAN!

  .... .... .........N+??O. .. .....  ..... ...
  .  . .  . . ....M?????????+ .    .   .    .  
  .. . . .. ...M???????????????=....        .  
  .. . . ...MI????????????????????+..  .    .  
  .. . ..M???????????????????????????M..    .  
  .....M?????????????????????????????????M.. . .
   +M???????????????????????????????????????O...
  ZO?????????????????????????????????????????+OM
  M??MI??????????????????????????????????+IM??MM
  MM??MM8N?????????????????????????????7?MM??NMM
  MMM??MMO?+O???????????????????????ZM??MMM?$MMM
  MMMM??MM??MMMN?????????????????MIMM?+MM+??MM?M
  MMMM??ZMM??MMM?=MM???M?????+MM?IMM+?MMM??+???M
  +MMMM??MMM?+MMM?NMMMMMM??MMMM??MMM?+MM???????M
  ??MMMM????M?+MM??MMMMMMM?+MM+?MMM??M?????????M
  ???MMMM??????MMMMMMMMMM??MMM?DMM?????????????M
  ???MMMM??????MMMMMMMMMM+MMM?+I???????????????M
  ????M?Z??????MMMMMM7MM+MMM???????????????????M
  ??????????????MMM????MMM?????????????????????M
  ??????????????????????+??????????????????????M
  ??????????????????????+??????????????????????M
  ??????????77777MMMMMMM8??????????????????????M
  ??IMMMMMMMMMMMMMMMMM??+??????????????????????M
  ????+MMMMMMMMMMMMMZ???+??????????????????????M
  M??????MMMMMMMMM+?????+??????????????????????M
  MD?IM????MMMMMI???????+????????????????NMM??MM
  MM??+MM????M+?????????+????????????????MM??MMM
  +MM??MMM??MM??????????+??????????+MM??MM+?8MMM
  ..MM??MM+?+MM?????????+?????????MMM??MM+?+M...
      .MIMM+?8MM??MM+???+????=MI?MMM??MMMM..... 
  ........MM??MMM?NMMD??++??MMM?+MM+?MMM........
            .$?MM+?MMM?MMM?MMM??MMN?... ... ... 
  .............MMM+?MMMI??+MM+?MMM..... ... ....
      .. .      .MM+MM?+?MM+?...  .... ... ... 
  .   .  . .   .....~MM7MMM... .. .... ... ....
  .. ... . .  .   ..   M

*/
		
		vme_controller(3,add_vmepart,*snd,rcv);
		//JRG, end Write case ^^^^
	} else { //Read
		vme_controller(2,add_vmepart,0,rcv);
		// printf(" scan vmepar: return from read, nrcv=%d,",nrcv);
		// for(i=0;i<nrcv;i++)printf(" %02x",rcv[i]&0xff);
		// printf("\n");
	}
}



void emu::fed::VMEModule::dcc(const char *cmd,char *rcv)
{
	
	unsigned long int add = vmeadd_ + (cmd[1] << 2);
	//unsigned short int *ptr = (unsigned short int *) add;
	// n = readline(sockfd,cmd,4); 
	// printf(" dcc: cmd %02x %02x %02x %02x \n",cmd[0]&0xff,cmd[1]&0xff,cmd[2]&0xff,cmd[3]&0xff);
	//tcmd=cmd[1]<<2;
	if (cmd[0] == 0x00) {
		char c[2] = {cmd[3], cmd[2]};
		unsigned short int *data = (unsigned short int *) c;
		vme_controller(3,add, *data,rcv);
		// printf(" dcc write: ptr %08x data %04x \n",ptr,*data);
		// printf(" about to write \n");
		// *ptr=*data;
	} else if (cmd[0] == 0x01 ) {
		// data2=rcv;
		//unsigned short int *data;
		vme_controller(2,add,0,rcv);
		// *data2=*ptr; 
		// printf(" dcc read: ptr %08x data %04x \n",ptr,*data2);
		// n = writen(sockfd,rcv,2);
	}
}



void emu::fed::VMEModule::vme_adc(int ichp, int ichn, char *rcv)
{
	
	// Lookup tables.
	char adcreg[5]={0x00,0x0e,0x0d,0x0b,0x07};
	char adcbyt[3][8]={{0x89,0x99,0xa9,0xb9,0xc9,0xd9,0xe9,0xf9},{0x89,0x99,0xa9,0xb9,0xc9,0xd9,0xe9,0xf9},{0x85,0x95,0xa5,0xb5,0xc5,0xd5,0xe5,0xf5}};
	// printf(" enter vme_adc \n");
	
	unsigned short int val[2] = {adcreg[ichp], adcbyt[ichp-1][ichn]};
	//unsigned short int tmp[2] = {0x0000,0x0000};
	
	if (val[0] == 0) {
		unsigned long int add_adcrs = vmeadd_ | 0x0000d000 | 0x00000024;
		//unsigned short int *ptr = (unsigned short int *) add_adcrs;
		vme_controller(2,add_adcrs,0,rcv); // *ptr=*data;
		return;
	}
	// ptr=(unsigned short int *)add_adcws;
	// printf(" select register%08x  %02x \n",ptr,val[0]&0xff);
	// vme_controller(3,ptr,&val[0],rcv);
	// *ptr=val[0];
	unsigned long int add_adcw = vmeadd_ | 0x0000d000 | 0x00000020;
	//unsigned short int *ptr = (unsigned short int *) add_adcw;
	
	// printf(" adc write %08x %02x \n",ptr,val[1]&0xff);
	vme_controller(3,add_adcw,val[1],rcv); // *ptr=val[1];
	
	unsigned long int add = vmeadd_ | 0x0000d000;
	if (val[0] != 0x07) {
		add |= 0x00000004;
	} else {
		add |= 0x0000000c;
	}
	vme_controller(2,add,0,rcv); // *data=*ptr;
	
	// print(" adc read %08x %02x %02x\n",ptr,rbuf[1]&0xff,rbuf[0]&0xff);
}



void emu::fed::VMEModule::RestoreIdle(enum DEVTYPE dv)
{
	char tmp[2] = {0x00,0x00};
	
	if (dv == RESET) {
		unsigned long int address = (vmeadd_ | 0x0000fffe);
		//ptr = (unsigned short int *) address;
		for (int i = 0; i < 5; i++) {
			vme_controller(1,address,1,tmp);
		}
	} else {
		unsigned long int address = (vmeadd_ | 0x00001000 | 0x00000018);
		vme_controller(3,address,0,tmp);
	}
}
