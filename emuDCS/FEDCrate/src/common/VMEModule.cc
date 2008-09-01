/*****************************************************************************\
* $Id: VMEModule.cc,v 3.12 2008/09/01 23:46:24 paste Exp $
*
* $Log: VMEModule.cc,v $
* Revision 3.12  2008/09/01 23:46:24  paste
* Trying to fix what I broke...
*
* Revision 3.11  2008/09/01 11:30:32  paste
* Added features to DDU, IRQThreads corresponding to new DDU firmware.
*
* Revision 3.10  2008/08/31 21:18:27  paste
* Moved buffers from VMEController class to VMEModule class for more rebust communication.
*
* Revision 3.9  2008/08/30 14:49:04  paste
* Attempts to make VME work under the new design model where VMEModules take over for the VMEController.
*
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
#include "VMEController.h"

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
	/*
	std::cout << "*** CAEN_read with:" << std::endl;
	std::cout << "Address: " << std::hex << Address << std::endl;
	std::cout << "data: " << std::hex << *data << std::endl;
	std::cout << "BHandle " << std::dec << BHandle_ << std::endl;
	*/
	
	CVAddressModifier AM = cvA24_U_DATA;
	CVDataWidth DW = cvD16;
	int err = CAENVME_ReadCycle(BHandle_,Address,data,AM,DW);
	if (err) {
		std::ostringstream error;
		//std::cout << "Sleeping due to error " << err << std::endl;
		//sleep((unsigned int) 3);
		error << "CAENVME read error " << err;
		std::cerr << " #&$%! " << error.str() << std::endl;
		//XCEPT_RAISE(FEDException, error.str());
	}
	
	//std::cout << "Returning data: " << std::hex << *data << std::endl << std::endl;
	
}



unsigned long int emu::fed::VMEModule::CAEN_read(unsigned long Address)
throw (FEDException)
{
	/*
	std::cout << "CAEN_read with:" << std::endl;
	std::cout << "Address: " << std::hex << Address << std::endl;
	std::cout << "BHandle " << std::dec << BHandle_ << std::endl;
	*/
	char readData[4];
	CVAddressModifier AM = cvA24_U_DATA;
	CVDataWidth DW = cvD16;
	int err = CAENVME_ReadCycle(BHandle_,Address,readData,AM,DW);
	//std::cout << "Read returned " << std::hex << (int) *readData << " with error " << err << std::endl;
	if (err) {
		std::ostringstream error;
		//std::cout << "Sleeping due to error " << err << std::endl;
		//sleep((unsigned int) 3);
		error << "CAENVME read error " << err;
		std::cerr << " #&$%! " << error.str() << std::endl;
		//XCEPT_RAISE(FEDException, error.str());
	}
	unsigned long int retData = readData[0] | (readData[1] << 8) | (readData[2] << 16) | (readData[3] << 24);
	return retData;
}



void emu::fed::VMEModule::CAEN_write(unsigned long Address,unsigned short int *data)
	throw (FEDException)
{
	/*
	std::cout << "CAEN_write with:" << std::endl;
	std::cout << "Address: " << std::hex << Address << std::endl;
	std::cout << "data: " << std::hex << *data << std::endl;
	std::cout << "BHandle " << std::dec << BHandle_ << std::endl;
	*/
	CVAddressModifier AM = cvA24_U_DATA;
	CVDataWidth DW = cvD16;
	int err = CAENVME_WriteCycle(BHandle_, Address, (char *)data, AM, DW);
	if (err) {
		std::ostringstream error;
		//std::cout << "Sleeping due to error " << err << std::endl;
		//sleep((unsigned int) 3);
		error << "CAENVME write error " << err;
		std::cerr << " #&$%! " << error.str() << std::endl;
		//XCEPT_RAISE(FEDException, error.str());
	}
}



void emu::fed::VMEModule::CAEN_write(unsigned long Address, int16_t data)
	throw (FEDException)
{
	/*
	std::cout << "CAEN_write with:" << std::endl;
	std::cout << "Address: " << std::hex << Address << std::endl;
	std::cout << "data: " << std::hex << data << std::endl;
	std::cout << "BHandle " << std::dec << BHandle_ << std::endl;
	*/
	CVAddressModifier AM = cvA24_U_DATA;
	CVDataWidth DW = cvD16;
	int err = CAENVME_WriteCycle(BHandle_, Address, &data, AM, DW);
	if (err) {
		std::ostringstream error;
		//std::cout << "Sleeping due to error " << err << std::endl;
		//sleep((unsigned int) 3);
		error << "CAENVME write error " << err;
		std::cerr << " #&$%! " << error.str() << std::endl;
		//XCEPT_RAISE(FEDException, error.str());
	}
}


/*
void emu::fed::VMEModule::vme_controller(int irdwr, unsigned short int *ptr, unsigned short int *data, char *rcv)
{
	//printf("vme_controller with irdwr %d ptr %08x data %04x rcv %08x\n",irdwr,ptr,*data,rcv);

	// LOG4CPLUS_INFO(getApplicationLogger(), " EmuFEDVME: Inside controller");
	
	char rdata[2];
	//std::cout << " rdata " << (int) rdata << std::endl;
	//std::cout << " rdata[0] " << std::hex << (int) rdata[0] << std::dec << std::endl;
	//std::cout << " rdata[1] " << std::hex << (int) rdata[1] << std::dec << std::endl;
	
	long unsigned int pttr = (long unsigned int) ptr;
	
	if (irdwr == 0) { // read
		CAEN_read(pttr,(unsigned short int *) rdata);
		rcv[0] = rdata[0];
		rcv[1] = rdata[1];
	} else if (irdwr == 1) { // write
		CAEN_write(pttr,data);
	} else if (irdwr == 2) { // read
		CAEN_read(pttr,(unsigned short int *) rdata);
		rcv[0] = rdata[0];
		rcv[1] = rdata[1];
	} else if (irdwr == 3) { // write
		CAEN_write(pttr,data);
	} else if (irdwr == 6) { // delay
		long int packet_delay = (long int) ((*data)*DELAY3);
		udelay_(packet_delay);
	}
}
*/

void emu::fed::VMEModule::vme_controller(int irdwr,unsigned short int *ptr,unsigned short int *data,char *rcv)
{
	static int ird = 0;
	char rdata[2];

	// LOG4CPLUS_INFO(getApplicationLogger(), " EmuFEDVME: Inside controller");
	long unsigned int pttr = (long unsigned int) ptr;
	if (irdwr == 0) { // read
		CAEN_read(pttr,(unsigned short int *) rdata);
		rcv[ird] = rdata[0];
		ird++;
		rcv[ird] = rdata[1];
		ird++;
	} else if (irdwr == 1) {
		CAEN_write(pttr,data);
	} else if (irdwr == 2) { // read final word
		CAEN_read(pttr,(unsigned short int *) rdata);
		rcv[ird] = rdata[0];
		ird++;
		rcv[ird] = rdata[1];
		ird = 0;
	} else if (irdwr == 3) {
		CAEN_write(pttr,data);
	} else if (irdwr == 6) {
		long int packet_delay = (long int) ((*data)*DELAY3);
		// printf(" packet_delay %d %ld \n",*data,packet_delay);
		udelay_(packet_delay);
	}

}


void  emu::fed::VMEModule::sleep_vme(const char *outbuf)   // in usecs (min 16 usec)
{
	unsigned short int delay = (unsigned short int) ((((outbuf[1]<<8)&0xff00) + (outbuf[0]&0xff)) / 16.0 + 1);
	udelay_((long int) (delay * DELAY3));
}


/*
void emu::fed::VMEModule::devdo(DEVTYPE dev,int ncmd,const char *cmd,int nbuf, const char *inbuf,char *outbuf,int irdsnd)
	throw (FEDException)
{
	
	std::cout << "devdo called with" << std::endl;
	std::cout << "dev: " << dev << std::endl;
	std::cout << "ncmd: " << ncmd << std::endl;
	std::cout << "cmd[0]: " << std::hex << (int) cmd[0] << std::endl;
	std::cout << "cmd[1]: " << std::hex << (int) cmd[1] << std::endl;
	std::cout << "nbuf: " << std::dec << nbuf << std::endl;
	std::cout << "inbuf[0]: " << std::hex << (int) inbuf[0] << std::endl;
	std::cout << "inbuf[1]: " << std::hex << (int) inbuf[1] << std::endl;
	std::cout << "irdsnd: " << std::dec << irdsnd << std::endl;
	
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
*/


void emu::fed::VMEModule::devdo(enum DEVTYPE dev,int ncmd,const char *cmd,int nbuf,const char *inbuf,char *outbuf,int irdsnd)
	throw (FEDException)
{
	/*
	std::cout << "-------------" << std::endl;
	std::cout << "devdo called with" << std::endl;
	//std::cout << "inbuf: " << std::dec << (int) inbuf << std::endl;
	std::cout << "inbuf[0]: " << std::hex << (int) inbuf[0] << std::endl;
	std::cout << "inbuf[1]: " << std::hex << (int) inbuf[1] << std::endl;
	//std::cout << "outbuf: " << std::dec << (int) outbuf << std::endl;
	std::cout << "outbuf[0]: " << std::hex << (int) outbuf[0] << std::endl;
	std::cout << "outbuf[1]: " << std::hex << (int) outbuf[1] << std::endl;
	*/
	char cmd2[1024];
	
	int ncmd2;
	int nbcmd2;
	int nbuf2;

	/* irdsnd for jtag
	irdsnd = 0 send immediately, no read
	irdsnd = 1 send immediately, read
	irdsnd = 2 send in buffer, no read
	*/
	int idev = 0;
	if (dev != 99) {
		idev = geo[dev].jchan;
	} else {
		idev = idevo_;
		if (idev > 4 && idev != 12) return;
	}
	// printf(" enter devdo %d %d \n",dev,idev);
	
	// printf(" idev idevo_ dev %d %d %d \n",idev,idevo_,dev);
	/****** check we have same old device otherwise we need to initialize */
	bool init = false;
	if (idev != idevo_) {
		init = true;
	}
	idevo_ = idev;

//  printf(" about to initialize plev idve devo init %d %d %d %d \n",plev,idev,idevo_,init);
/************  JTAG initialize ******************/
/************  immediate instruction nonJTAG ****/

	switch (idev) {
	
	case 1:  /* JTAG */
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
		// All taken care of in VMEModule::scan(), etc.
		/*
		if (init) {
			mask = (idev << 12);
			add_i = vmeadd|msk01|msk_i;
			add_d=vmeadd|msk01|msk_d;
			add_dh=vmeadd|msk01|msk_dh;
			add_ds=vmeadd|msk01|msk_ds;
			add_dt=vmeadd|msk01|msk_dt;
			add_rst=vmeadd|msk01|msk_rst;
			add_sw=vmeadd|msk01|msk_sw;
			add_sr=vmeadd|msk01|msk_sr;
			add_r=vmeadd|msk01|msk_r;
		}
		*/
		break;
	
	case 9:
		//add_vmepara=vmeadd|msk_vmepara;
		vmepara(cmd,inbuf,outbuf);
		break;
	
	case 10: 
		//add_vmeser=vmeadd|msk_vmeser;
		vmeser(cmd,inbuf,outbuf);
		break;
	
	case 11: 
		/*
		add_dcc_r=vmeadd|msk00|msk_dcc_r;
		add_dcc_w=vmeadd|msk00|msk_dcc_w;
		*/
		dcc(cmd,outbuf);
		break;
	
	case 12:   /* RESET emergency VME PROM loading */
		// All taken care of in VMEModules::scan(), etc.
		/*
		if(init==1){
			add_reset=vmeadd|msk0f;
		}
		*/
		break;
	
	case 13:
		/*
		add_adcr=vmeadd|msk0d|msk_adcr;
		add_adcw=vmeadd|msk0d|msk_adcw;
		add_adcrbb=vmeadd|msk0d|msk_adcrbb;
		add_adcrs=vmeadd|msk0d|msk_adcrs; 
		add_adcws=vmeadd|msk0d|msk_adcws; 
		*/
		vme_adc(cmd[0],cmd[1],outbuf);
		break;
		
	default:
		std::ostringstream error;
		error << "devdo idev not understood: " << idev;
		XCEPT_RAISE(FEDException, error.str());
	}

	/**********  end initialize ***********************/
	/**********   send the JTAG data ************************/ 
	if (idev <= 8 || idev == 12) {
		if (ncmd > 0) {
			/* stan jtag kludge kludge for serial devices */
			if (geo[dev].nser != 0) {
			
				cmd2[0] = 0x00;
				cmd2[1] = 0x00;
				char tmp = 0;
				int pcmd = 0;
				
				for (int i = 0; i < geo[dev].nser; i++) {
					if (geo[dev].seri[geo[dev].nser-i-1] < 0) {
						tmp = cmd[0];
						pcmd = geo[-1*geo[dev].seri[geo[dev].nser-i-1]].kbit;
					} else {
						tmp = geo[geo[dev].seri[geo[dev].nser-i-1]].kbypass;
						pcmd = geo[geo[dev].seri[geo[dev].nser-i-1]].kbit;
					}
					for (int k = 0; k < pcmd; k++) {
						if (((tmp >> k) & 0x01) != 0) {
							if (geo[dev].nser < 9) {
								cmd2[0] += pows_(2,k-1);
							} else {
								cmd2[1] += pows_(2,k-9);
							}
						}
					}
				}
				ncmd2 = pcmd;
				nbcmd2 = ncmd2/8+1;
				nbuf2 = geo[dev].sxtrbits;
				if (nbuf > 0) {  
					nbuf2 += nbuf;
				} else {
					nbuf2 = 0;
				}

			} else {
				nbuf2 = nbuf;
				ncmd2 = ncmd;
				int k = ncmd2/8+1;
				if (k >= 32) XCEPT_RAISE(FEDException, "Catastrophic failure in devdo!");
				for (int i = 0; i < k; i++) {
					cmd2[i] = cmd[i];
				}
			}
			/* end stan kludge */
		}
	}

	switch (idev) {
	
	case 1: /* jtag feboards */
	case 2: /* jtag motherboard cntrl */
	case 3: /* jtag motherboard prom */
	case 4: /* jtag vme-mthrbrd prom */
	case 5: /* jtag */
	case 6: /* jtag */
	case 7: /* jtag */
	case 8: /* jtag */
	
		if (ncmd == -99) {
			sleep_vme(cmd);
			break;
		}
		if (ncmd < 0) {
			RestoreIdle(dev);
			break;
		}
		if (ncmd > 0) {
			if (nbuf > 0){
				//std::cout << "Scanning 1!" << std::endl;
				scan(dev,INSTR_REG,cmd2,ncmd2,outbuf,0);
			} else {
				//std::cout << "Scanning 2!" << std::endl;
				scan(dev,INSTR_REG,cmd2,ncmd2,outbuf,irdsnd);
			}
		}
		
		if (nbuf > 0) {
			//std::cout << "Scanning 3!" << std::endl;
			scan(dev,DATA_REG,inbuf,nbuf2,outbuf,irdsnd);
		}
		
		// Endian bit-swapping
		if (irdsnd == 1 && nbuf2 % 16 != 0) {
			//std::cout << "Swapping!" << std::endl;
			unsigned short int ishft = 16 - nbuf2 % 16;
			unsigned short int temp = ((outbuf[nbuf2/8+1]<<8)&0xff00) | (outbuf[nbuf2/8]&0xff);
			temp = (temp >> ishft);
			outbuf[nbuf2/8+1] = (temp&0xff00)>>8;
			outbuf[nbuf2/8] = temp&0x00ff;
		}
	
		break;
		
	case 12: /* reset vme  prom */
		// printf(" reset vme prom ncmd2 %d %d nbuf2 %d \n",ncmd2,ncmd,nbuf2);     
		if (ncmd == -99) {
			sleep_vme(cmd);
			break;
		}
		if (ncmd < 0) {
			RestoreIdle(dev);
			break;
		}
		if (ncmd>0) {
			if (nbuf>0) {
				scan(dev,INSTR_REG,cmd2,ncmd2,outbuf,0);
			} else {
				scan(dev,INSTR_REG,cmd2,ncmd2,outbuf,irdsnd);
			}
		}
		
		if (nbuf > 0) scan(dev,DATA_REG,inbuf,nbuf,outbuf,irdsnd);
		
		break;
		
	default:
		break;

	}
	/*************  end of senddata **********************/
	/*
	std::cout << "-------------" << std::endl;
	//std::cout << "inbuf: " << std::dec << (int) inbuf << std::endl;
	std::cout << "inbuf[0]: " << std::hex << (int) inbuf[0] << std::endl;
	std::cout << "inbuf[1]: " << std::hex << (int) inbuf[1] << std::endl;
	//std::cout << "outbuf: " << std::dec << (int) outbuf << std::endl;
	std::cout << "outbuf[0]: " << std::hex << (int) outbuf[0] << std::endl;
	std::cout << "outbuf[1]: " << std::hex << (int) outbuf[1] << std::dec << std::endl;
	*/
}

/*
void emu::fed::VMEModule::scan(DEVTYPE dev, int reg, const char *snd, int cnt, char *rcv, int ird)
	throw (FEDException)
{

	std::cout << "scan called with the following:" << std::endl;
	std::cout << "dev " << dev << std::endl;
	std::cout << "reg " << std::hex << reg << std::dec << std::endl;
	std::cout << "snd[0] " << std::hex << (int) snd[0] << std::dec << std::endl;
	std::cout << "snd[1] " << std::hex << (int) snd[1] << std::dec << std::endl;
	std::cout << "cnt " << std::hex << cnt << std::dec << std::endl;
	std::cout << "rcv[0] " << std::hex << (int) rcv[0] << std::dec << std::endl;
	std::cout << "rcv[1] " << std::hex << (int) rcv[1] << std::dec << std::endl;
	std::cout << "ird " << std::hex << ird << std::dec << std::endl;
	
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
	//unsigned short int tird[3] = {1,1,3};
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
		//vme_controller(tird[ird],add_i,*data,rcv);
		vme_controller(1,add_i,*data,rcv);
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
			//vme_controller(tird[ird],add_d,*data,rcv);
			vme_controller(1,add_d,*data,rcv);
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
				//vme_controller(tird[ird],add_dt,*data,rcv);
				vme_controller(1,add_dt,*data,rcv);
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
		//vme_controller(tird[ird],add_dt,*data,rcv);
		vme_controller(1,add_dt,*data,rcv);
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
*/

void emu::fed::VMEModule::scan(enum DEVTYPE dev, int reg, const char *snd, int cnt, char *rcv, int ird)
	throw (FEDException)
{
	/*
	std::cout << "%%%%%%%%%%%%%%" << std::endl;
	std::cout << "scan called with" << std::endl;
	std::cout << " dev " << dev << std::endl;
	std::cout << " reg " << reg << std::endl;
	std::cout << " snd " << (int) snd << std::endl;
	std::cout << " snd[0] " << std::hex << (int) snd[0] << std::dec << std::endl;
	std::cout << " snd[1] " << std::hex << (int) snd[1] << std::dec << std::endl;
	std::cout << " cnt " << cnt << std::endl;
	std::cout << " rcv " << (int) rcv << std::endl;
	std::cout << " rcv[0] " << std::hex << (int) rcv[0] << std::dec << std::endl;
	std::cout << " rcv[0] " << std::hex << (int) rcv[1] << std::dec << std::endl;
	std::cout << " ird " << ird << std::endl;
	*/
	unsigned short int tmp[2] = {0x0000};

	int tird = 0;
	if (ird == 0 || ird == 1) {
		tird = 1;
	} else if (ird == 2) {
		tird = 3;
	} else {
		std::ostringstream error;
		error << "scan invalid ird " << ird;
		XCEPT_RAISE(FEDException, error.str());
	}

	if (cnt == 0) return;

	int idev = geo[dev].jchan;
	
	if (idev == 12) { // RESET!

		unsigned short int x00[1]={0x00};
		unsigned short int x01[1]={0x01};
		unsigned short int x02[1]={0x02};
		unsigned short int x03[1]={0x03};

		unsigned long int add_reset = vmeadd_ | 0xfffe;
		unsigned short int *ptr = (unsigned short int *) add_reset;
		unsigned short int *data = (unsigned short int *) snd;

		/* instr */
		
		if (reg == 0) {
			vme_controller(1,ptr,x00,rcv);
			vme_controller(1,ptr,x00,rcv);
			vme_controller(1,ptr,x01,rcv);
			vme_controller(1,ptr,x01,rcv);
			vme_controller(1,ptr,x00,rcv);
			vme_controller(1,ptr,x00,rcv);
		}
		
		/* data */
		
		if (reg == 1) {
			vme_controller(1,ptr,x00,rcv);
			vme_controller(1,ptr,x00,rcv);
			vme_controller(1,ptr,x01,rcv);
			vme_controller(1,ptr,x00,rcv);
			vme_controller(1,ptr,x00,rcv);
		}
		
		unsigned int byte = cnt/16;
		unsigned short int bit = cnt%16;
		
		for (unsigned int i = 0; i < byte; i++) {
			for (unsigned int j = 0; j < 16; j++) {
				if (i != byte-1 || bit != 0 || j != 15) {
					if ((*data >> j) & 0x01) {
						vme_controller(1,ptr,x02,rcv);
					} else {
						vme_controller(1,ptr,x00,rcv);
					}
				} else {
					if ((*data >> j) & 0x01) {
						vme_controller(1,ptr,x03,rcv);
					} else {
						vme_controller(1,ptr,x01,rcv);
					}
				}
			}
			data++;
		}
		
		for (unsigned short int j = 0; j < bit; j++) {
			if (j < bit - 1) {
				if ((*data >> j) & 0x01) {
					vme_controller(1,ptr,x02,rcv);
				} else {
					vme_controller(1,ptr,x00,rcv);
				}
				
			} else {
				if ((*data >> j) & 0x01) {
					vme_controller(1,ptr,x03,rcv);
				} else {
					vme_controller(1,ptr,x01,rcv);
				}
			}
		}
		
		vme_controller(1,ptr,x01,rcv);
		vme_controller(3,ptr,x00,rcv);
		
		return;
	}
	
	// Not RESET!
	unsigned long int mask = (idev << 12);

	int cnt2 = cnt - 1;
	unsigned short int *data = (unsigned short int *) snd;

	/* instr */

	if (reg == 0) {
		unsigned long int add_i = (vmeadd_ | mask | 0x1c) & 0xfffff0ff;
		add_i |= (cnt2 << 8);
		unsigned short int *ptr_i = (unsigned short int *) add_i;
		vme_controller(tird,ptr_i,data,rcv);
		return;
	}

	/* data */
	
	if (reg == 1) {
		unsigned int byte = cnt/16;
		unsigned short int bit = cnt%16;

		if (byte == 0 || (byte == 1 && bit == 0)) {
			unsigned long int add_d = (vmeadd_ | mask | 0x0c) & 0xfffff0ff;
			add_d |= (cnt2 << 8);
			unsigned short int *ptr_d = (unsigned short int *) add_d;
			vme_controller(tird,ptr_d,data,rcv);
			if (ird == 1) {
				unsigned long int add_r = (vmeadd_ | mask | 0x14);
				unsigned short int *ptr_r = (unsigned short int *) add_r;
				vme_controller(2,ptr_r,tmp,rcv);
			}
			return;
		}
		
		unsigned long int add_dh = (vmeadd_ | mask | 0x04) & 0xfffff0ff;
		add_dh |= 0x0f00;
		unsigned short int *ptr_dh = (unsigned short int *) add_dh;
		vme_controller(1,ptr_dh,data,rcv);
		data++;
		
		if (ird == 1) {
			unsigned long int add_r = (vmeadd_ | mask | 0x14);
			unsigned short int *ptr_r = (unsigned short int *) add_r;
			vme_controller(0,ptr_r,tmp,rcv);
		}
		
		for (unsigned int i = 0; i < byte - 1; i++) {
			
			if (i == byte - 2 && bit == 0) {
				unsigned long int add_dt = (vmeadd_ | mask | 0x08) & 0xfffff0ff;
				add_dt |= 0x0f00;
				unsigned short int *ptr_dt = (unsigned short int *) add_dt;
				vme_controller(tird,ptr_dt,data,rcv);
				
				if (ird == 1) {
					unsigned long int add_r = (vmeadd_ | mask | 0x14);
					unsigned short int *ptr_r = (unsigned short int *) add_r;
					vme_controller(2,ptr_r,data,rcv);
				}
				return;
				
			} else {
				
				unsigned long int add_ds = (vmeadd_ | mask) & 0xfffff0ff;
				add_ds |= 0x0f00;
				unsigned short int *ptr_ds = (unsigned short *) add_ds;
				vme_controller(1,ptr_ds,data,rcv);
				data++;
				
				if (ird == 1) {
					unsigned long int add_r = (vmeadd_ | mask | 0x14);
					unsigned short int *ptr_r = (unsigned short int *) add_r;
					vme_controller(0,ptr_r,tmp,rcv);
				}
			}
		}
		
		cnt2 = bit-1;
		unsigned long int add_dt = (vmeadd_ | mask | 0x08) & 0xfffff0ff;
		add_dt |= (cnt2 << 8);
		unsigned short int *ptr_dt = (unsigned short int *) add_dt;
		vme_controller(tird,ptr_dt,data,rcv);
		
		if(ird==1){
			unsigned long int add_r = (vmeadd_ | mask | 0x14);
			unsigned short int *ptr_r = (unsigned short int *) add_r;
			vme_controller(2,ptr_r,tmp,rcv);
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

/*
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
*/

void emu::fed::VMEModule::vmeser(const char *cmd,const char *snd,char *rcv)
{
	short int seri[16] = {2,2,0,2,4,6,0,2,0,2,0,0,4,6,0,2};
	
	unsigned short int tmp[1]={0x0000};
	/*
	irdwr:   
	0 bufread
	1 bufwrite 
	2 bufread snd  
	3 bufwrite snd 
	4 flush to VME
	5 loop back 
	6 delay
	*/
	unsigned short int icmd = (cmd[1] & 0x000f);
	unsigned short int iadr = (cmd[0] & 0x000f);
	unsigned long int add_vmeser = (vmeadd_ | 0x00040000) + (iadr << 12) + (icmd << 2);
	unsigned short int *ptr = (unsigned short int *) add_vmeser;
	
	if ((icmd > 8 && iadr == 4) || (iadr >= 8)) { //write, but ignore snd data
		
		vme_controller(3,ptr,tmp,rcv);
		
	} else if ((icmd < 9 && iadr == 4) || iadr < 4) { //read
		
		int times = (iadr == 4 ? seri[icmd]/2 : 2);
		for (int i = 0; i < times - 1; i++) {
			vme_controller(0,ptr,tmp,rcv);
		}
		vme_controller(2,ptr,tmp,rcv);
		
		for (int i = 0; i < times; i++) { // Endian swap
			char tr = rcv[2*i];
			rcv[2*i] = rcv[(2*i)+1];
			rcv[2*i+1] = tr;
		}
		
	}
	return;
}


void emu::fed::VMEModule::vmepara(const char *cmd, const char *snd, char *rcv)
{

	unsigned short int icmd = cmd[1] & 0x00ff;  //0-127 read, >=128 write
	unsigned short int idev = cmd[0] & 0x000f;  //0-7 CMD ignored, >=8 CMD req'd.
	unsigned long int add_vmepart = (vmeadd_ | 0x00030000) + (idev << 12) + (icmd << 2);
	unsigned short int *ptr = (unsigned short int *) add_vmepart;
	
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
		short unsigned int *data = (short unsigned int *) snd;
		vme_controller(3,ptr,data,rcv);
		//JRG, end Write case ^^^^
	} else { //Read
		unsigned short int tmp[1] = {0x0000};
		vme_controller(2,ptr,tmp,rcv);
		// printf(" scan vmepar: return from read, nrcv=%d,",nrcv);
		// for(i=0;i<nrcv;i++)printf(" %02x",rcv[i]&0xff);
		// printf("\n");
	}
}


/*
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
*/

void emu::fed::VMEModule::dcc(const char *cmd,char *rcv)
{

	unsigned long int add = vmeadd_ + (cmd[1] << 2);
	unsigned short int *ptr = (unsigned short int *) add;
	
	unsigned short int *data;
	
	if (cmd[0] == 0x00) { // write
		
		char c[2];
		c[0] = cmd[3];
		c[1] = cmd[2];
		
		data = (unsigned short int *) c;
		vme_controller(3,ptr,data,rcv);
	} else if (cmd[0] == 0x01) { // read
		vme_controller(2,ptr,data,rcv);
	}
}


/*
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
*/


void emu::fed::VMEModule::vme_adc(int ichp,int ichn,char *rcv)
{
	
	unsigned short int tmp[2] = {0x0000,0x0000};
	char adcreg[5] = {0x00,0x0e,0x0d,0x0b,0x07};
	char adcbyt[3][8] = {
		{0x89,0x99,0xa9,0xb9,0xc9,0xd9,0xe9,0xf9},
		{0x89,0x99,0xa9,0xb9,0xc9,0xd9,0xe9,0xf9},
		{0x85,0x95,0xa5,0xb5,0xc5,0xd5,0xe5,0xf5}
	};

	unsigned short int val[2] = {adcreg[ichp], adcbyt[ichp-1][ichn]};
	
	if (val[0] == 0) {
		unsigned long int add_adcrs = vmeadd_ | 0xd000 | 0x24;
		unsigned short int *ptr = (unsigned short int *) add_adcrs;
		vme_controller(2,ptr,tmp,rcv);
		return;
	}

	unsigned long int add_adcw = vmeadd_ | 0xd000;
	unsigned short int *ptr = (unsigned short int *) add_adcw;
	vme_controller(3,ptr,&val[1],rcv);
	
	if (val[0] != 0x07) {
		unsigned long int add_adcr = vmeadd_ | 0xd000 | 0x04;
		ptr = (unsigned short int *) add_adcr;
	} else {
		unsigned long int add_adcrbb = vmeadd_ | 0xd000 | 0x0c;
		ptr = (unsigned short int *) add_adcrbb;
	}
	vme_controller(2,ptr,tmp,rcv);

}


/*
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
*/


void emu::fed::VMEModule::RestoreIdle(enum DEVTYPE dev)
{
	int idev = geo[dev].jchan;
	char tmp[2] = {0x00,0x00};
	
	if (idev == 12) { // RESET!
		unsigned short int one[1] = {0x01};
		unsigned long int add_reset = vmeadd_ | 0xfffe;
		unsigned short int *ptr = (unsigned short int *) add_reset;
		for(int i = 0; i < 5; i++) {
			vme_controller(1,ptr,one,tmp);
		}
	} else { // Not RESET!
		unsigned short int tmp2[1] = {0x0000};
		unsigned long int add_rst = vmeadd_ | (idev << 12) | 0x18;
		unsigned short int *ptr_rst = (unsigned short *) add_rst;
		vme_controller(3,ptr_rst,tmp2,tmp);
	}

}
