#include "DDUDebugger.h"

using namespace std;


DDUDebugger::DDUDebugger()
{
}



DDUDebugger::~DDUDebugger()
{
}



std::map<string, string> DDUDebugger::DDUFPGAStat(unsigned long int stat)
{
	std::map<string, string> returnValues;

	if (stat&0x0000F000) {
		if (stat&0xF0000000) {
			if (0x80000000&stat) returnValues["DMB LCT/DAV/MOVLP Mismatch"] = "red";
			if (0x40000000&stat) returnValues["CFEB L1A Mismatch"] = "red";
			if (0x20000000&stat) returnValues["DDUsawNoGoodDMB-CRCs"] = "blue";
			if (0x10000000&stat) returnValues["CFEB Count Mismatch"] = "red";
		}
		if (stat&0x0F000000) {
			if (0x08000000&stat) returnValues["FirstDat Error"] = "red";
			if (0x04000000&stat) returnValues["L1A-FIFO Full occurred"] = "red";
			if (0x02000000&stat) returnValues["Data Stuck in FIFO occurred"] = "red";
			if (0x01000000&stat) returnValues["NoLiveFiber warning"] = "blue";
		}
		if (stat&0x00F00000) {
			if (0x00800000&stat) returnValues["Special-word voted-bit warning"] = "orange";
			if (0x00400000&stat) returnValues["InRDctrl Error"] = "red";
			if (0x00200000&stat) returnValues["DAQ Stop bit set"] = "blue";
			if (0x00100000&stat) returnValues["DAQ says Not Ready"] = "blue";
			if (0x00300000&stat==0x00200000) returnValues["DAQ Applied Backpressure"] = "blue";
		}
		if (stat&0x000F0000) {
			if (0x00080000&stat) returnValues["TMB Error"] = "orange";
			if (0x00040000&stat) returnValues["ALCT Error"] = "orange";
			if (0x00020000&stat) returnValues["Trigger Wordcount Error"] = "orange";
			if (0x00010000&stat) returnValues["Trigger L1A Match Error"] = "orange";
		}
		// JRG, low-order 16-bit status (most serious errors):
		if (stat&0x0000F000) {
			if (0x00008000&stat) returnValues["Critical Error ** needs reset **"] = "error";
			if (0x00004000&stat) returnValues["Single Error, bad event"] = "orange";
			if (0x00002000&stat) returnValues["Single warning, possible data problem"] = "blue";
			if (0x00001000&stat) returnValues["Near Full Warning"] = "blue";
		}
		if (stat&0x00000F00) {
			if (0x00000800&stat) returnValues["64-bit Alignment Error"] = "blue";
			if (0x00000400&stat) returnValues["DDU Control DLL Error (recent)"] = "blue";
			if (0x00000200&stat) returnValues["DMB Error in event"] = "orange";
			if (0x00000100&stat) returnValues["Lost In Event Error"] = "orange";
		}
		if (stat&0x000000F0) {
			if (0x00000080&stat) returnValues["Lost In Data Error occurred"] = "red";
			if (0x00000040&stat) returnValues["Timeout Error occurred"] = "red";
			if (0x00000020&stat) returnValues["Trigger CRC Error"] = "orange";
			if (0x00000010&stat) returnValues["Multiple Transmit Errors occurred"] = "red";
		}
		if (stat&0x0000000F) {
			if (0x00000008&stat) returnValues["Lost Sync occurred (FIFO Full or L1A Error)"] = "red";
			if (0x00000004&stat) returnValues["Fiber/FIFO Connection Error occurred"] = "red";
			if (0x00000002&stat) returnValues["Single L1A Mismatch"] = "orange";
			if (0x00000001&stat) returnValues["DMB or CFEB CRC Error"] = "orange";
		}
	} else {
  //returnValues["OK"] = "none";
	}

	return returnValues;
}



std::map<string, string> DDUDebugger::OutputStat(int stat)
{
	std::map<string, string> returnValues;

	if (stat&0x0000D981) {
		if (0x00001000&stat) returnValues["DDU S-Link Not Present"] = "green";
		if (0x00000100&stat) returnValues["SPY/GbE Fiber Disconnected"] = "blue";
		else if (0x00000200&stat) returnValues["SPY/GbE FIFO Always Empty"] = "none";
		if (stat&0x0000ECEF) {
			if (stat&0x0000E000) {
				if (0x00008000&stat) returnValues["DDU Buffer Overflow occurred"] = "none";
				if (0x00004000&stat) returnValues["DAQ (DCC/S-Link) Wait occurred"] = "blue";
				if (0x00002000&stat) returnValues["DDU S-Link Full occurred"] = "none";
				//if (0x00001000&stat) returnValues["DDU S-Link Never Ready"] = "none";
				//if (0x000000cef&stat&&(0x000000cef&stat)<=0x00ff) *out << br();
			}
			if (stat&0x00000E00) {
				//if (stat&0x00000900)==0x0800||(stat&0x00000500)==0x0400) *out << br();
				if (0x00000800&stat && (0x00000100&stat)==0) returnValues["DDU GbE Overflow occurred"] = "blue";
				if (0x00000400&stat && (0x00000100&stat)==0) returnValues["GbE Transmit Limit occurred"] = "none";
				//if (0x00000200&stat&&(0x00000100&stat)==0) returnValues["GbE FIFO Always Empty &nbsp ";
				//if (0x00000100&stat) returnValues["<font color=blue>SPY/GbE Fiber Disconnect occurred</font>";
				//if (0x000000ef&stat) *out << br();
			}
			if (stat&0x000000F0) {
				if (0x00000080&stat) returnValues["DDU DAQ-Limited Overflow occurred (DCC/S-Link Wait)"] = "red";
				if (0x00000040&stat) returnValues["DAQ (DCC/S-Link) Wait"] = "blue";
				if (0x00000020&stat) returnValues["DDU S-Link Full/Stop"] = "none";
				if (0x00000010&stat&&(0x00001000&stat)==0) returnValues["DDU S-Link Not Ready"] = "red";
			}
			if (stat&0x0000000F) {
				//if (0x0000000e&stat&&(0x00000001&stat)==0) *out << br();
				if (0x00000008&stat&&(0x00000100&stat)==0) returnValues["GbE FIFO Full"] = "none";
				if (0x00000004&stat&&(0x00000100&stat)==0) returnValues["DDU Skipped SPY Event (GbE data not sent)"] = "none";
				if (0x00000002&stat&&(0x00000100&stat)==0) returnValues["GbE FIFO Not Empty"] = "none";
				if (0x00000001&stat) returnValues["DCC Link Not Ready"] = "blue";
			}
			//*out << "</font></blockquote>";
		}
	} else {
  //returnValues["OK"] = "none";
	}

	return returnValues;
}



std::map<string, string> DDUDebugger::EBReg1(int stat)
{
	std::map<string, string> returnValues;

	if (stat&0x0000ffff) {
		//*out << "<blockquote><font size=-1 color=orange face=arial>";
		if (stat&0x0000F000) {
			if (0x00008000&stat) returnValues["DMB Timeout signal, ** needs reset **"] = "error";
			if (0x00004000&stat) returnValues["Mult L1A Error occurred"] = "red";
			if (0x00002000&stat) returnValues["L1A-FIFO Near Full Warning"] = "blue";
			if (0x00001000&stat) returnValues["GbE FIFO Almost-Full"] = "none";
			//if (0x0fff&stat) *out << br();
		}
		if (stat&0x00000F00) {
			if (0x00000800&stat) returnValues["Ext.FIFO Near Full Warning"] = "blue";
			//if (0x00000400&stat) returnValues["Near Full Warning"] = "blue";
			if (0x00000400&stat) returnValues["InSingle Warning"] = "blue";
			if (0x00000200&stat) returnValues["CFEB-CRC not OK"] = "none";
			if (0x00000100&stat) returnValues["CFEB-CRC End Error"] = "orange";
			//if (0x00ff&stat) *out << br();
		}
		if (stat&0x000000F0) {
			if (0x00000080&stat) returnValues["CFEB-CRC Count Error"] = "orange";
			if (0x00000040&stat) returnValues["DMB or CFEB CRC Error"] = "orange";
			//if (0x00000020&stat) returnValues["Latched Trigger Trail"] = "none";
			if (0x00000020&stat) returnValues["Trigger Readout Error"] = "orange";
			if (0x00000010&stat) returnValues["Trigger Trail Done"] = "none";
			//if (0x000f&stat) *out << br();
		}
		if (stat&0x0000000F) {
			if (0x00000008&stat) returnValues["Start Timeout"] = "red";
			if (0x00000004&stat) returnValues["End Timeout"] = "red";
			if (0x00000002&stat) returnValues["SP/TF Error in last event"] = "orange";
			if (0x00000001&stat) returnValues["SP/TF data detected in last event"] = "orange";
		}
	} else {
  //returnValues["OK"] = "none";
	}

	return returnValues;
}



std::map<string, string> DDUDebugger::EBReg2(int stat)
{
	std::map<string, string> returnValues;

	if(stat&0x0000ffff) {
		if (0x00000020&stat) returnValues["Empty CSC in Event flag"] = "green";
		//if (0x0000FFDF&stat) *out << "<blockquote><font size=-1 color=orange face=arial>";
		if (stat&0x0000F000) {
			if (0x00008000&stat) returnValues["Lost In Event Error"] = "orange";
			if (0x00004000&stat) returnValues["DMB Error in Event"] = "orange";
			if (0x00002000&stat) returnValues["Control DLL Error occured"] = "blue";
			if (0x00001000&stat) returnValues["2nd Header First flag"] = "orange";
			//if(0x0fdf&stat) *out << br();
		}
		if (stat&0x00000F00) {
			if (0x00000800&stat) returnValues["Early 2nd Trailer flag"] = "none";
			if (0x00000400&stat) returnValues["Extra 1st Trailer flag"] = "none";
			if (0x00000200&stat) returnValues["Extra 1st Header flag"] = "none";
			if (0x00000100&stat) returnValues["Extra 2nd Header flag"] = "none";
			//if(0x00df&stat) *out << br();
		}
		if (stat&0x000000D0) {
			if (0x00000080&stat) returnValues["SCA Full detected this Event"] = "orange";
			if (0x00000040&stat) returnValues["Probable DMB Full occurred"] = "blue";
			//if (0x00000020&stat) returnValues["Empty Event flag"] = "green";
			if (0x00000010&stat) returnValues["Bad Control Word Error occurred"] = "red";
			//if(0x000f&stat) *out << br();
		}
		if (stat&0x0000000F) {
			if (0x00000008&stat) returnValues["Missed Trigger Trailer Error"] = "orange";
			if (0x00000004&stat) returnValues["First Dat Error"] = "orange";
			if (0x00000002&stat) returnValues["Bad First Word"] = "orange";
			if (0x00000001&stat) returnValues["Confirmed DMB Full occured"] = "red";
			// if (0x00000001&stat) returnValues["Lost In Data occured"] = "red";
		}
	} else {
  //returnValues["OK"] = "none";
	}

	return returnValues;
}



std::map<string, string> DDUDebugger::EBReg3(int stat)
{
	std::map<string, string> returnValues;

	if(stat&0x0000ffff) {
		//*out << "<blockquote><font size=-1 color=black face=arial>";
		if (stat&0x0000F000) {
			if (0x00008000&stat) returnValues["Trigger Readout Error"] = "orange";
			if (0x00004000&stat) returnValues["ALCT Trailer Done"] = "none";
			if (0x00002000&stat) returnValues["2nd ALCT Trailer detected"] = "red";
			//if (0x00002000&stat) returnValues["ALCT DAV Vote True occurred"] = "none";
			if (0x00001000&stat) returnValues["ALCT L1A mismatch error occurred"] = "none";
			//if(0x0fff&stat) *out << br();
		}
		if (stat&0x00000F00) {
			if (0x00000800&stat) returnValues["ALCT CRC Error occurred"] = "none";
			if (0x00000400&stat) returnValues["ALCT Wordcount Error occurred"] = "none";
			if (0x00000200&stat) returnValues["Missing ALCT Trailer occurred"] = "none";
			if (0x00000100&stat) returnValues["ALCT Error occurred"] = "none";
			//if(0x00ff&stat) *out << br();
		}
		if (stat&0x000000F0) {
			if (0x00000080&stat) returnValues["DMB Critical Error occurred"] = "none";
			//if (0x00000080&stat) returnValues["Compare Trigger CRC flag"] = "none";
			if (0x00000040&stat) returnValues["TMB Trailer Done"] = "none";
			if (0x00000020&stat) returnValues["2nd TMB Trailer detected"] = "red";
			//if (0x00000020&stat) returnValues["TMB DAV Vote True occurred"] = "none";
			if (0x00000010&stat) returnValues["TMB L1A mismatch error occurred"] = "none";
			//if(0x000f&stat) *out << br();
		}
		if (stat&0x0000000F) {
			if (0x00000008&stat) returnValues["TMB CRC Error occurred"] = "none";
			if (0x00000004&stat) returnValues["TMB Word Count Error occurred"] = "none";
			if (0x00000002&stat) returnValues["Missing TMB Trailer occurred"] = "none";
			if (0x00000001&stat) returnValues["TMB Error occurred"] = "none";
		}

	} else {
  //returnValues["OK"] = "none";
	}


	return returnValues;
}



std::map<string, string> DDUDebugger::FIFO2(int stat)
{
	std::map<string, string> returnValues;

	if (stat&0x000000ff) {

		if (0x00000040&stat) returnValues["L1A FIFO Empty"] = "none";
		if (0x00000040&stat == 0) returnValues["L1A FIFO Not Empty"] = "none";
		if (0x00000080&stat) returnValues["DDU C-code L1A error"] = "blue";
		if (0x00000002&stat) returnValues["GbE FIFO Almost-Full occurred"] = "none";
		if (0x00000001&stat) returnValues["L1A FIFO Almost-Full occurred"] = "blue";

	} else {
  //returnValues["OK"] = "none";
	}


	return returnValues;
}



std::map<string, string> DDUDebugger::FFError(int stat)
{
	std::map<string, string> returnValues;

	if(stat&0x000000ff) {

		if (0x00000040&stat) returnValues["L1A FIFO Empty"] = "none";
		if (0x00000040&stat == 0) returnValues["L1A FIFO Not Empty"] = "none";
		if (0x00000002&stat) returnValues["GbE FIFO Full occurred"] = "none";
		if (0x00000001&stat) returnValues["L1A FIFO Full occurred"] = "red";

	} else {
  //returnValues["OK"] = "none";
	}


	return returnValues;
}



std::map<string, string> DDUDebugger::InCHistory(int stat)
{
	std::map<string, string> returnValues;

	if(stat&0x00000fff) {
		//*out << "<blockquote><font size=-1 color=red face=arial>";
		if (stat&0x00000F00) {
			if (0x00000800&stat) returnValues["InRD End C-Code Error occurred"] = "red";
			if (0x00000400&stat) returnValues["InRD Begin C-Code Error occurred"] =  "red";
			if (0x00000200&stat) returnValues["InRD Multiple L1A Mismatches occurred"] = "red";
			else if (0x00000100&stat) returnValues["InRD Single L1A Mismatch occurred"] = "blue";
		}
		if (stat&0x000000F0) {
			if (0x00000080&stat) returnValues["InRD Hard Error occurred"] = "red";
			if (0x00000040&stat) returnValues["InRD Sync Error occurred"] = "red";
			if (0x00000020&stat) returnValues["InRD Single Error occurred"] = "blue";
			if (0x00000010&stat) returnValues["InRD Mem/FIFO Error occurred"] = "red";
		}
		if (stat&0x0000000F) {
			if (0x00000008&stat) returnValues["InRD Fiber Connection Error occurred"] = "red";
			if (0x00000004&stat) returnValues["InRD Multiple Transmit Errors occurred"] = "red";
			if (0x00000002&stat) returnValues["InRD Stuck Data Error occurred"] = "red";
			if (0x00000001&stat) returnValues["InRD Timeout Error occurred"] = "red";
		}
	} else {
  //returnValues["OK"] = "none";
	}


	return returnValues;
}



std::map<string, string> DDUDebugger::WarnMon(int stat)
{
	std::map<string, string> returnValues;

	if (stat&0x000000ff) {
		if (0x01&stat) returnValues["InRD0 set FMM warning"] = "none";
		if (0x02&stat) returnValues["InRD1 set FMM warning"] = "none";
		if (0x04&stat) returnValues["InRD2 set FMM warning"] = "none";
		if (0x08&stat) returnValues["InRD3 set FMM warning"] = "none";
		if (0x10&stat) returnValues["L1A FIFO set FMM warning"] = "none";
		if (0x20&stat) returnValues["Ext. FIFO set FMM warning"] = "none";
		if (0x40&stat) returnValues["DMB set FMM warning"] = "none";
		if (0x80&stat) returnValues["DDU set FMM warning"] = "none";

	} else {
		//returnValues["OK"] = "none";
	}


	return returnValues;
}



string DDUDebugger::ddu_fpgatrap(DDU *thisDDU)
{
	ostringstream *out = new ostringstream();

	thisDDU->ddu_fpgatrap();

	//printf(" enter DDUtrapDecode \n");
	unsigned long int lcode[10];
	int i;
	char buf[100],buf1[100],buf2[100],buf3[100],buf4[100];
	char cbuf1[20],cbuf2[20],cbuf3[20],cbuf4[20];
	char sred[20]="<font color=red>";
	char syel[20]="<font color=orange>";
	char sblu[20]="<font color=blue>";
	char sgrn[20]="<font color=green>";
	char snul[20]="</font>";
	sprintf(buf1," ");
	sprintf(buf2," ");
	sprintf(buf3," ");
	sprintf(buf4," ");
	sprintf(cbuf1," ");
	sprintf(cbuf2," ");
	sprintf(cbuf3," ");
	sprintf(cbuf4," ");
	lcode[0]=thisDDU->fpga_lcode[0];
	lcode[1]=thisDDU->fpga_lcode[1];
	lcode[2]=thisDDU->fpga_lcode[2];
	lcode[3]=thisDDU->fpga_lcode[3];
	lcode[4]=thisDDU->fpga_lcode[4];
	lcode[5]=thisDDU->fpga_lcode[5];
	sprintf(buf,"  192-bit DDU Control Diagnostic Trap (24 bytes)");
	*out << buf << endl;
	i=23;
	sprintf(buf,"                        o-stat  fful  fifo-c fifo-b");
	*out << buf << endl;
	sprintf(buf,"      rcv bytes %2d-%2d:",i,i-7);
	sprintf(cbuf1,"%s",sgrn);
	if(0x09010000&lcode[5])sprintf(cbuf1,"%s",sblu);
	if(0x40000000&lcode[5])sprintf(cbuf1,"%s",syel);
	if(0x80800000&lcode[5])sprintf(cbuf1,"%s",sred);
	sprintf(buf1,"%s   %04lx%s",cbuf1,(0xffff0000&lcode[5])>>16,snul);
	sprintf(cbuf2,"%s",sgrn);
	if(0x01ff&lcode[5])sprintf(cbuf2,"%s",sred);
	sprintf(buf2,"%s   %04lx%s",cbuf2,0xffff&lcode[5],snul);
	sprintf(cbuf3,"%s",sgrn);
	if(0xffff0000&lcode[4])sprintf(cbuf3,"%s",sred);
	sprintf(buf3,"%s   %04lx%s",cbuf3,(0xffff0000&lcode[4])>>16,snul);
	sprintf(cbuf4,"%s",sgrn);
	if(0x01ff&lcode[4])sprintf(cbuf4,"%s",sblu);
	sprintf(buf4,"%s   %04lx%s",cbuf4,0xffff&lcode[4],snul);
	*out << buf << buf1 << buf2 << buf3 << buf4 << endl;

	i=15;
	sprintf(buf,"                        fifo-a instat c-code  erc");
	*out << buf << endl;
	sprintf(buf,"      rcv bytes %2d-%2d:",i,i-7);
	sprintf(cbuf1,"%s",sgrn);
	if(0xfff00000&lcode[3])sprintf(cbuf1,"%s",sred);
	sprintf(buf1,"%s   %04lx%s",cbuf1,(0xffff0000&lcode[3])>>16,snul);
	sprintf(cbuf2,"%s",sgrn);
	if(0x00f0&lcode[3])sprintf(cbuf2,"%s",syel);
	if(0xff0f&lcode[3])sprintf(cbuf2,"%s",sred);
	sprintf(buf2,"%s   %04lx%s",cbuf2,0xffff&lcode[3],snul);
	sprintf(cbuf3,"%s",sgrn);
	if(0x00200000&lcode[2])sprintf(cbuf3,"%s",syel);
	if(0xffdf0000&lcode[2])sprintf(cbuf3,"%s",sred);
	sprintf(buf3,"%s   %04lx%s",cbuf3,(0xffff0000&lcode[2])>>16,snul);
	sprintf(cbuf4,"%s",sgrn);
	if(0x9f1f&lcode[2])sprintf(cbuf4,"%s",syel);
	sprintf(buf4,"%s   %04lx%s",cbuf4,0xffff&lcode[2],snul);
	*out << buf << buf1 << buf2 << buf3 << buf4 << endl;

	i=7;
	sprintf(buf,"                         erb    era   32-bit status");
	*out << buf << endl;
	sprintf(buf,"      rcv bytes %2d-%2d:",i,i-7);
	sprintf(cbuf1,"%s",sgrn);
	if(0x00110000&lcode[1])sprintf(cbuf1,"%s",syel);
	if(0xd08e0000&lcode[1])sprintf(cbuf1,"%s",sred);
	sprintf(buf1,"%s   %04lx%s",cbuf1,(0xffff0000&lcode[1])>>16,snul);
	sprintf(cbuf2,"%s",sgrn);
	if(0x2c00&lcode[1])sprintf(cbuf2,"%s",sblu);
	if(0x01e0&lcode[1])sprintf(cbuf2,"%s",syel);
	if(0xc00c&lcode[1])sprintf(cbuf2,"%s",sred);
	sprintf(buf2,"%s   %04lx%s",cbuf2,0xffff&lcode[1],snul);
	sprintf(cbuf3,"%s",sgrn);
	if(0x21800000&lcode[0])sprintf(cbuf3,"%s",sblu);
	if(0xd00f0000&lcode[0])sprintf(cbuf3,"%s",syel);
	if(0x0e400000&lcode[0])sprintf(cbuf3,"%s",sred);
	sprintf(buf3,"%s   %04lx%s",cbuf3,(0xffff0000&lcode[0])>>16,snul);
	sprintf(cbuf4,"%s",sgrn);
	if(0x3400&lcode[0])sprintf(cbuf4,"%s",sblu);
	if(0x4b23&lcode[0])sprintf(cbuf4,"%s",syel);
	if(0x80dc&lcode[0])sprintf(cbuf4,"%s",sred);
	sprintf(buf4,"%s   %04lx%s",cbuf4,0xffff&lcode[0],snul);
	*out << buf << buf1 << buf2 << buf3 << buf4 << endl;



	unsigned int CSCstat=0;
	unsigned long int i0trap[8];
	unsigned long int i1trap[8];
	unsigned long int i0stat,i1stat,ddustat,erastat;
	short int igot_i0,igot_i1,solved,iFill,iTimeout;
	igot_i0=0;  igot_i1=0;  solved=0;  iFill=0;  iTimeout=0;
	*out << "DDU Diagnosis results:" << endl;
	CSCstat=thisDDU->vmepara_CSCstat();  // which inputs report a problem
/*
  // Begin Global DDUstatus check: already have this from the Diagnostic trap!
	thisDDU->ddu_fpgastat();
	ddustat=((0xffff&thisDDU->ddu_code1)<<16)|(0xffff&thisDDU->ddu_code0);
*/
	ddustat=(0xffffffff&lcode[0]);

/*
 // already have this from the Diagnostic trap!
	thisDDU->ddu_rderareg();
	erastat=0x0000ffff&thisDDU->ddu_code0;
*/
	erastat=(0x0000ffff&lcode[1]);

	thisDDU->infpgastat(INFPGA0);  // Begin Global InFPGA0 check
	i0stat=((0xffff&thisDDU->infpga_code1)<<16)|(0xffff&thisDDU->infpga_code0);
	//  sprintf(buf,"infpga0 32-bit Status: %08lXh ",i0stat);
	//  *out << buf << endl;
	if(i0stat&0x04000000){            // DLL Error
		*out << "**DLLerror detected on InFPGA0** " << endl;
		if((i0stat&0x00000800)>0)*out << "  ^^^probable cause of Gt-Rx errors " << endl;
		solved=1;
	} else if(i0stat&0x00000004){       // Fiber Change
		thisDDU->infpga_CheckFiber(INFPGA0);
		sprintf(buf,"**Fiber Connection error detected for DDUinput[7:0]=0x%02x** ",(thisDDU->infpga_code0&0xff00)>>8);
		*out << buf << endl;
		if((i0stat&0x00000800)>0)*out << "  ^^^probable cause of Gt-Rx errors " << endl;
		else if((i0stat&0x00000130)>0)*out << "  ^^^probable cause of SpecialWord/Xmit errors " << endl;
		solved=1;
	} else if((i0stat&0x00000800)>0){   // GT-Rx Error
		thisDDU->infpga_RxErr(INFPGA0);
		sprintf(buf," *GT-Rx Error for DDUinput[7:0]=0x%02x* ",(thisDDU->infpga_code0&0xff00)>>8);
		*out << buf << endl;
		if((i0stat&0x00000130)>0)*out << "  ^^^probable cause of SpecialWord/Xmit errors " << endl;
		solved=1;
	} else if((lcode[1]&0x00010000)>0&&(i0stat&0x00880000)>0){  // DMB-Full
		thisDDU->infpga_DMBwarn(INFPGA0);
		sprintf(buf," *confirmed DMB Full for DDUinput[7:0]=0x%02x* ",(thisDDU->infpga_code0&0xff00)>>8);
		*out << buf << endl;
		if((lcode[0]&0x00040000)>0&&(lcode[2]&0x00001c00)==0)*out << " ^^^DMB Full FIFO for ALCT " << endl;
		else if((lcode[0]&0x00080000)>0&&(lcode[2]&0x0000001c)==0)*out << " ^^^DMB Full FIFO for TMB " << endl;
		else *out << " ^^^DMB Full FIFO, probably for CFEB " << endl;
		if((i0stat&0x00000130)>0)*out << "       --probable cause of SpecialWord/Xmit errors  " << endl;
		solved=1;
	}


	thisDDU->infpgastat(INFPGA1);  // Begin InFPGA1 check, repeat from InFPGA0^^
	i1stat=((0xffff&thisDDU->infpga_code1)<<16)|(0xffff&thisDDU->infpga_code0);
	//  sprintf(buf2,"infpga1 32-bit Status: %08lXh ",i1stat);
	//  *out << buf << endl;
	if(i1stat&0x04000000){
		*out << "**DLLerror detected on InFPGA1** " << endl;
		if((i1stat&0x00000800)>0)*out << "  ^^^probable cause of Gt-Rx errors " << endl;
		solved=1;
	} else if(i1stat&0x00000004){
		thisDDU->infpga_CheckFiber(INFPGA1);
		sprintf(buf,"**Fiber Connection error detected for DDUinput[14:8]=0x%02x** ",(thisDDU->infpga_code0&0x7f00)>>8);
		*out << buf << endl;
		if((i1stat&0x00000800)>0)*out << "  ^^^probable cause of Gt-Rx errors " << endl;
		else if((i1stat&0x00000130)>0)*out << "  ^^^probable cause of SpecialWord/Xmit errors " << endl;
		solved=1;
	} else if((i1stat&0x00000800)>0){
		thisDDU->infpga_RxErr(INFPGA1);
		sprintf(buf," *GT-Rx Error for DDUinput[14:8]=0x%02x* ",(thisDDU->infpga_code0&0x7f00)>>8);
		*out << buf << endl;
		if((i1stat&0x00000130)>0)*out << "  ^^^probable cause of SpecialWord/Xmit errors " << endl;
		solved=1;
	} else if((lcode[1]&0x00010000)>0&&(i1stat&0x00880000)>0){
		thisDDU->infpga_DMBwarn(INFPGA1);
		sprintf(buf," *confirmed DMB Full for DDUinput[14:8]=0x%02x* ",(thisDDU->infpga_code0&0x7f00)>>8);
		*out << buf << endl;
		if((lcode[0]&0x00040000)>0&&(lcode[2]&0x00001c00)==0)*out << " ^^^DMB Full FIFO for ALCT " << endl;
		else if((lcode[0]&0x00080000)>0&&(lcode[2]&0x0000001c)==0)*out << " ^^^DMB Full FIFO for TMB " << endl;
		else *out << " ^^^DMB Full FIFO, probably for CFEB " << endl;
		if((i1stat&0x00000130)>0)*out << "       --probable cause of SpecialWord/Xmit errors " << endl;
		solved=1;
	}
	//  ^^^^ InFPGA big-problem analysis solved it? ^^^^

	if(lcode[0]&0x00000400){
		*out << "**DLLerror detected on DDUctrl FPGA** " << endl;
		solved=1;
	}
	if((lcode[5]&0x00800000)>0&&(lcode[2]&0x0c000000)==0){  // DAQ-induced Buffer overflow, FullFIFO
		*out << "**DAQ-induced Buffer Overflow** " << endl;
		solved=1;
	}
	if((lcode[0]&0x4000000A)==8&&(lcode[1]&0x00004000)==0&&(lcode[2]&0x0c000000)==0){ // DDU Buff ovfl
		*out << "**DDU FIFO Full** " << endl;
		if(lcode[5]&0x0000000f){	// Ext.FIFO
			sprintf(buf," ^^^Memory error for DDU Ext.FIFO[3:0]=0x%01lx ",lcode[5]&0x0000000f);
			*out << buf;
		}
		if(lcode[5]&0x00000100)*out << "  ^^^L1A FIFO Full ";
		*out << " " << endl;
		solved=1;
	}

	if(solved<1&&(((erastat|lcode[1])&0x00008000)>0)){  // DMB-Timeout?  check at and after Critical error point, get DMB Error Reg
		*out << "**DMB Timeout signal detected** " << endl;
		thisDDU->ddu_rddmberr();
		sprintf(buf,"    ^^^Error on DDUinput[14:0]=0x%04x ",(thisDDU->ddu_code0)&0x7fff);
		*out << buf << endl;

	// Note: if ALCT Timeout on DMB then LIE, CRC & WC errors are likely from
	//       overrun to TMB trail; look for lcode-LIE caused by bad/missTrigTrail.
	//    If DMB end-timeout causes 64-bit misalignment then maybe check that too.
		solved=1;
		iTimeout=1;
		if((lcode[0]&0x00040000)>0&&(lcode[1]&0x00080000)>0)*out << " ^^^DMB Timeout for ALCT " << endl;
		else if((lcode[0]&0x00080000)>0&&(lcode[1]&0x00080000)>0)*out << " ^^^DMB Timeout for TMB " << endl;
		else if((lcode[0]&0x00000800)==0)*out << " ^^^DMB Timeout, probably for CFEB " << endl;
		else{
			*out << " ^^^DMB Timeout w/64-bit misalignment, possibly from CFEB " << endl;
			solved=0;
		}
		if((i0stat&0x0000e000)>0)*out << "       --probable cause of problems on InFPGA0 " << endl;
		if((i1stat&0x0000e000)>0)*out << "       --probable cause of problems on InFPGA1 " << endl;
	} else if((lcode[2]&0x0C000000)>0||(lcode[1]&0x00020000)>0){  // DDU FIFO Transfer/C-code Error
		*out << "**DDU FIFO Transfer error detected** " << endl;
		if(lcode[2]&0x04000000)*out << "  ^^^Begin C-code error " << endl;
		if(lcode[2]&0x08000000)*out << "  ^^^End C-code error " << endl;
		sprintf(buf,"  ^^^probably occurred on Ext.FIFO[3:0]=0x%01lx  (from InMxmit Reg) ",(lcode[2]>>28)&0x0000000f);
		*out << buf << endl;
		solved=1;
	}

	// InCtrlErr at CritErr point:
	if(solved<1&&((lcode[0]&0x00400000)>0)){ // InCtrlErr & NotDDUfullFIFO
		//    *out << "-debug> inside 3>" << endl;
		if((i0stat|i1stat)&0x40000000){    // Filler=64bit-misalign
			if((i0stat&0x40000000)>0){       //   for InFPGA0
				iFill=1;
				thisDDU->infpga_XmitErr(INFPGA0);
				sprintf(buf," *64-bit Align Error for DDUinput[7:0]=0x%02x* ",(thisDDU->infpga_code0&0x00ff));
				*out << buf << endl;
				if((i0stat&0x00000130)>0)*out << "  ^^^also associated with SpecialWord errors  " << endl;
			}
			if((i1stat&0x40000000)>0){       //   for InFPGA1
				iFill+=2;
				thisDDU->infpga_XmitErr(INFPGA1);
				sprintf(buf," *64-bit Align Error for DDUinput[14:8]=0x%02x* ",(thisDDU->infpga_code0&0x007f));
				*out << buf << endl;
				if((i1stat&0x00000130)>0)*out << "  ^^^also associated with SpecialWord errors  " << endl;
			}
		//      solved=1;
		}
	// If InCtrlErr and not solved, get InTrap registers (each 32 bytes)
		if(i0stat&0x00008000){
			//      *out << "-debug> inside 4>" << endl;
			thisDDU->infpga_trap(INFPGA0);
			i0trap[5]=thisDDU->fpga_lcode[5];
			i0trap[4]=thisDDU->fpga_lcode[4];
			i0trap[3]=thisDDU->fpga_lcode[3];
			i0trap[2]=thisDDU->fpga_lcode[2];
			i0trap[1]=thisDDU->fpga_lcode[1];
			i0trap[0]=thisDDU->fpga_lcode[0];
			igot_i0=1;
		}
		if(i1stat&0x00008000){
			thisDDU->infpga_trap(INFPGA1);
			i1trap[5]=thisDDU->fpga_lcode[5];
			i1trap[4]=thisDDU->fpga_lcode[4];
			i1trap[3]=thisDDU->fpga_lcode[3];
			i1trap[2]=thisDDU->fpga_lcode[2];
			i1trap[1]=thisDDU->fpga_lcode[1];
			i1trap[0]=thisDDU->fpga_lcode[0];
			igot_i1=1;
		}
	}


	if(solved<1&&iFill>0){  //  check for cause of misalignment early
		if(lcode[0]&0x80000002)*out << "  ^^^possible that DMB may have caused 64-bit Align Error" << endl;  // LCT/DAV(lcode[0]31), DMBL1A(lcode[0]1)
		else if((lcode[2]&0x00000c00)>0||(lcode[0]&0x00040000)>0){  // ALCTerr
			thisDDU->ddu_rdalcterr();
			*out << "  ^^^possible that ALCT may have caused 64-bit Align Error" << endl;
			if((lcode[1]&0x00080000)>0&&(lcode[0]&0x00040000)>0)*out << "    ^^probable ALCT Trail word problem" << endl;
			else if(lcode[0]&0x00000020)*out << "    ^^probable ALCT CRC mismatch" << endl;
			else if(lcode[0]&0x00020000)*out << "    ^^probable ALCT Wordcount mismatch" << endl;
			if((thisDDU->ddu_code0)&0x7fff){
				sprintf(buf,"      ^^ALCT Errors from DDUinput[14:0]=0x%04x ",(thisDDU->ddu_code0)&0x7fff);
				*out << buf << endl;
			}
		} else if((lcode[2]&0x0000000c)>0||(lcode[0]&0x00080000)>0){  // TMBerr
			thisDDU->ddu_rdtmberr();
			*out << "  ^^^possible that TMB may have caused 64-bit Align Error" << endl;
			if((lcode[1]&0x00080000)>0&&(lcode[0]&0x00080000)>0)*out << "    ^^probable TMB Trail word problem" << endl;
			else if(lcode[0]&0x00000020)*out << "    ^^probable TMB CRC mismatch" << endl;
			else if(lcode[0]&0x00020000)*out << "    ^^probable TMB Wordcount mismatch" << endl;
			if((thisDDU->ddu_code0)&0x7fff){
				sprintf(buf,"      ^^TMB Errors from DDUinput[14:0]=0x%04x ",(thisDDU->ddu_code0)&0x7fff);
				*out << buf << endl;
			}
		} else if(lcode[1]&0x00080000) {  // TrgTrail error, maybe never get this one
			thisDDU->ddu_rdalcterr();
			*out << "  ^^^probable that ALCT or TMB caused 64-bit Align Error" << endl;
			*out << "    ^^Trigger Trail word problem" << endl;
			if((thisDDU->ddu_code0)&0x7fff){
				sprintf(buf,"      ^^ALCT Errors from DDUinput[14:0]=0x%04x ",(thisDDU->ddu_code0)&0x7fff);
				*out << buf << endl;
			}
			thisDDU->ddu_rdtmberr();
			if((thisDDU->ddu_code0)&0x7fff){
				sprintf(buf,"      ^^TMB Errors from DDUinput[14:0]=0x%04x ",(thisDDU->ddu_code0)&0x7fff);
				*out << buf << endl;
			}
		} else if(lcode[0]&0x50000001){  // CFEBerr
			*out << "  ^^^CFEB may have caused 64-bit Align Error" << endl;
			if(lcode[0]&0x10000000)*out << "    ^^CFEB count mismatch" << endl;
			if(lcode[0]&0x40000000)*out << "    ^^CFEB L1A mismatch" << endl;
			if(lcode[0]&0x00000001)*out << "    ^^CFEB or DMB CRC mismatch" << endl;
			if(lcode[1]&0x00000380){
				*out << "      ^confirmed CFEB CRC error" << endl;
				if(lcode[1]&0x00000080)*out << "        ^CFEB CRC-count error" << endl;
				if(lcode[1]&0x00000100)*out << "        ^CFEB CRC-end error" << endl;
				if(lcode[1]&0x00000200)*out << "        ^CFEB CRC mismatch" << endl;
			}
			else if(lcode[0]&0x00000201)*out << "      ^confirmed DMB CRC mismatch at least" << endl;
		}
		thisDDU->ddu_rddmberr();
		if((thisDDU->ddu_code0)&0x7fff){
			sprintf(buf,"      ^^CSC Errors from DDUinput[14:0]=0x%04x ",(thisDDU->ddu_code0)&0x7fff);
			*out << buf << endl;
		}
		if(iTimeout>0){
			if(lcode[1]&0x00008000)*out << "  ^^^Timeout at DMB caused 64-bit Align Error" << endl;
			solved=1;
		}
	}

	// If InCtrlErr, determine what happened at CritErr point:
	//        Timeout/StuckDat/MultXmit/MemErr/MultL1A
	if(solved<1&&((lcode[0]&0x00400000)>0)){
		//    *out << "-debug> inside 5>" << endl;
		if(igot_i0>0){  // got_i0trap;
			//      *out << "-debug> inside 6>" << endl;
			if((i0trap[0]&0x00000040)>0){
				if((i0trap[3]&0x00ff0000)>0)sprintf(buf," *Start Timeout for DDUinput[7:0] = 0x%02lx* ",((i0trap[3]>>16)&0x000000ff));
				else if((i0trap[4]&0x0000ffff)>0)sprintf(buf," *End Timeout for DDUinput[7:0] = 0x%02lx* ",((i0trap[4]>>8)|i0trap[4])&0x000000ff);
				*out << buf << endl;
				if(iFill==1||iFill==3)*out << "  ^^^may have caused 64-bit Align Error for InFPGA0" << endl;
				solved=1;
			} else if((i0trap[0]&0x00000080)>0){  // StuckData
				sprintf(buf," *StuckData error for DDUinput[7:0] = 0x%02lx* ",((i0trap[2]>>24)&0x000000ff));
				*out << buf << endl;
				if(iFill==1||iFill==3)*out << "  ^^^may have caused 64-bit Align Error for InFPGA0" << endl;
				solved=1;
			} else if((i0trap[0]&0x00000010)>0){  // Multi-Xmit error
				thisDDU->infpga_XmitErr(INFPGA0);
				sprintf(buf," *Multiple SpecialWord bit-errors for DDUinput[7:0]=0x%02x* ",(thisDDU->infpga_code0&0x00ff));
				if(iFill==1||iFill==3)sprintf(buf," *Extra or Missing 16-bit words for DDUinput[7:0]=0x%02x* ",(thisDDU->infpga_code0&0x00ff));
				*out << buf << endl;
				if(((i0trap[0]&0x00000020)>0))*out << "  ^^^multiple bit-errors in the same word " << endl;  // typical for offset in DMBhdr1+2, maybe TMBtr/DMBtr
				else *out << "  ^^^single bit-errors in different words " << endl;
				if(iFill==1||iFill==3)*out << "  ^^^probably related to 64-bit Align Error for InFPGA0" << endl;
				solved=1;
			} else if((i0trap[0]&0x00000008)>0){  // InFPGA0 Memory Full
				if((i0trap[0]&0x00040000)>0){
					sprintf(buf," *Memory error for DDU InRD0* ");
					*out << buf << endl;
					if(((i0trap[5]&0x0000001f)>0)){
						sprintf(buf,"  ^^^L1A buffer overflow (%ld memories available) ",i0trap[5]&0x0000001f);
						*out << buf << endl;
					} else{
						*out << "  ^^^all InMem units used " << endl;
						if(iFill==1||iFill==3)*out << "  ^^^may have caused 64-bit Align Error for InFPGA0" << endl;
					}
				} else if((i0trap[0]&0x00400000)>0){
					sprintf(buf," *Memory error for DDU InRD1* ");
					*out << buf << endl;
					if(((i0trap[5]&0x000003e0)>0)){
						sprintf(buf,"  ^^^L1A buffer overflow (%ld memories available) ",(i0trap[5]>>5)&0x0000001f);
						*out << buf << endl;
					} else{
						*out << "  ^^^all InMem units used " << endl;
						if(iFill==1||iFill==3)*out << "  ^^^may have caused 64-bit Align Error for InFPGA0" << endl;
					}
				}
				solved=1;
			} else if((i0trap[0]&0x002001fc)==0x00200000){
				sprintf(buf," *Multiple L1A errors for DDUinput[7:0] = 0x%02lx* ",((i0trap[2]>>16)&0x000000ff));
				*out << buf << endl;
				solved=1;
			}
		}
		if(igot_i1>0){  // got_i1trap;
			//      *out << "-debug> inside 7>" << endl;
			if((i1trap[0]&0x00000040)>0){
				if((i1trap[3]&0x007f0000)>0)sprintf(buf," *Start Timeout for DDUinput[14:8] = 0x%02lx* ",((i1trap[3]>>16)&0x0000007f));
				else if((i1trap[4]&0x00007f7f)>0)sprintf(buf," *End Timeout for DDUinput[14:8] = 0x%02lx* ",((i1trap[4]>>8)|i1trap[4])&0x0000007f);
				*out << buf << endl;
				if(iFill>1)*out << "  ^^^may have caused 64-bit Align Error for InFPGA1" << endl;
				solved=1;
			} else if((i1trap[0]&0x00000080)>0){  // StuckData
				sprintf(buf," *StuckData error for DDUinput[14:8] = 0x%02lx* ",((i1trap[2]>>24)&0x0000007f));
				*out << buf << endl;
				if(iFill>1)*out << "  ^^^may have caused 64-bit Align Error for InFPGA1" << endl;
				solved=1;
			} else if((i1trap[0]&0x00000010)>0){  // Multi-Xmit error
				thisDDU->infpga_XmitErr(INFPGA1);
				sprintf(buf," *Multiple SpecialWord bit-errors for DDUinput[14:8]=0x%02x* ",(thisDDU->infpga_code0&0x007f));
				if(iFill>1)sprintf(buf," *Extra or Missing 16-bit words for DDUinput[14:8]=0x%02x* ",(thisDDU->infpga_code0&0x007f));
				*out << buf << endl;
				if(((i1trap[0]&0x00000020)>0))*out << "  ^^^multiple bits in the same word " << endl;
				else *out << "  ^^^single bits in different words " << endl;
				if(iFill>1)*out << "  ^^^probably related to 64-bit Align Error for InFPGA1" << endl;
				solved=1;
			} else if((i1trap[0]&0x00000008)>0){  // InFPGA1 Memory Full
				if((i1trap[0]&0x00040000)>0){
					sprintf(buf," *Memory error for DDU InRD2* ");
					*out << buf << endl;
					if(((i1trap[5]&0x0000001f)>0)){
						sprintf(buf,"  ^^^L1A buffer overflow (%ld memories available) ",i1trap[5]&0x0000001f);
						*out << buf << endl;
					} else{
						*out << "  ^^^all InMem units used " << endl;
						if(iFill>1)*out << "  ^^^may have caused 64-bit Align Error for InFPGA1" << endl;
					}
				} else if((i1trap[0]&0x00400000)>0){
					sprintf(buf," *Memory error for DDU InRD3* ");
					*out << buf << endl;
					if(((i1trap[5]&0x000003e0)>0)){
						sprintf(buf,"  ^^^L1A buffer overflow (%ld memories available) ",(i1trap[5]>>5)&0x0000001f);
						*out << buf << endl;
					} else{
						*out << "  ^^^all InMem units used " << endl;
						if(iFill>1)*out << "  ^^^may have caused 64-bit Align Error for InFPGA1" << endl;
					}
				}
				solved=1;
			} else if((i1trap[0]&0x002001fc)==0x00200000){
				sprintf(buf," *Multiple L1A errors for DDUinput[14:8] = 0x%02lx* ",((i1trap[2]>>16)&0x0000007f));
				*out << buf << endl;
				solved=1;
			}
		}
		if(solved<1&&iFill<1)*out << "  InFPGAs are not related to the cause of the problem" << endl;
	}

	// if it's not InFPGA related:
	if(solved<1){
		if((lcode[0]&0x4000000A)==8 && (lcode[1]&0x00004000)==0){  // DDU Buff ovfl
			*out << "**DDU FIFO Full** " << endl;
			if(lcode[5]&0x000000f0){  // InRd Mem
				sprintf(buf," ^^^Memory error for DDU InRd[3:0]=0x%01lx ",(lcode[5]>>4)&0x0000000f);
				*out << buf;
			}
			*out << "  ^^^Should've been detected at InFPGA level... ";
			*out << " " << endl;
			solved=1;
		}

		if((lcode[2]&0x00000080)>0&&(lcode[1]&0x00080000)==0&&(lcode[0]&0x90000000)==0x80000000){  //  LCT-DAV error == DMBcrit + !BadTrgTrail + !CFEBcntErr
			thisDDU->ddu_rddmberr();
			*out << " *DDUctrl saw mismatch for DMB-CFEB LCT/DAV/Movlp* " << endl;
			sprintf(buf,"    ^^^Detected Error from DDUinput[14:0]=0x%04x ",(thisDDU->ddu_code0)&0x7fff);
			*out << buf << endl;
			solved=1;
		}

		if((lcode[2]&0x00000080)>0&&(lcode[1]&0x00080000)>0){  //  DMBcrit + BadTrgTrail
			thisDDU->ddu_rddmberr();
			*out << " *DDUctrl saw Trigger Trailer Error* " << endl;
			sprintf(buf,"    ^^^Detected Error from DDUinput[14:0]=0x%04x ",(thisDDU->ddu_code0)&0x7fff);
			*out << buf << endl;
			if(lcode[2]&0x00000008){    // HALCTerr
				thisDDU->ddu_rdalcterr();
				sprintf(buf,"    ^^^ALCT Errors from DDUinput[14:0]=0x%04x ",(thisDDU->ddu_code0)&0x7fff);
				*out << buf << endl;
			}
			if(lcode[2]&0x00000001){    // HTMBerr
				thisDDU->ddu_rdtmberr();
				sprintf(buf,"    ^^^TMB Errors from DDUinput[14:0]=0x%04x ",(thisDDU->ddu_code0)&0x7fff);
				*out << buf << endl;
			}
			if(lcode[0]&0x00000010)*out << "    ^^^Probably caused Multi-SpecialWord errors" << endl;
			solved=1;
		} else if((lcode[2]&0x00000080)>0&&(lcode[0]&0x10000000)>0){  //  DMBcrit + CFEBcntErr
			thisDDU->ddu_rddmberr();
			*out << " *DDUctrl saw wrong CFEB count in data* " << endl;
			sprintf(buf,"    ^^^Detected Error from DDUinput[14:0]=0x%04x ",(thisDDU->ddu_code0)&0x7fff);
			*out << buf << endl;
		}

		if(lcode[0]&0x00000010){  // DDUctrl Multi-Xmit-Err
			thisDDU->ddu_rdxmiterr();
			sprintf(buf," *DDUctrl Multiple SpecialWord bit-errors for DDUinput[14:0]=0x%04x* ",thisDDU->ddu_code0);
			*out << buf << endl;
			if(lcode[0]&0x0000800)*out << "  ^^^DDUctrl FPGA saw 64-bit-misalign flag, reported at InFPGA level? "  << endl;
			solved=1;
		}
		if(lcode[0]&0x00000004){  // DDUctrl Fiber/FIFO Connect error
			sprintf(buf," *DDUctrl Fiber/FIFO error for Ext.FIFO[3:0]=0x%01x* ",(unsigned int) (lcode[3]>>24)&0x0000000f);
			*out << buf << endl;
			if((CSCstat&0x7fff)>0){
				sprintf(buf,"  ^^^possibly for DDUinput[14:0]=0x%04x ",(CSCstat&0x7fff));
				*out << buf << endl;
			}
			*out << "  ^^^Fiber error not reported at InFPGA level "  << endl;
			solved=1;
		}
		if(lcode[0]&0x00000040){  // DDUctrl Timeout error
			sprintf(buf," *DDUctrl Timeout error, ");
			if(lcode[4]&0x00f00000)sprintf(buf2,"Start Timeout for Ext.FIFO[3:0]=0x%1lx* ",(lcode[3]>>20)&0x0000000f);
			if(lcode[4]&0x0f000000)sprintf(buf2,"End-Wait Timeout for Ext.FIFO[3:0]=0x%1lx* ",(lcode[3]>>24)&0x0000000f);
			if(lcode[4]&0xf0000000)sprintf(buf2,"End-Busy Timeout for Ext.FIFO[3:0]=0x%1lx* ",(lcode[3]>>28)&0x0000000f);
			*out << buf << buf2 << endl;
			if((CSCstat&0x7fff)>0){
				sprintf(buf,"  ^^^probably for DDUinput[14:0]=0x%04x ",(CSCstat&0x7fff));
				*out << buf << endl;
			}
			solved=1;
		}
			if(lcode[0]&0x02000000){  // DDUctrl StuckDat error
			sprintf(buf," *DDUctrl StuckData error, Ext.FIFO[3:0]=0x%01lx* ",(lcode[3]>>28)&0x0000000f);
			*out << buf << endl;
			if((CSCstat&0x7fff)>0){
				sprintf(buf,"  ^^^probably for DDUinput[14:0]=0x%04x ",(CSCstat&0x7fff));
				*out << buf << endl;
			}
			solved=1;
		}
	}

	if(solved<1){ // Stopgap measures if not solved:
	//                check for LID+cause, else LIE+cause, related errors...
		if(lcode[0]&0x00000080){  // LID error, critical error
			*out << " **Big data corruption problem detected, DDU Lost in Data** " << endl;
			if(lcode[1]&0x08000000)*out << "   Bad 2nd DMBtr";
			if(lcode[1]&0x04000000)*out << "   Extra 1st DMBtr";
			if(lcode[1]&0x02000000)*out << "   Extra 1st DMBhdr";
			if(lcode[1]&0x01000000)*out << "   Extra 2nd DMBhdr";
			if(lcode[1]&0x00200000)*out << "   invalid Lone Word signal";
			if(lcode[1]&0x00080000)*out << "   Bad Trig Trail";
			*out << " " << endl;
		} else if(lcode[0]&0x00000100){  // LIE error, not critical
			*out << " *Small data corruption problem detected, DDU Lost in Event*" << endl;
			if(lcode[1]&0x00080000)*out << "   Missed Trig Trail";
			if(lcode[1]&0x00040000)*out << "   Bad 1st DMBhdr";
			if(lcode[1]&0x10000000)*out << "   2nd DMBhdr 1st";
		// Should have been detected and solved above:
			if(lcode[1]&0x00020000)*out << "   DDU FIFO Transfer error (bad C-code)";
			*out << " " << endl;
		}
		if(lcode[0]&0x00040000){  // ALCT error, not critical
			thisDDU->ddu_rdalcterr();
			*out << " *ALCT data problem detected:";
			if(lcode[2]&0x00000800)*out << "   CRCerr";
			if(lcode[2]&0x00001000)*out << "   L1A mismatch";
			if(lcode[2]&0x00000400)*out << "   WordCountErr";
			if((lcode[2]&0x00001c00)==0)*out << "   ALCT problem on DMB, likely Full FIFO, maybe Timeout";
			*out << "* " << endl;
			sprintf(buf,"    ^^^Detected Error from DDUinput[14:0]=0x%04x ",(thisDDU->ddu_code0)&0x7fff);
			*out << buf << endl;
			//      solved=1;
		}
		if(lcode[0]&0x00080000){  // TMB error, not critical
			thisDDU->ddu_rdtmberr();
			*out << " *TMB data problem detected:";
			if(lcode[2]&0x00000008)*out << "   CRCerr";
			if(lcode[2]&0x00000010)*out << "   L1A mismatch";
			if(lcode[2]&0x00000004)*out << "   WordCountErr";
			if((lcode[2]&0x0000001c)==0)*out << "   TMB problem on DMB, Timeout or Full FIFO";
			*out << "* " << endl;
			sprintf(buf,"    ^^^Detected Error from DDUinput[14:0]=0x%04x ",(thisDDU->ddu_code0)&0x7fff);
			*out << buf << endl;
			//      solved=1;
		}

		if((lcode[1]&0x00004000)>0||(lcode[0]&0x00000008)>0){  // Mult-L1A error:
		//        confirmed CFEB L1err; DMB(hdr/tr), TMB or ALCT combined & accumulated
			thisDDU->ddu_rddmberr();
			*out << " *Cause was Multiple L1A errors* " << endl;
			if((lcode[0]&0x90400fff)==0x0000000a&&(lcode[4]&0x00008000)>0){
				sprintf(buf,"  ^^^DDU C-code L1A error, Ext.FIFO[3:0]=0x%01lx* ",(lcode[3]>>20)&0x0000000f);  // TrgL1 & DMBtrL1 not involved
				*out << buf << endl;
				solved=1;
			} else if((lcode[0]&0x9fc00fff)==0x0000020a){
				sprintf(buf,"  ^^^DMB L1A Mismatch (not from InFPGA) for DDUinput[14:0]=0x%04x ",(thisDDU->ddu_code0)&0x7fff);
				*out << buf << endl;
				solved=1;
			} else if((lcode[0]&0x9fc00dff)==0x0040000a){
				*out << "  ^^^DMB L1A Mismatch error (from InFPGA) " << endl;
				if(lcode[3]&0x00f00000){
					sprintf(buf,"   ^^errors found for Ext.FIFO[3:0]=0x%01lx* ",(lcode[3]>>20)&0x0000000f);
					*out << buf << endl;
				}
				solved=2;
			} else if((lcode[0]&0x00000002)>0){
				*out << "  ^^^Likely caused by DMB L1A Mismatch, other errors too " << endl;  // other things wrong too
				if(lcode[3]&0x00f00000){
					sprintf(buf,"   ^^errors found for Ext.FIFO[3:0]=0x%01lx* ",(lcode[3]>>20)&0x0000000f);
					*out << buf << endl;
				}
				solved=2;
			}
			if((lcode[0]&0xdf800dfd)==0x40000008)*out << "  ^^^CFEB L1A Mismatch " << endl;
			else if((lcode[0]&0x40000008)>0)*out << "  ^^^CFEB L1A Mismatch, other errors too " << endl;  // other things wrong too
			if((lcode[0]&0x40000008)>0)solved=2;
			if(((thisDDU->ddu_code0)&0x7fff)>0&&solved==2){
				sprintf(buf,"   ^^errors found for DDUinput[14:0]=0x%04x ",(thisDDU->ddu_code0)&0x7fff);
				*out << buf << endl;
				solved=1;
			}

			if((lcode[0]&0x9f810df5)==0x00010000)*out << "  ^^^Trigger L1A Mismatch " << endl;
			else if((lcode[0]&0x00010000)>0)*out << "  ^^^Trigger L1A Mismatch, other errors too " << endl;
			if((lcode[0]&0x00010000)>0){
				solved=1;
				thisDDU->ddu_rdalcterr();
				if(lcode[2]&0x00001000)*out << "   ^^ALCT L1A Mismatch " << endl;
				else if(lcode[0]&0x00040000)*out << "   ^^ALCT errors present " << endl;
				if((thisDDU->ddu_code0)&0x7fff){
					sprintf(buf,"    ^ALCT errors found for DDUinput[14:0]=0x%04x ",(thisDDU->ddu_code0)&0x7fff);
					*out << buf << endl;
				}

				thisDDU->ddu_rdtmberr();
				if(lcode[2]&0x00000010)*out << "   ^^TMB L1A Mismatch " << endl;
				else if(lcode[0]&0x00080000)*out << "   ^^TMB errors present " << endl;
				if((thisDDU->ddu_code0)&0x7fff){
					sprintf(buf,"    ^TMB errors found for DDUinput[14:0]=0x%04x ",(thisDDU->ddu_code0)&0x7fff);
					*out << buf << endl;
				}
			}
		}
	}

	if(solved<1){
		*out << " *Error cause not clearly determined*" << endl;
		sprintf(buf,"    ^^^likely came from DDUinput[14:0]=0x%04x ",CSCstat&0x7fff);
		*out << buf << endl;
	}

	/*
	Are all DDU critical problems covered?
	--if SPWD/Mxmit (esp. from stat) check for ALCT/TMB errors (from lcode)
	Ideas (firmware & software):
	-FILLed does not Solve...want to see if lost/offset TrgTrail caused it
		-> check for Fill combined with others: .not.C-codeErr (lcode[2]27:26)!
			distinguish DMB Trail/CRC error from CFEB/CRC error?
			==> CRCerr+!DMBerr+(DMBcrcOK?) @CFEB
			==> CRCerr+DMBerr @DMB
	-include TMB/ALCT Err in CSCsumErrReg that goes in DAQ path?
		-> how does DMB handle it?  Good enough?  Remove SCAovfl case?
		-> any way to set CSC-specific FMM path bits?
	-include logic for multi-TMB/ALCT L1error (WC,CRC?) sets Crit/SyncErr?
		-> how does DMB handle it?  Good enough?
		-> Multi-Trg-L1A causes error?
		-> CRC/WC go to a "MultiXmitErr" Reg?
	-InMl1aErr could go someplace in FPGA logic?
	*/
	// JRGhere, End Error Decode ^^^^^


	return out->str();
}



std::map<string, string> DDUDebugger::KillFiber(long int stat)
{
	std::map<string, string> returnValues;

	if ((stat&0x00018000)==0x8000) returnValues["ALCT checking is disabled"] = "blue";
	if ((stat&0x00028000)==0x8000) returnValues["TMB checking is disabled"] = "blue";
	//if ((stat&0x00008000)>0&&(stat&0x00030000)<0x00030000) *out << br() ;
	if ((stat&0x00048000)==0x8000) returnValues["CFEB DAV/LCT/MOVLP/L1A checks disabled"] = "blue";
	if ((stat&0x00088000)==0x8000) returnValues["Some DMB checks disabled for SP/TF compatibility"] = "blue";
	if ((stat&0x00008000)==0) returnValues["All checks are Enabled"] = "green";

	return returnValues;
}



std::map<string, string> DDUDebugger::InFPGAStat(enum DEVTYPE dt, unsigned long int stat)
{
	std::map<string, string> returnValues;

	unsigned int fiberOffset = (dt == INFPGA0 ? 0 : 8);
	//*out << "<blockquote><font size=-1 color=red face=arial>";
	if (stat&0xF0000000){
		if (0x80000000&stat) returnValues["DLL2 Lock Error"] = "red";
		// if (0x80000000&stat) returnValues["DLL-2 Not Locked"] = "red";
		// if (0x40000000&stat) returnValues["DLL-1 Not Locked"] = "red";
		if (0x40000000&stat) returnValues["64-bit Filler was used"] = "red";
		if (0x20000000&stat) {
			ostringstream fibers;
			fibers << (1 + fiberOffset * 2 / 8);
			returnValues["RdCtrl-"+fibers.str()+" Not Ready"] = "red";
		}
		if (0x10000000&stat) {
			ostringstream fibers;
			fibers << (0 + fiberOffset * 2 / 8);
			returnValues["RdCtrl-"+fibers.str()+" Not Ready"] = "red";
		}
	}
	if (stat&0x0F000000){
		if (0x08000000&stat) returnValues["NoLiveFiber 0 or 1"] = "blue";
		if (0x04000000&stat) returnValues["DLL Error occurred"] = "blue";
		if (0x02000000&stat) {
			ostringstream fibers;
			fibers << (1 + fiberOffset * 2 / 8);
			returnValues["InRD"+fibers.str()+" DMB Warn"] = "none";
		}
		if (0x01000000&stat) {
			ostringstream fibers;
			fibers << (0 + fiberOffset * 2 / 8);
			returnValues["InRD"+fibers.str()+" DMB Warn"] = "none";
		}
	}
	if (stat&0x00F00000){
		ostringstream registers;
		registers << (1 + fiberOffset * 2 / 8);
		if (0x00800000&stat) {
			returnValues["InRD"+registers.str()+" DMB Full"] = "blue";
		}
		if (0x00400000&stat) {
		
			returnValues["Mem/FIFO-InRD"+registers.str()+" Error"] = "red";
		}
		if (0x00200000&stat) {
			returnValues["MultL1A Error-InRD"+registers.str()] = "red";
		}
		if (0x00100000&stat) {
			ostringstream fibers;
			fibers << (4+fiberOffset) << "-" << (7+fiberOffset);
			returnValues["NoLiveFiber"+fibers.str()] = "none";
		}
	}
	if (stat&0x000F0000){
		ostringstream registers;
		registers << (0 + fiberOffset * 2 / 8);
		if (0x00080000&stat) returnValues["InRD"+registers.str()+" DMB Full"] = "blue";
		if (0x00040000&stat) returnValues["Mem/FIFO-InRD"+registers.str()+" Error"] = "red";
		if (0x00020000&stat) returnValues["MultL1A Error-InRD"+registers.str()] = "red";
		if (0x00010000&stat) {
			ostringstream fibers;
			fibers << (0+fiberOffset) << "-" << (3+fiberOffset);
			returnValues["NoLiveFiber"+fibers.str()] = "none";
		}
	}
	// JRG, low-order 16-bit status (most serious errors):
	if (stat&0x0000F000){
		if (0x00008000&stat) returnValues["Critical Error ** needs reset **"] = "error";
		if (0x00004000&stat) returnValues["Single Error, bad event"] = "orange";
		if (0x00002000&stat) returnValues["Single warning, possible data problem"] = "blue";
		if (0x00001000&stat) returnValues["Near Full Warning"] = "blue";
	}
	if (stat&0x00000F00){
		if (0x00000800&stat) returnValues["RX Error occurred"] = "blue";
		if (0x00000400&stat) returnValues["DLL Error (recent)"] = "blue";
		if (0x00000200&stat) returnValues["SCA Full detected"] = "orange";
		if (0x00000100&stat) returnValues["Special Word voted-bit warning"] = "blue";
	}
	if (stat&0x000000F0){
		if (0x00000080&stat) returnValues["Stuck Data occurred"] = "red";
		if (0x00000040&stat) returnValues["Timeout occurred"] = "red";
		if (0x00000020&stat) returnValues["Multiple voted-bit Errors"] = "red";
		if (0x00000010&stat) returnValues["Multiple Transmit Errors"] = "red";
	}
	if (stat&0x0000000F){
		if (0x00000008&stat) returnValues["Mem/FIFO Full Error"] = "red";
		if (0x00000004&stat) returnValues["Fiber Error"] = "red";
		if (0x00000002&stat) returnValues["L1A Match Error"] = "orange";
		if (0x00000001&stat) returnValues["Not Ready Error"] = "red";
	}

	return returnValues;
}



std::map<string, string> DDUDebugger::FIFOStat(enum DEVTYPE dt, int stat)
{
	std::map<string, string> returnValues;

	//unsigned int fiberOffset = (dt == INFPGA0 ? 0 : 8);
	//*out << "<blockquote><font size=-1 color=black face=arial>";
	if (stat&0x00000055){
		//returnValues["&nbsp InRD0 Status: &nbsp <font color=blue>";
		if (0x00000040&stat) returnValues["Ext.FIFO 3/4 Full"] = "blue";
		if (0x00000010&stat) returnValues["L1A FIFO Almost Full"] = "blue";
		if (0x00000004&stat) returnValues["MemCtrl Almost Full"] = "blue";
		//*out << "</font>";
		if (0x00000001&stat) returnValues["L1A FIFO Empty"] = "none";
		if (0x00000001&stat==0) returnValues["L1A FIFO Not Empty"] = "none";
	}
	if (stat&0x000000AA){
		//returnValues["&nbsp InRD1 Status: &nbsp <font color=blue>";
		if (0x00000080&stat) returnValues["Ext.FIFO 3/4 Full"] = "blue";
		if (0x00000020&stat) returnValues["L1A FIFO Almost Full"] = "blue";
		if (0x00000008&stat) returnValues["MemCtrl Almost Full"] = "blue";
		//*out << "</font>";
		if (0x000002&stat) returnValues["L1A FIFO Empty"] = "none";
		if (0x000002&stat==0) returnValues["L1A FIFO Not Empty"] = "none";
	}

	return returnValues;
}



std::map<string, string> DDUDebugger::FIFOFull(enum DEVTYPE dt, int stat)
{
	std::map<string, string> returnValues;

	//unsigned int fiberOffset = (dt == INFPGA0 ? 0 : 8);
	//*out << "<blockquote><font size=-1 color=black face=arial>";
	if (stat&0x00000005){
		//returnValues["&nbsp InRD0 Status: &nbsp <font color=red>";
		if (0x00000004&stat) returnValues["Ext.FIFO Full Occurred"] = "red";
		if (0x00000001&stat) returnValues["L1A FIFO Full Occurred"] = "red";
		//*out << "</font>" << br();
	}
	if (stat&0x0000000A){
		//returnValues["&nbsp InRD1 Status: &nbsp <font color=red>";
		if (0x00000008&stat) returnValues["Ext.FIFO Full Occurred"] = "red";
		if (0x00000002&stat) returnValues["L1A FIFO Full Occurred"] = "red";
		//*out << "</font>" << br();
	}

	return returnValues;
}



std::map<string, string> DDUDebugger::CCodeStat(enum DEVTYPE dt, int stat)
{
	std::map<string, string> returnValues;

	//unsigned int fiberOffset = (dt == INFPGA0 ? 0 : 8);
	//out << "<blockquote><font size=-1 color=black face=arial>";
	if (stat&0x000000ff){
		//*out << " &nbsp InRD0: &nbsp <font color=red>";
		if (0x000080&stat) returnValues["Critical Error ** needs reset **"] = "error";
		if (0x000040&stat) returnValues["Sync Error ** needs reset **"] = "error";
		if (0x000020&stat) returnValues["Single Error"] = "orange";
		if (0x000010&stat) returnValues["FIFO Overflow detected"] = "red";
		if (0x000008&stat) returnValues["Fiber Connection Error"] = "red";
		if (0x000004&stat) returnValues["Multi-Transmit Error"] = "red";
		if (0x000002&stat) returnValues["Stuck Data"] = "red";
		if (0x000001&stat) returnValues["Timeout"] = "red";
		//*out  << "</font>" << br();
	}
	if (stat&0x0000ff00){
		//*out << " &nbsp InRD1 &nbsp <font color=red>";
		if (0x0008000&stat) returnValues["Critical Error ** needs reset **"] = "error";
		if (0x00004000&stat) returnValues["Sync Error ** needs reset **"] = "error";
		if (0x00002000&stat) returnValues["Single Error"] = "orange";
		if (0x00001000&stat) returnValues["FIFO Overflow detected"] = "red";
		if (0x00000800&stat) returnValues["Fiber Connection Error"] = "red";
		if (0x00000400&stat) returnValues["Multi-Transmit Error"] = "red";
		if (0x00000200&stat) returnValues["Stuck Data"] = "red";
		if (0x00000100&stat) returnValues["Timeout"] = "red";
		//*out  << "</font>" << br();
	}

	return returnValues;
}




std::map<string, string> DDUDebugger::FiberDiagnostics(enum DEVTYPE dt, int reg, unsigned long int stat)
{
	std::map<string, string> returnValues;

	unsigned int fiberOffset = (dt == INFPGA0 ? 0 : 8);
	if (0x1f000000&stat) {
		ostringstream fiber;
		fiber << "Fiber " << (3 + fiberOffset + 4*reg) << ": " << ((0x1f000000&stat) >> 24);
		returnValues[fiber.str()] = "none";
	}
	if (0x001f0000&stat) {
		ostringstream fiber;
		fiber << "Fiber " << (2 + fiberOffset + 4*reg) << ": " << ((0x001f0000&stat) >> 16);
		returnValues[fiber.str()] = "none";
	}
	if (0x00001f00&stat) {
		ostringstream fiber;
		fiber << "Fiber " << (1 + fiberOffset + 4*reg) << ": " << ((0x00001f00&stat) >> 8);
		returnValues[fiber.str()] = "none";
	}
	if (0x0000001f&stat) {
		ostringstream fiber;
		fiber << "Fiber " << (0 + fiberOffset + 4*reg) << ": " << ((0x0000001f&stat) >> 0);
		returnValues[fiber.str()] = "none";
	}

	return returnValues;
}





std::map<string, string> DDUDebugger::WriteMemoryActive(enum DEVTYPE dt, int iFiber, int stat)
{
	std::map<string, string> returnValues;

	unsigned int fiberOffset = (dt == INFPGA0 ? 0 : 8);
	int fiber0 = stat & 0x1f;
	int fiber1 = (stat >> 5) & 0x1f;
	if (fiber0 != 0x1f) {
		ostringstream fiber;
		fiber << "Fiber " << (0 + iFiber*2 + fiberOffset) << ": " << fiber0;
		returnValues[fiber.str()] = "none";
	} else {
		ostringstream fiber;
		fiber << "Fiber " << (0 + iFiber*2 + fiberOffset) << " is inactive";
		returnValues[fiber.str()] = "none";
	}
	if (fiber1 != 0x1f) {
		ostringstream fiber;
		fiber << "Fiber " << (1 + iFiber*2 + fiberOffset) << ": " << fiber1;
		returnValues[fiber.str()] = "none";
	} else {
		ostringstream fiber;
		fiber << "Fiber " << (1 + iFiber*2 + fiberOffset) << " is inactive";
		returnValues[fiber.str()] = "none";
	}

	return returnValues;
}



string DDUDebugger::infpga_trap(DDU *thisDDU, enum DEVTYPE dt)
{
	ostringstream *out = new ostringstream();

	thisDDU->infpga_trap(dt);

	unsigned long int lcode[10];
	int i;
	char buf[100],buf1[100],buf2[100],buf3[100],buf4[100];
	char cbuf1[20],cbuf2[20],cbuf3[20],cbuf4[20];
	char sred[20]="<font color=red>";
	char syel[20]="<font color=orange>";
	char sblu[20]="<font color=blue>";
	char sgrn[20]="<font color=green>";
	char snul[20]="</font>";
	sprintf(buf1," ");
	sprintf(buf2," ");
	sprintf(buf3," ");
	sprintf(buf4," ");
	sprintf(cbuf1," ");
	sprintf(cbuf2," ");
	sprintf(cbuf3," ");
	sprintf(cbuf4," ");
	lcode[0]=thisDDU->fpga_lcode[0];
	lcode[1]=thisDDU->fpga_lcode[1];
	lcode[2]=thisDDU->fpga_lcode[2];
	lcode[3]=thisDDU->fpga_lcode[3];
	lcode[4]=thisDDU->fpga_lcode[4];
	lcode[5]=thisDDU->fpga_lcode[5];

	sprintf(buf,"  192-bit DDU InFPGA Diagnostic Trap (24 bytes) \n");
	*out << buf << endl;

	i=23;
	sprintf(buf,"                        LFfull MemAvail C-code End-TO");
	*out << buf << endl;
	sprintf(buf,"      rcv bytes %2d-%2d:",i,i-7);
	sprintf(cbuf1,"%s",sgrn);
	if(0x000f8000&lcode[5]<3)sprintf(cbuf1,"%s",sblu);
	if(0x000f8000&lcode[5]<2)sprintf(cbuf1,"%s",syel);
	if((0x000f8000&lcode[5]<1)||(0xfff00000&lcode[5]))sprintf(cbuf1,"%s",sred);
	sprintf(buf1,"%s   %04lx%s",cbuf1,(0xffff0000&lcode[5])>>16,snul);
	sprintf(cbuf2,"%s",sgrn);
	if((0x001f&lcode[5]<3)||(0x03e0&lcode[5]<3)||(0x7c00&lcode[5]<3)||(0x000f8000&lcode[5]<3))sprintf(cbuf2,"%s",sblu);
	if((0x001f&lcode[5]<2)||(0x03e0&lcode[5]<2)||(0x7c00&lcode[5]<2)||(0x000f8000&lcode[5]<2))sprintf(cbuf2,"%s",syel);
	if((0x001f&lcode[5]<1)||(0x03e0&lcode[5]<1)||(0x7c00&lcode[5]<1)||(0x000f8000&lcode[5]<1))sprintf(cbuf2,"%s",sred);
	if(0x000f8000&lcode[5]<3){
		sprintf(buf2,"%s    <blink>%04lx</blink>%s",cbuf2,0xffff&lcode[5],snul);
	}else{
		sprintf(buf2,"%s    %04lx%s",cbuf2,0xffff&lcode[5],snul);
	}
	sprintf(cbuf3,"%s",sgrn);
	if(0x20200000&lcode[4])sprintf(cbuf3,"%s",syel);
	if(0xdfdf0000&lcode[4])sprintf(cbuf3,"%s",sred);
	sprintf(buf3,"%s    %04lx%s",cbuf3,(0xffff0000&lcode[4])>>16,snul);
	sprintf(cbuf4,"%s",sgrn);
	if(0xffff&lcode[4])sprintf(cbuf4,"%s",sred);
	sprintf(buf4,"%s   %04lx%s",cbuf4,0xffff&lcode[4],snul);
	*out << buf << buf1 << buf2 << buf3 << buf4 << endl;

	i=15;
	sprintf(buf,"                      Start-TO FAF/Nrdy L1err  DMBwarn");
	*out << buf << endl;
	sprintf(buf,"      rcv bytes %2d-%2d:",i,i-7);
	sprintf(cbuf1,"%s",sgrn);
	if(0xff000000&lcode[3])sprintf(cbuf1,"%s",sblu);
	if(0x00ff0000&lcode[3])sprintf(cbuf1,"%s",sred);
	sprintf(buf1,"%s   %04lx%s",cbuf1,(0xffff0000&lcode[3])>>16,snul);
	sprintf(cbuf2,"%s",sgrn);
	if(0xfc00&lcode[3])sprintf(cbuf2,"%s",sblu);
	sprintf(buf2,"%s    %04lx%s",cbuf2,0xffff&lcode[3],snul);
	sprintf(cbuf3,"%s",sgrn);
	if(0x00ff0000&lcode[2])sprintf(cbuf3,"%s",syel);
	if(0xff000000&lcode[2])sprintf(cbuf3,"%s",sred);
	sprintf(buf3,"%s    %04lx%s",cbuf3,(0xffff0000&lcode[2])>>16,snul);
	sprintf(cbuf4,"%s",sgrn);
	if(0x00ff&lcode[2])sprintf(cbuf4,"%s",sblu);
	if(0xff00&lcode[2])sprintf(cbuf4,"%s",sred);
	sprintf(buf4,"%s   %04lx%s",cbuf4,0xffff&lcode[2],snul);
	*out << buf << buf1 << buf2 << buf3 << buf4 << endl;

	i=7;
	sprintf(buf,"                        32-bit-Empty0M  32-bit-status");
	*out << buf << endl;
	sprintf(buf,"      rcv bytes %2d-%2d:",i,i-7);
	sprintf(cbuf1,"%s",sgrn);
	if(0xffff0000&lcode[1]==0xf8000000)sprintf(cbuf1,"%s",sred);
	sprintf(buf1,"%s   %04lx%s",cbuf1,(0xffff0000&lcode[1])>>16,snul);
	sprintf(cbuf2,"%s",sgrn);
	if(0xffff&lcode[1]==0xf800)sprintf(cbuf2,"%s",sred);
	sprintf(buf2,"%s    %04lx%s",cbuf2,0xffff&lcode[1],snul);
	sprintf(cbuf3,"%s",sgrn);
	if(0x0c080000&lcode[0])sprintf(cbuf3,"%s",sblu);
	if(0xf0660000&lcode[0])sprintf(cbuf3,"%s",sred);
	sprintf(buf3,"%s    %04lx%s",cbuf3,(0xffff0000&lcode[0])>>16,snul);
	sprintf(cbuf4,"%s",sgrn);
	if(0x2d00&lcode[0])sprintf(cbuf4,"%s",sblu);
	if(0x4202&lcode[0])sprintf(cbuf4,"%s",syel);
	if(0x80fd&lcode[0])sprintf(cbuf4,"%s",sred);
	sprintf(buf4,"%s   %04lx%s",cbuf4,0xffff&lcode[0],snul);
	*out << buf << buf1 << buf2 << buf3 << buf4 << endl;
	
	return out->str();
}



std::map<string, string> DDUDebugger::ParallelStat(int stat)
{
	std::map<string, string> returnValues;

	if (stat&0x0080) returnValues["VME DLL-2 Not Locked"] = "blue";
	if (stat&0x0040) returnValues["VME DLL-1 Not Locked"] = "blue";
	if (stat&0x8000) returnValues["VME FPGA detects a problem"] = "red";
	if (stat&0x4000) returnValues["VME FPGA has a clock problem"] = "red";
	if (stat&0x2000) returnValues["VME FPGA is Not Ready"] = "orange";
	if (stat&0x1000) returnValues["DDU is Not Ready"] = "orange";

	return returnValues;
}



std::map<string, string> DDUDebugger::FMMReg(int stat)
{
	std::map<string, string> returnValues;

	if (stat==4) returnValues["BUSY"] = "orange";
	else if (stat==1) returnValues["WARNING, Near Full"] = "blue";
	else if (stat==8) returnValues["READY"] = "green";
	else if (stat==2) returnValues["Lost Sync"] = "red";
	else if (stat==0xC) returnValues["ERROR"] = "error";
	else returnValues["UNDEFINED"] = "error";

	return returnValues;
}



std::map<string, string> DDUDebugger::GbEPrescale(int stat)
{
	std::map<string, string> returnValues;

	int reg0 = stat & 0xF;
	int reg1 = (stat & 0xF0) >> 4;
	int reg2 = (stat & 0xF00) >> 8;
	int reg3 = (stat & 0xF000) >> 12;
	
	if (reg0 == reg2 && reg1 == reg3 && reg0 + reg1 == 0xF) {
		if ((0x7&stat) == 0x7) returnValues["Transmitting never"] = "none";
		else {
			unsigned int prescale = 1 << reg0;
			ostringstream prescaleText;
			prescaleText << "1:" << prescale;
			returnValues["Transmitting "+prescaleText.str()+" events"] = "green";
		}
		if (0x8&stat) returnValues["Ignoring DCC/S-Link Wait"] = "orange";
	} else {
		returnValues["Transmitting never"] = "none";
	}

	return returnValues;
}



std::map<string, string> DDUDebugger::FakeL1Reg(int stat)
{
	std::map<string, string> returnValues;

	int reg0 = stat & 0xF;
	int reg1 = (stat & 0xF0) >> 4;
	int reg2 = (stat & 0xF00) >> 8;
	int reg3 = (stat & 0xF000) >> 12;
	
	if (reg0 == reg2 && reg1 == reg3 && reg0 + reg1 == 0xF) {
		if (reg0 & 0x1) returnValues["InFPGA0 passthrough"] = "orange";
		if (reg0 & 0x2) returnValues["InFPGA1 passthrough"] = "orange";
		if (reg0 & 0x4) returnValues["DDUFPGA passthrough"] = "orange";
	} else {
		returnValues["No passthrough"] = "none";
	}

	return returnValues;
}



std::map<string, string> DDUDebugger::F0EReg(int stat)
{
	std::map<string, string> returnValues;

	if ((stat & 0xFFF0) == 0xFED0) returnValues["FMM error reporting disabled"] = "orange";
	else returnValues["FMM error reporting enabled"] = "green";

	return returnValues;
}


/*
std::map<string, string> DDUDebugger::FIFOStat(int stat)
{
	std::map<string, string> returnValues;



	return returnValues;
}
*/
