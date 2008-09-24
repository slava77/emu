/*****************************************************************************\
* $Id: DDUDebugger.cc,v 1.11 2008/09/24 18:38:38 paste Exp $
*
* $Log: DDUDebugger.cc,v $
* Revision 1.11  2008/09/24 18:38:38  paste
* Completed new VME communication protocols.
*
* Revision 1.10  2008/08/25 12:25:49  paste
* Major updates to VMEController/VMEModule handling of CAEN instructions.  Also, added version file for future RPMs.
*
* Revision 1.9  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#include "DDUDebugger.h"

#include <iostream>
#include <iomanip>
#include <sstream>

#include "DDU.h"
#include "Chamber.h"

std::map<std::string, std::string> emu::fed::DDUDebugger::DDUFPGAStat(unsigned long int stat)
{
	std::map<std::string, std::string> returnValues;

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
			if (0x00300000&stat == 0x00200000) returnValues["DAQ Applied Backpressure"] = "blue";
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



std::map<std::string, std::string> emu::fed::DDUDebugger::OutputStat(int stat)
{
	std::map<std::string, std::string> returnValues;

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
				//if (0x000000cef&stat&&(0x000000cef&stat)<=0x00ff) *out << cgicc::br();
			}
			if (stat&0x00000E00) {
				//if (stat&0x00000900)==0x0800||(stat&0x00000500)==0x0400) *out << cgicc::br();
				if (0x00000800&stat && (0x00000100&stat) == 0) returnValues["DDU GbE Overflow occurred"] = "blue";
				if (0x00000400&stat && (0x00000100&stat) == 0) returnValues["GbE Transmit Limit occurred"] = "none";
				//if (0x00000200&stat&&(0x00000100&stat)==0) returnValues["GbE FIFO Always Empty &nbsp ";
				//if (0x00000100&stat) returnValues["<font color=blue>SPY/GbE Fiber Disconnect occurred</font>";
				//if (0x000000ef&stat) *out << cgicc::br();
			}
			if (stat&0x000000F0) {
				if (0x00000080&stat) returnValues["DDU DAQ-Limited Overflow occurred (DCC/S-Link Wait)"] = "red";
				if (0x00000040&stat) returnValues["DAQ (DCC/S-Link) Wait"] = "blue";
				if (0x00000020&stat) returnValues["DDU S-Link Full/Stop"] = "none";
				if (0x00000010&stat && (0x00001000&stat) == 0) returnValues["DDU S-Link Not Ready"] = "red";
			}
			if (stat&0x0000000F) {
				//if (0x0000000e&stat&&(0x00000001&stat)==0) *out << cgicc::br();
				if (0x00000008&stat && (0x00000100&stat) == 0) returnValues["GbE FIFO Full"] = "none";
				if (0x00000004&stat && (0x00000100&stat) == 0) returnValues["DDU Skipped SPY Event (GbE data not sent)"] = "none";
				if (0x00000002&stat && (0x00000100&stat) == 0) returnValues["GbE FIFO Not Empty"] = "none";
				if (0x00000001&stat) returnValues["DCC Link Not Ready"] = "blue";
			}
			//*out << "</font></blockquote>";
		}
	} else {
		//returnValues["OK"] = "none";
	}

	return returnValues;
}



std::map<std::string, std::string> emu::fed::DDUDebugger::EBReg1(int stat)
{
	std::map<std::string, std::string> returnValues;

	if (stat&0x0000ffff) {
		//*out << "<blockquote><font size=-1 color=orange face=arial>";
		if (stat&0x0000F000) {
			if (0x00008000&stat) returnValues["DMB Timeout signal, ** needs reset **"] = "error";
			if (0x00004000&stat) returnValues["Mult L1A Error occurred"] = "red";
			if (0x00002000&stat) returnValues["L1A-FIFO Near Full Warning"] = "blue";
			if (0x00001000&stat) returnValues["GbE FIFO Almost-Full"] = "none";
			//if (0x0fff&stat) *out << cgicc::br();
		}
		if (stat&0x00000F00) {
			if (0x00000800&stat) returnValues["Ext.FIFO Near Full Warning"] = "blue";
			//if (0x00000400&stat) returnValues["Near Full Warning"] = "blue";
			if (0x00000400&stat) returnValues["InSingle Warning"] = "blue";
			if (0x00000200&stat) returnValues["CFEB-CRC not OK"] = "none";
			if (0x00000100&stat) returnValues["CFEB-CRC End Error"] = "orange";
			//if (0x00ff&stat) *out << cgicc::br();
		}
		if (stat&0x000000F0) {
			if (0x00000080&stat) returnValues["CFEB-CRC Count Error"] = "orange";
			if (0x00000040&stat) returnValues["DMB or CFEB CRC Error"] = "orange";
			//if (0x00000020&stat) returnValues["Latched Trigger Trail"] = "none";
			if (0x00000020&stat) returnValues["Trigger Readout Error"] = "orange";
			if (0x00000010&stat) returnValues["Trigger Trail Done"] = "none";
			//if (0x000f&stat) *out << cgicc::br();
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



std::map<std::string, std::string> emu::fed::DDUDebugger::EBReg2(int stat)
{
	std::map<std::string, std::string> returnValues;

	if (stat&0x0000ffff) {
		if (0x00000020&stat) returnValues["Empty CSC in Event flag"] = "green";
		//if (0x0000FFDF&stat) *out << "<blockquote><font size=-1 color=orange face=arial>";
		if (stat&0x0000F000) {
			if (0x00008000&stat) returnValues["Lost In Event Error"] = "orange";
			if (0x00004000&stat) returnValues["DMB Error in Event"] = "orange";
			if (0x00002000&stat) returnValues["Control DLL Error occured"] = "blue";
			if (0x00001000&stat) returnValues["2nd Header First flag"] = "orange";
			//if(0x0fdf&stat) *out << cgicc::br();
		}
		if (stat&0x00000F00) {
			if (0x00000800&stat) returnValues["Early 2nd Trailer flag"] = "none";
			if (0x00000400&stat) returnValues["Extra 1st Trailer flag"] = "none";
			if (0x00000200&stat) returnValues["Extra 1st Header flag"] = "none";
			if (0x00000100&stat) returnValues["Extra 2nd Header flag"] = "none";
			//if(0x00df&stat) *out << cgicc::br();
		}
		if (stat&0x000000D0) {
			if (0x00000080&stat) returnValues["SCA Full detected this Event"] = "orange";
			if (0x00000040&stat) returnValues["Probable DMB Full occurred"] = "blue";
			//if (0x00000020&stat) returnValues["Empty Event flag"] = "green";
			if (0x00000010&stat) returnValues["Bad Control Word Error occurred"] = "red";
			//if(0x000f&stat) *out << cgicc::br();
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



std::map<std::string, std::string> emu::fed::DDUDebugger::EBReg3(int stat)
{
	std::map<std::string, std::string> returnValues;

	if (stat&0x0000ffff) {
		//*out << "<blockquote><font size=-1 color=black face=arial>";
		if (stat&0x0000F000) {
			if (0x00008000&stat) returnValues["Trigger Readout Error"] = "orange";
			if (0x00004000&stat) returnValues["ALCT Trailer Done"] = "none";
			if (0x00002000&stat) returnValues["2nd ALCT Trailer detected"] = "red";
			//if (0x00002000&stat) returnValues["ALCT DAV Vote True occurred"] = "none";
			if (0x00001000&stat) returnValues["ALCT L1A mismatch error occurred"] = "none";
			//if(0x0fff&stat) *out << cgicc::br();
		}
		if (stat&0x00000F00) {
			if (0x00000800&stat) returnValues["ALCT CRC Error occurred"] = "none";
			if (0x00000400&stat) returnValues["ALCT Wordcount Error occurred"] = "none";
			if (0x00000200&stat) returnValues["Missing ALCT Trailer occurred"] = "none";
			if (0x00000100&stat) returnValues["ALCT Error occurred"] = "none";
			//if(0x00ff&stat) *out << cgicc::br();
		}
		if (stat&0x000000F0) {
			if (0x00000080&stat) returnValues["DMB Critical Error occurred"] = "none";
			//if (0x00000080&stat) returnValues["Compare Trigger CRC flag"] = "none";
			if (0x00000040&stat) returnValues["TMB Trailer Done"] = "none";
			if (0x00000020&stat) returnValues["2nd TMB Trailer detected"] = "red";
			//if (0x00000020&stat) returnValues["TMB DAV Vote True occurred"] = "none";
			if (0x00000010&stat) returnValues["TMB L1A mismatch error occurred"] = "none";
			//if(0x000f&stat) *out << cgicc::br();
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



std::map<std::string, std::string> emu::fed::DDUDebugger::FIFO2(int stat)
{
	std::map<std::string, std::string> returnValues;

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



std::map<std::string, std::string> emu::fed::DDUDebugger::FFError(int stat)
{
	std::map<std::string, std::string> returnValues;

	if (stat&0x000000ff) {

		if (0x00000040&stat) returnValues["L1A FIFO Empty"] = "none";
		if (0x00000040&stat == 0) returnValues["L1A FIFO Not Empty"] = "none";
		if (0x00000002&stat) returnValues["GbE FIFO Full occurred"] = "none";
		if (0x00000001&stat) returnValues["L1A FIFO Full occurred"] = "red";

	} else {
		//returnValues["OK"] = "none";
	}


	return returnValues;
}



std::map<std::string, std::string> emu::fed::DDUDebugger::InCHistory(int stat)
{
	std::map<std::string, std::string> returnValues;

	if (stat&0x00000fff) {
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



std::map<std::string, std::string> emu::fed::DDUDebugger::WarnMon(int stat)
{
	std::map<std::string, std::string> returnValues;

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


std::vector <std::string> emu::fed::DDUDebugger::DDUDebugTrap(std::vector<uint16_t> lcode, DDU *thisDDU)
{

	std::vector<std::string> out;
	std::stringstream outStream;

	// First, spit out the full status.

	std::string debugNames[12] = {
		"o-stat",
		"fifo-full",
		"fifo-c",
		"fifo-b",
		"fifo-a",
		"in-stat",
		"c-code",
		"er-c",
		"er-b",
		"er-a",
		"32-bit stat high",
		"32-bit stat low"
	};

	// Pop out the decoded register.
	for (unsigned int iBits = 0; iBits < 12; iBits++) {
		outStream << debugNames[iBits] << ": " << std::setw(4) << std::setfill('0') << std::hex << lcode[11 - iBits];
		out.push_back(outStream.str());
		outStream.str("");
	}

// 	i = 23;
// 	sprintf(buf, "                        o-stat  fful  fifo-c fifo-b");
// 	*out << buf << std::endl;
// 	sprintf(buf, "      rcv bytes %2d-%2d:", i, i - 7);
// 	sprintf(cbuf1, "%s", sgrn);
// 	if (0x09010000&lcode[5])sprintf(cbuf1, "%s", sblu);
// 	if (0x40000000&lcode[5])sprintf(cbuf1, "%s", syel);
// 	if (0x80800000&lcode[5])sprintf(cbuf1, "%s", sred);
// 	sprintf(buf1, "%s   %04lx%s", cbuf1, (0xffff0000&lcode[5]) >> 16, snul);
// 	sprintf(cbuf2, "%s", sgrn);
// 	if (0x01ff&lcode[5])sprintf(cbuf2, "%s", sred);
// 	sprintf(buf2, "%s   %04lx%s", cbuf2, 0xffff&lcode[5], snul);
// 	sprintf(cbuf3, "%s", sgrn);
// 	if (0xffff0000&lcode[4])sprintf(cbuf3, "%s", sred);
// 	sprintf(buf3, "%s   %04lx%s", cbuf3, (0xffff0000&lcode[4]) >> 16, snul);
// 	sprintf(cbuf4, "%s", sgrn);
// 	if (0x01ff&lcode[4])sprintf(cbuf4, "%s", sblu);
// 	sprintf(buf4, "%s   %04lx%s", cbuf4, 0xffff&lcode[4], snul);
// 	*out << buf << buf1 << buf2 << buf3 << buf4 << std::endl;
// 
// 	i = 15;
// 	sprintf(buf, "                        fifo-a instat c-code  erc");
// 	*out << buf << std::endl;
// 	sprintf(buf, "      rcv bytes %2d-%2d:", i, i - 7);
// 	sprintf(cbuf1, "%s", sgrn);
// 	if (0xfff00000&lcode[3])sprintf(cbuf1, "%s", sred);
// 	sprintf(buf1, "%s   %04lx%s", cbuf1, (0xffff0000&lcode[3]) >> 16, snul);
// 	sprintf(cbuf2, "%s", sgrn);
// 	if (0x00f0&lcode[3])sprintf(cbuf2, "%s", syel);
// 	if (0xff0f&lcode[3])sprintf(cbuf2, "%s", sred);
// 	sprintf(buf2, "%s   %04lx%s", cbuf2, 0xffff&lcode[3], snul);
// 	sprintf(cbuf3, "%s", sgrn);
// 	if (0x00200000&lcode[2])sprintf(cbuf3, "%s", syel);
// 	if (0xffdf0000&lcode[2])sprintf(cbuf3, "%s", sred);
// 	sprintf(buf3, "%s   %04lx%s", cbuf3, (0xffff0000&lcode[2]) >> 16, snul);
// 	sprintf(cbuf4, "%s", sgrn);
// 	if (0x9f1f&lcode[2])sprintf(cbuf4, "%s", syel);
// 	sprintf(buf4, "%s   %04lx%s", cbuf4, 0xffff&lcode[2], snul);
// 	*out << buf << buf1 << buf2 << buf3 << buf4 << std::endl;
// 
// 	i = 7;
// 	sprintf(buf, "                         erb    era   32-bit status");
// 	*out << buf << std::endl;
// 	sprintf(buf, "      rcv bytes %2d-%2d:", i, i - 7);
// 	sprintf(cbuf1, "%s", sgrn);
// 	if (0x00110000&lcode[1])sprintf(cbuf1, "%s", syel);
// 	if (0xd08e0000&lcode[1])sprintf(cbuf1, "%s", sred);
// 	sprintf(buf1, "%s   %04lx%s", cbuf1, (0xffff0000&lcode[1]) >> 16, snul);
// 	sprintf(cbuf2, "%s", sgrn);
// 	if (0x2c00&lcode[1])sprintf(cbuf2, "%s", sblu);
// 	if (0x01e0&lcode[1])sprintf(cbuf2, "%s", syel);
// 	if (0xc00c&lcode[1])sprintf(cbuf2, "%s", sred);
// 	sprintf(buf2, "%s   %04lx%s", cbuf2, 0xffff&lcode[1], snul);
// 	sprintf(cbuf3, "%s", sgrn);
// 	if (0x21800000&lcode[0])sprintf(cbuf3, "%s", sblu);
// 	if (0xd00f0000&lcode[0])sprintf(cbuf3, "%s", syel);
// 	if (0x0e400000&lcode[0])sprintf(cbuf3, "%s", sred);
// 	sprintf(buf3, "%s   %04lx%s", cbuf3, (0xffff0000&lcode[0]) >> 16, snul);
// 	sprintf(cbuf4, "%s", sgrn);
// 	if (0x3400&lcode[0])sprintf(cbuf4, "%s", sblu);
// 	if (0x4b23&lcode[0])sprintf(cbuf4, "%s", syel);
// 	if (0x80dc&lcode[0])sprintf(cbuf4, "%s", sred);
// 	sprintf(buf4, "%s   %04lx%s", cbuf4, 0xffff&lcode[0], snul);
// 	*out << buf << buf1 << buf2 << buf3 << buf4 << std::endl;

	// Next, spit out the funky fiber information.
	unsigned long int CSCStat = thisDDU->readCSCStatus();
	if (CSCStat & 0x7fff) {
		outStream << "FMM errors detected on fiber(s) ";
		for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
			if (CSCStat & (1<<iFiber)) {
				outStream << std::dec << iFiber << " (" << thisDDU->getChamber(iFiber)->name() << ") ";
			}
		}
		out.push_back(outStream.str());
		outStream.str("");
	}
	if (CSCStat & 0x8000) {
		out.push_back("DDU is in an FMM error condition");
	}

	unsigned int DMBError = thisDDU->readDMBError();
	if (DMBError & 0x7fff) {
		outStream << "DMB errors detected on fiber(s) ";
		for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
			if (DMBError & (1<<iFiber)) {
				outStream << std::dec << iFiber << " (" << thisDDU->getChamber(iFiber)->name() << ") ";
			}
		}
		out.push_back(outStream.str());
		outStream.str("");
	}
	
	unsigned int TMBError = thisDDU->readTMBError();
	if (TMBError & 0x7fff) {
		outStream << "TMB errors detected on fiber(s) ";
		for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
			if (TMBError & (1<<iFiber)) {
				outStream << std::dec << iFiber << " (" << thisDDU->getChamber(iFiber)->name() << ") ";
			}
		}
		out.push_back(outStream.str());
		outStream.str("");
	}
	
	unsigned int ALCTError = thisDDU->readALCTError();
	if (ALCTError & 0x7fff) {
		outStream << "ALCT errors detected on fiber(s) ";
		for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
			if (ALCTError & (1<<iFiber)) {
				outStream << std::dec << iFiber << " (" << thisDDU->getChamber(iFiber)->name() << ") ";
			}
		}
		out.push_back(outStream.str());
		outStream.str("");
	}
	
	unsigned int XmitError = thisDDU->readXmitError();
	if (XmitError & 0x7fff) {
		outStream << "Transmit errors detected on fiber(s) ";
		for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
			if (XmitError & (1<<iFiber)) {
				outStream << std::dec << iFiber << " (" << thisDDU->getChamber(iFiber)->name() << ") ";
			}
		}
		out.push_back(outStream.str());
		outStream.str("");
	}

	std::vector<uint16_t> inTrap[2];
	bool inTrapSet[2] = {
		false,
		false
	};
	unsigned long int inStat[2];
	short int iFill = 0;
	bool iTimeout = false;
	bool solved = false;
	
	//unsigned long int ddustat = (0xffffffff & lcode[0]);

	unsigned long int erastat = (0x0000ffff & lcode[1]);

	// INFPGAs in order.
	enum DEVTYPE devType[2] = {
		INFPGA0,
		INFPGA1
	};
	std::string devName[2] = {
		"INFPGA0",
		"INFPGA1"
	};

	for (int iDev = 0; iDev < 2; iDev++) {
		
		inStat[iDev] = thisDDU->readFPGAStatus(devType[iDev]);
		
		if (inStat[iDev]&0x04000000) {          // DLL Error
			out.push_back(devName[iDev] + ": DLL error detected");
			if (inStat[iDev]&0x00000800) out.push_back(devName[iDev] + ": (Probable cause of Gt-Rx errors)");
			solved = true;
			
		} else if (inStat[iDev]&0x00000004) {     // Fiber Change
			unsigned int fiberCheck = thisDDU->readFiberStatus(devType[iDev]);
			outStream << devName[iDev] << ": Fiber connection error detected on fiber(s) ";
			for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
				// INFPGA0 looks at fibers 0-7, INFPGA1 looks at 8-15
				unsigned int realFiber = iFiber + iDev*8;
				if ((fiberCheck >> 8) & (1<<iFiber)) {
					outStream << std::dec << realFiber << " (" << thisDDU->getChamber(realFiber)->name() << ") ";
				}
			}
			out.push_back(outStream.str());
			outStream.str("");

			if (inStat[iDev]&0x00000800) out.push_back(devName[iDev] + ": (Probable cause of Gt-Rx errors)");
			else if (inStat[iDev]&0x00000130) out.push_back( devName[iDev] + ": (Probable cause of SpecialWord/Xmit errors)");
			solved = true;
			
		} else if (inStat[iDev]&0x00000800) {   // GT-Rx Error
			unsigned int fiberCheck = thisDDU->readRxError(devType[iDev]);
			outStream << devName[iDev] << ": GT-Rx error detected on fiber(s) ";
			for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
				// INFPGA0 looks at fibers 0-7, INFPGA1 looks at 8-15
				unsigned int realFiber = iFiber + iDev*8;
				if ((fiberCheck >> 8) & (1<<iFiber)) {
					outStream << std::dec << realFiber << " (" << thisDDU->getChamber(realFiber)->name() << ") ";
				}
			}
			out.push_back(outStream.str());
			outStream.str("");
			
			if (inStat[iDev]&0x00000130) out.push_back(devName[iDev] + ": (Probable cause of SpecialWord/Xmit errors)");
			solved = true;

		} else if (lcode[1]&0x00010000 && inStat[iDev]&0x00880000) {  // DMB-Full
			unsigned int fiberCheck = thisDDU->readDMBWarning(devType[iDev]);
			out.push_back(devName[iDev] + ": ");
			outStream << devName[iDev] << ": Confirmed DMB full on fiber(s) ";
			for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
				// INFPGA0 looks at fibers 0-7, INFPGA1 looks at 8-15
				unsigned int realFiber = iFiber + iDev*8;
				if ((fiberCheck >> 8) & (1<<iFiber)) {
					outStream << std::dec << realFiber << " (" << thisDDU->getChamber(realFiber)->name() << ") ";
				}
			}
			out.push_back(outStream.str());
			outStream.str("");
			
			if (lcode[0]&0x00040000 && (lcode[2]&0x00001c00) == 0) out.push_back(devName[iDev] + ": [DMB full fifo for ALCT]");
			else if (lcode[0]&0x00080000 && (lcode[2]&0x0000001c) == 0) out.push_back(devName[iDev] + ": [DMB full fifo for TMB]");
			else out.push_back(devName[iDev] + ": [DMB full fifo, probably for CFEB]");
			if (inStat[iDev]&0x00000130) out.push_back(devName[iDev] + ": (Probable cause of SpecialWord/Xmit errors)");
			solved = true;
		}
	}



	if (lcode[0]&0x00000400) {
		out.push_back("DDUFPGA: DLL error detected");
		solved = true;
	}
	if (lcode[5]&0x00800000 && (lcode[2]&0x0c000000) == 0) {  // DAQ-induced Buffer overflow, FullFIFO
		out.push_back("DDUFPGA: DAQ-induced buffer overflow detected");
		solved = true;
	}
	if ((lcode[0]&0x4000000A) == 8 && (lcode[1]&0x00004000) == 0 && (lcode[2]&0x0c000000) == 0) { // DDU Buff ovfl
		out.push_back("DDUFPGA: DDU FIFO full");
		if (lcode[5]&0x0000000f) {	// Ext.FIFO
			outStream << "DDUFPGA: [Memory error for DDU external fifo(s) ";
			for (int iFIFO=0; iFIFO < 4; iFIFO++) {
				if (lcode[5] & (1<<iFIFO)) outStream << std::dec << iFIFO;
			}
			out.push_back(outStream.str() + "]");
			outStream.str("");
		}
		
		if (lcode[5]&0x00000100) out.push_back("DDUFPGA: [L1A FIFO Full]");
		solved = true;
	}
	
	// That was the easy stuff.  Now it's time to debug further.
	if ((erastat | lcode[1])&0x00008000) {  // DMB-Timeout?  check at and after Critical error point, get DMB Error Reg
		out.push_back("DDUFPGA: DMB timeout signal detected on above fibers");

		// Note: if ALCT Timeout on DMB then LIE, CRC & WC errors are likely from
		//       overrun to TMB trail; look for lcode-LIE caused by bad/missTrigTrail.
		//    If DMB end-timeout causes 64-bit misalignment then maybe check that too.
		solved = true;
		iTimeout = true;

		if (lcode[0]&0x00040000 && lcode[1]&0x00080000) out.push_back("DDUFPGA: [DMB timeout for ALCT]");
		else if (lcode[0]&0x00080000 && lcode[1]&0x00080000) out.push_back("DDUFPGA: [DMB timeout for TMB]");
		else if (!(lcode[0]&0x00000800)) out.push_back("DDUFPGA: [DMB timeout, probably for CFEB]");
		else {
			out.push_back("DDUFPGA: [DMB timeout with 64-bit misalignment, possibly from CFEB]");
			solved = false;
		}
		
		if (inStat[0]&0x0000e000) out.push_back("DDUFPGA: (probable cause of problems on InFPGA0)");
		if (inStat[1]&0x0000e000) out.push_back("DDUFPGA: (probable cause of problems on InFPGA1)");
	
	} else if (lcode[2]&0x0C000000 || lcode[1]&0x00020000) {  // DDU FIFO Transfer/C-code Error
		out.push_back("DDUFPGA: DDU FIFO transfer error detected");
		if (lcode[2]&0x04000000) out.push_back("DDUFPGA: [Begin C-code error]");
		if (lcode[2]&0x08000000) out.push_back("DDUFPGA: [End C-code error]");
		outStream << "DDUFPGA: (probably occurred on external FIFO(s) ";
		for (int iFIFO = 0; iFIFO < 4; iFIFO++) {
			if ((lcode[2] >> 28) & (1<<iFIFO)) outStream << std::dec << iFIFO << " ";
		}
		outStream << "(from InMxmit Reg))";
		out.push_back(outStream.str());
		outStream.str("");
		solved = true;
	}
	
	// InCtrlErr at CritErr point:
	if (lcode[0]&0x00400000) { // InCtrlErr & NotDDUfullFIFO
		//    *out << "-debug> inside 3>" << std::endl;
		for (unsigned int iDev = 0; iDev < 2; iDev++) {
			if (inStat[iDev] & 0x40000000) { // Filler=64bit-misalign
				iFill |= (1 << iDev);
				unsigned int fiberCheck = thisDDU->readTxError(devType[iDev]);
				outStream << devName[iDev] << ": 64-bit align error detected on fiber(s) ";
				for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
				// INFPGA0 looks at fibers 0-7, INFPGA1 looks at 8-15
					unsigned int realFiber = iFiber + iDev*8;
					if ((fiberCheck >> 8) & (1<<iFiber)) {
						outStream << std::dec << realFiber << " (" << thisDDU->getChamber(realFiber)->name() << ") ";
					}
				}
				out.push_back(outStream.str());
				outStream.str("");

				if (inStat[iDev]&0x00000130) out.push_back(devName[iDev] + ": [Also associated with SpecialWord errors]");
			}

			// If InCtrlErr and not solved, get InTrap registers (each 32 bytes)
			if (inStat[iDev]&0x00008000) {
				//      *out << "-debug> inside 4>" << std::endl;
				inTrap[iDev] = thisDDU->readDebugTrap(devType[iDev]);
				inTrapSet[iDev] = true;
			}
		}
	}


	if (!solved && iFill) {  //  check for cause of misalignment early
		if (lcode[0]&0x80000002) out.push_back("(possible that DMB may have caused 64-bit align error)"); // LCT/DAV(lcode[0]31), DMBL1A(lcode[0]1)
		else if (lcode[2]&0x00000c00 || lcode[0]&0x00040000) {  // ALCTerr
			out.push_back("(possible that ALCT may have caused 64-bit align error)");
			
			if (lcode[1]&0x00080000 && lcode[0]&0x00040000) out.push_back("(probable ALCT trail word problem)");
			else if (lcode[0]&0x00000020) out.push_back("(probable ALCT CRC mismatch)");
			else if (lcode[0]&0x00020000) out.push_back("(probable ALCT wordcount mismatch)");
			
		} else if (lcode[2]&0x0000000c || lcode[0]&0x00080000) {  // TMBerr
			out.push_back("(possible that TMB may have caused 64-bit align error)");
			
			if (lcode[1]&0x00080000 && lcode[0]&0x00080000) out.push_back("(probable TMB trail word problem)");
			else if (lcode[0]&0x00000020) out.push_back("(probable TMB CRC mismatch)");
			else if (lcode[0]&0x00020000) out.push_back("(probable TMB wordcount mismatch)");
			
		} else if (lcode[1]&0x00080000) { // TrgTrail error, maybe never get this one
			out.push_back("(probable that ALCT or TMB caused 64-bit align error)");
			out.push_back("Trigger trail word problem detected");
			
		} else if (lcode[0]&0x50000001) {  // CFEBerr
			out.push_back("(CFEB may have caused 64-bit align error)");
			if (lcode[0]&0x10000000) out.push_back("CFEB count mismatch detected");
			if (lcode[0]&0x40000000) out.push_back("CFEB L1A mismatch detected");
			if (lcode[0]&0x00000001) out.push_back("CFEB or DMB CRC mismatch detected");
			if (lcode[1]&0x00000380) {
				out.push_back("[confirmed CFEB CRC error]");
				if (lcode[1]&0x00000080) out.push_back("(CFEB CRC-count error)");
				if (lcode[1]&0x00000100) out.push_back("CFEB CRC-end error");
				if (lcode[1]&0x00000200) out.push_back("CFEB CRC mismatch");
			} else if (lcode[0]&0x00000201) out.push_back("[confirmed DMB CRC mismatch at least]");
		}
		
		if (iTimeout) {
			if (lcode[1]&0x00008000) out.push_back("[Timeout at DMB caused 64-bit align error]");
			solved = true;
		}
	}

	// If InCtrlErr, determine what happened at CritErr point:
	//        Timeout/StuckDat/MultXmit/MemErr/MultL1A
	if (!solved && lcode[0]&0x00400000) {
		//    *out << "-debug> inside 5>" << std::endl;
		for (int iDev = 0; iDev < 2; iDev++) {
			if (inTrapSet[iDev]) {  // got_i0trap;
				//      *out << "-debug> inside 6>" << std::endl;
				if (inTrap[iDev][0]&0x0040) {
					if (inTrap[iDev][6]&0x00ff) {
						outStream << devName[iDev] << ": Start timeout for fiber(s) ";
						for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
							// INFPGA0 looks at fibers 0-7, INFPGA1 looks at 8-15
							unsigned int realFiber = iFiber + iDev*8;
							if ((inTrap[iDev][7]) & (1<<iFiber)) {
								outStream << std::dec << realFiber << " (" << thisDDU->getChamber(realFiber)->name() << ") ";
							}
						}
						out.push_back(outStream.str());
						outStream.str("");
					} else if (inTrap[iDev][8]) {
						outStream << devName[iDev] << ": End timeout for fiber(s) ";
						for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
							// INFPGA0 looks at fibers 0-7, INFPGA1 looks at 8-15
							unsigned int realFiber = iFiber + iDev*8;
							if (((inTrap[iDev][8] >> 8) | inTrap[iDev][8]) & (1<<iFiber)) {
								outStream << std::dec << realFiber << " (" << thisDDU->getChamber(realFiber)->name() << ") ";
							}
						}
						out.push_back(outStream.str());
						outStream.str("");
					
						if (iFill & (1 << iDev)) out.push_back("(may have caused 64-bit align error for " + devName[iDev] + ")");
						solved = true;
					} else if (inTrap[iDev][0]&0x0080) {  // StuckData
						outStream << devName[iDev] << ": stuck data for fiber(s) ";
						for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
				// INFPGA0 looks at fibers 0-7, INFPGA1 looks at 8-15
							unsigned int realFiber = iFiber + iDev*8;
							if ((inTrap[iDev][4] >> 8) & (1<<iFiber)) {
								outStream << std::dec << realFiber << " (" << thisDDU->getChamber(realFiber)->name() << ") ";
							}
						}
						out.push_back(outStream.str());
						outStream.str("");
						
						if (iFill & (1 << iDev)) out.push_back("(may have caused 64-bit align error for " + devName[iDev] + ")");
						solved = true;
					} else if (inTrap[iDev][0]&0x0010) {  // Multi-Xmit error
						unsigned int fiberCheck = thisDDU->readTxError(devType[iDev]);
						outStream << devName[iDev] << ": Multiple SpecialWord bit errors for fiber(s) ";
						for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
							// INFPGA0 looks at fibers 0-7, INFPGA1 looks at 8-15
							unsigned int realFiber = iFiber + iDev*8;
							if (fiberCheck & (1<<iFiber)) {
								outStream << std::dec << realFiber << " (" << thisDDU->getChamber(realFiber)->name() << ") ";
							}
						}
						out.push_back(outStream.str());
						outStream.str("");
						
						if (iFill & (1 << iDev)) out.push_back(devName[iDev] + ": [Extra or missing 16-bit words, too]");

						if (inTrap[iDev][0]&0x0020) out.push_back(devName[iDev] + "[multiple bit-errors in the same word]");  // typical for offset in DMBhdr1+2, maybe TMBtr/DMBtr
						else out.push_back(devName[iDev] + ": [single bit-errors in different words]");
					
						if (iFill & (1 << iDev)) out.push_back("(probably related to 64-bit align error for " + devName[iDev] + ")");
						solved = true;

					} else if (inTrap[iDev][0]&0x0008) {  // InFPGA0 Memory Full
						if ((inTrap[iDev][1]&0x0004) > 0) {
							outStream << devName[iDev] << ": Memory error for InRD" << std::dec << (0 + iDev*2);
							out.push_back(outStream.str());
							outStream.str("");
							
							if (inTrap[iDev][10]&0x001f) {
								outStream << devName[iDev] << ": [L1A buffer overflow (" << std::dec << (inTrap[iDev][10]&0x001f) << " memories available)]";
								out.push_back(outStream.str());
								outStream.str("");
							} else {
								out.push_back(devName[iDev] + ": [all InMem units used]");
								if (iFill & (1 << iDev)) out.push_back("(may have caused 64-bit align error for " + devName[iDev]);
							}
						} else if (inTrap[iDev][1]&0x0040) {
							outStream << devName[iDev] << ": Memory error for InRD" << std::dec << (1 + iDev*2);
							out.push_back(outStream.str());
							outStream.str("");
							
							if (inTrap[iDev][10]&0x03e0) {
								outStream << devName[iDev] << ": [L1A buffer overflow (" << std::dec << (inTrap[iDev][10]&0x03e0) << " memories available)]";
								out.push_back(outStream.str());
								outStream.str("");
							} else {
								out.push_back(devName[iDev] + ": [all InMem units used]");
								if (iFill & (1 << iDev)) out.push_back("(may have caused 64-bit align error for " + devName[iDev]);
							}
						}
						solved = true;
					} else if ( !(inTrap[iDev][0] & 0x01fc) && (inTrap[iDev][1]&0x0020)) {
						outStream << devName[iDev] << ": Multiple L1A errors for fiber(s) ";
						for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
							// INFPGA0 looks at fibers 0-7, INFPGA1 looks at 8-15
							unsigned int realFiber = iFiber + iDev*8;
							if ((inTrap[iDev][5]) & (1<<iFiber)) {
								outStream << std::dec << realFiber << " (" << thisDDU->getChamber(realFiber)->name() << ") ";
							}
						}
						out.push_back(outStream.str());
						outStream.str("");

						solved = true;
					}
				}
			}
		}
		
		if (!solved && !iFill) out.push_back("(InFPGAs are not related to the cause of the problem)");
	}

	// if it's not InFPGA related:
	if (!solved) {
		if ((lcode[0]&0x4000000A) == 8 && !(lcode[1]&0x00004000)) {  // DDU Buff ovfl
			out.push_back("DDU FIFO full detected");
			if (lcode[5]&0x000000f0) {  // InRd Mem
				outStream << "[Memory error for DDU detected in ";
				for (int iReg = 0; iReg < 4; iReg++) {
					if ((lcode[5] >> 4) & (1<<iReg)) outStream << "InRd" << std::dec << iReg << " ";
				}
				out.push_back(outStream.str() + "]");
				outStream.str("");
			}
			out.push_back("(Should have been detected at InFPGA level...)");
			solved = true;
		}

		if (lcode[2]&0x00000080 && !(lcode[1]&0x00080000) && (lcode[0]&0x90000000) == 0x80000000) {  //  LCT-DAV error == DMBcrit + !BadTrgTrail + !CFEBcntErr
			out.push_back("[Mismatch for DMB-CFEB LCT/DAV/Movlp detected]");

			solved = true;
		}

		if (lcode[2]&0x00000080 && lcode[1]&0x00080000) {  //  DMBcrit + BadTrgTrail
			out.push_back("[Trigger trailer error detected]");
			
			if (lcode[2]&0x00000008) {  // HALCTerr
				out.push_back("[ALCT errors detected]");
			}
			
			if (lcode[2]&0x00000001) {  // HTMBerr
				out.push_back("[TMB errors detected]");
			}
			
			if (lcode[0]&0x00000010) out.push_back("(Probably caused multi-SpecialWord errors)");
			solved = true;
			
		} else if (lcode[2]&0x00000080 && lcode[0]&0x10000000) {  //  DMBcrit + CFEBcntErr
			out.push_back("[Wrong CFEB count in data detected]");
		}

		if (lcode[0]&0x00000010) {  // DDUctrl Multi-Xmit-Err
			out.push_back("[Multiple SpecialWord bit errors detected]");
			
			if (lcode[0]&0x0000800) out.push_back("(DDUctrl FPGA saw 64-bit-misalign flag, reported at InFPGA level?)");
			solved = true;
		}

		if (lcode[0]&0x00000004) {  // DDUctrl Fiber/FIFO Connect error
			outStream << "[Fiber/FIFO error detected in external FIFO(s) ";
			for (int iReg = 0; iReg < 4; iReg++) {
				if ((lcode[3] >> 24) & (1<<iReg)) outStream << std::dec << iReg << " ";
			}
			out.push_back(outStream.str() + "]");
			outStream.str("");
			
			out.push_back("(Fiber error not reported at InFPGA level)");
			solved = true;
		}
		
		if (lcode[0]&0x00000040) {  // DDUctrl Timeout error
			
			out.push_back("[Timeout error detected]");
			if (lcode[4]&0x00f00000) {
				outStream << "(Start timeout for external FIFO(s) ";
				for (int iReg = 0; iReg < 4; iReg++) {
					if ((lcode[3] >> 20) & (1<<iReg)) outStream << std::dec << iReg << " ";
				}
				out.push_back(outStream.str() + ")");
				outStream.str("");
			}
			if (lcode[4]&0x0f000000) {
				outStream << "(End-wait timeout for external FIFO(s) ";
				for (int iReg = 0; iReg < 4; iReg++) {
					if ((lcode[3] >> 24) & (1<<iReg)) outStream << std::dec << iReg << " ";
				}
				out.push_back(outStream.str() + ")");
				outStream.str("");
			}
			if (lcode[4]&0xf0000000) {
				outStream << "(End-busy timeout for external FIFO(s) ";
				for (int iReg = 0; iReg < 4; iReg++) {
					if ((lcode[3] >> 28) & (1<<iReg)) outStream << std::dec << iReg << " ";
				}
				out.push_back(outStream.str() + ")");
				outStream.str("");
			}
			
			solved = true;
		}
		if (lcode[0]&0x02000000) {  // DDUctrl StuckDat error
			outStream << "[Stuck data error detected for external FIFO(s) ";
			for (int iReg = 0; iReg < 4; iReg++) {
				if ((lcode[3] >> 28) & (1<<iReg)) outStream << std::dec << iReg << " ";
			}
			out.push_back(outStream.str() + "]");
			outStream.str("");

			solved = true;
		}
	}

	if (!solved) { // Stopgap measures if not solved:
		//                check for LID+cause, else LIE+cause, related errors...
		if (lcode[0]&0x00000080) {  // LID error, critical error
			out.push_back("Big data corruption problem detected, DDU Lost in Data");
			if (lcode[1]&0x08000000) out.push_back("[Bad 2nd DMBtr]");
			if (lcode[1]&0x04000000) out.push_back("[Extra 1st DMBtr]");
			if (lcode[1]&0x02000000) out.push_back("[Extra 1st DMBhdr]");
			if (lcode[1]&0x01000000) out.push_back("[Extra 2nd DMBhdr]");
			if (lcode[1]&0x00200000) out.push_back("[Invalid Lone Word signal]");
			if (lcode[1]&0x00080000) out.push_back("[Bad Trig Trail]");
			
		} else if (lcode[0]&0x00000100) {  // LIE error, not critical
			out.push_back("Small data corruption problem detected, DDU Lost in Event");
			if (lcode[1]&0x00080000) out.push_back("[Missed Trig Trail]");
			if (lcode[1]&0x00040000) out.push_back("[Bad 1st DMBhdr]");
			if (lcode[1]&0x10000000) out.push_back("[2nd DMBhdr 1st]");
			// Should have been detected and solved above:
			if (lcode[1]&0x00020000) out.push_back("[DDU FIFO Transfer error (bad C-code)]");
		}
		if (lcode[0]&0x00040000) {  // ALCT error, not critical

			out.push_back("ALCT data problem detected");
			if (lcode[2]&0x00000800) out.push_back("[CRCerr]");
			if (lcode[2]&0x00001000) out.push_back("[L1A mismatch]");
			if (lcode[2]&0x00000400) out.push_back("[WordCountErr]");
			if (!(lcode[2]&0x00001c00)) out.push_back("[ALCT problem on DMB, likely Full FIFO, maybe Timeout]");
			
		}
		if (lcode[0]&0x00080000) {  // TMB error, not critical
			out.push_back("TMB data problem detected");
			if (lcode[2]&0x00000008) out.push_back("[CRCerr]");
			if (lcode[2]&0x00000010) out.push_back("[L1A mismatch]");
			if (lcode[2]&0x00000004) out.push_back("[WordCountErr]");
			if (!(lcode[2]&0x0000001c)) out.push_back("[TMB problem on DMB, Timeout or Full FIFO]");
		}

		if (lcode[1]&0x00004000 || lcode[0]&0x00000008) {  // Mult-L1A error:
			//        confirmed CFEB L1err; DMB(hdr/tr), TMB or ALCT combined & accumulated
			out.push_back("Multiple L1A errors detected");
			bool multiSolve = false;
			
			if ((lcode[0]&0x90400fff) == 0x0000000a && lcode[4]&0x00008000) {
				outStream << "[DDU C-code L1A error detected for external FIFO(s) ";
				for (int iReg = 0; iReg < 4; iReg++) {
					if ((lcode[3] >> 20) & (1<<iReg)) outStream << std::dec << iReg << " ";
				}
				out.push_back(outStream.str() + "]");
				outStream.str("");

				solved = true;
				
			} else if ((lcode[0]&0x9fc00fff) == 0x0000020a) {
				out.push_back("[DMB L1A mismatch (not from InFPGA)]");
				
				solved = true; // 2
				multiSolve = true;
				
			} else if ((lcode[0]&0x9fc00dff) == 0x0040000a) {
				outStream << "[DMB L1A mismatch error (from InFPGA) detected for external FIFO(s) ";
				for (int iReg = 0; iReg < 4; iReg++) {
					if ((lcode[3] >> 20) & (1<<iReg)) outStream << std::dec << iReg << " ";
				}
				out.push_back(outStream.str() + "]");
				outStream.str("");
				
				solved = true; // 2
				multiSolve = true;
				
			} else if (lcode[0]&0x00000002) {
				outStream << "[Likely caused by DMB L1A mismatch, other errors too for external FIFO(s) ";
				for (int iReg = 0; iReg < 4; iReg++) {
					if ((lcode[3] >> 20) & (1<<iReg)) outStream << std::dec << iReg << " ";
				}
				out.push_back(outStream.str() + "]");
				outStream.str("");
				
				solved = true; // 2
				multiSolve = true;
			}
			
			if ((lcode[0]&0xdf800dfd) == 0x40000008) out.push_back("[CFEB L1A mismatch]");
			else if (lcode[0]&0x40000008) out.push_back("[CFEB L1A Mismatch, other errors too]");  // other things wrong too
			if (lcode[0]&0x40000008) {
				solved = true; // 2
				multiSolve = true;
			}
			
			if ((DMBError&0x7fff) && multiSolve) {
				out.push_back("[Other errors]");
				solved = true;
			}

			if ((lcode[0]&0x9f810df5) == 0x00010000) out.push_back("[Trigger L1A mismatch]");
			else if (lcode[0]&0x00010000) out.push_back("[Trigger L1A mismatch, other errors too]");
			if (lcode[0]&0x00010000) {
				solved = true;
				
				if (lcode[2]&0x00001000) out.push_back("(ALCT L1A Mismatch)");
				else if (lcode[0]&0x00040000) out.push_back("(ALCT errors present)");
				

				if (lcode[2]&0x00000010) out.push_back("(TMB L1A Mismatch)");
				else if (lcode[0]&0x00080000) out.push_back("(TMB errors present)");
				
			}
		}
	}

	if (!solved) {
		out.push_back("Cause of error not clearly determined");
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


	return out;
}



std::map<std::string, std::string> emu::fed::DDUDebugger::KillFiber(long int stat)
{
	std::map<std::string, std::string> returnValues;

	if ((stat&0x00018000) == 0x8000) returnValues["ALCT checking is disabled"] = "blue";
	if ((stat&0x00028000) == 0x8000) returnValues["TMB checking is disabled"] = "blue";
	//if ((stat&0x00008000)>0&&(stat&0x00030000)<0x00030000) *out << cgicc::br() ;
	if ((stat&0x00048000) == 0x8000) returnValues["CFEB DAV/LCT/MOVLP/L1A checks disabled"] = "blue";
	if ((stat&0x00088000) == 0x8000) returnValues["Some DMB checks disabled for SP/TF compatibility"] = "blue";
	if ((stat&0x00008000) == 0) returnValues["All checks are Enabled"] = "green";

	return returnValues;
}



std::map<std::string, std::string> emu::fed::DDUDebugger::InFPGAStat(enum DEVTYPE dt, unsigned long int stat)
{
	std::map<std::string, std::string> returnValues;

	unsigned int fiberOffset = (dt == INFPGA0 ? 0 : 8);
	//*out << "<blockquote><font size=-1 color=red face=arial>";
	if (stat&0xF0000000) {
		if (0x80000000&stat) returnValues["DLL2 Lock Error"] = "red";
		// if (0x80000000&stat) returnValues["DLL-2 Not Locked"] = "red";
		// if (0x40000000&stat) returnValues["DLL-1 Not Locked"] = "red";
		if (0x40000000&stat) returnValues["64-bit Filler was used"] = "red";
		if (0x20000000&stat) {
			std::stringstream fibers;
			fibers << (1 + fiberOffset * 2 / 8);
			returnValues["RdCtrl-"+fibers.str()+" Not Ready"] = "red";
		}
		if (0x10000000&stat) {
			std::stringstream fibers;
			fibers << (0 + fiberOffset * 2 / 8);
			returnValues["RdCtrl-"+fibers.str()+" Not Ready"] = "red";
		}
	}
	if (stat&0x0F000000) {
		if (0x08000000&stat) returnValues["NoLiveFiber 0 or 1"] = "blue";
		if (0x04000000&stat) returnValues["DLL Error occurred"] = "blue";
		if (0x02000000&stat) {
			std::stringstream fibers;
			fibers << (1 + fiberOffset * 2 / 8);
			returnValues["InRD"+fibers.str()+" DMB Warn"] = "none";
		}
		if (0x01000000&stat) {
			std::stringstream fibers;
			fibers << (0 + fiberOffset * 2 / 8);
			returnValues["InRD"+fibers.str()+" DMB Warn"] = "none";
		}
	}
	if (stat&0x00F00000) {
		std::stringstream registers;
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
			std::stringstream fibers;
			fibers << (4 + fiberOffset) << "-" << (7 + fiberOffset);
			returnValues["NoLiveFiber"+fibers.str()] = "none";
		}
	}
	if (stat&0x000F0000) {
		std::stringstream registers;
		registers << (0 + fiberOffset * 2 / 8);
		if (0x00080000&stat) returnValues["InRD"+registers.str()+" DMB Full"] = "blue";
		if (0x00040000&stat) returnValues["Mem/FIFO-InRD"+registers.str()+" Error"] = "red";
		if (0x00020000&stat) returnValues["MultL1A Error-InRD"+registers.str()] = "red";
		if (0x00010000&stat) {
			std::stringstream fibers;
			fibers << (0 + fiberOffset) << "-" << (3 + fiberOffset);
			returnValues["NoLiveFiber"+fibers.str()] = "none";
		}
	}
	// JRG, low-order 16-bit status (most serious errors):
	if (stat&0x0000F000) {
		if (0x00008000&stat) returnValues["Critical Error ** needs reset **"] = "error";
		if (0x00004000&stat) returnValues["Single Error, bad event"] = "orange";
		if (0x00002000&stat) returnValues["Single warning, possible data problem"] = "blue";
		if (0x00001000&stat) returnValues["Near Full Warning"] = "blue";
	}
	if (stat&0x00000F00) {
		if (0x00000800&stat) returnValues["RX Error occurred"] = "blue";
		if (0x00000400&stat) returnValues["DLL Error (recent)"] = "blue";
		if (0x00000200&stat) returnValues["SCA Full detected"] = "orange";
		if (0x00000100&stat) returnValues["Special Word voted-bit warning"] = "blue";
	}
	if (stat&0x000000F0) {
		if (0x00000080&stat) returnValues["Stuck Data occurred"] = "red";
		if (0x00000040&stat) returnValues["Timeout occurred"] = "red";
		if (0x00000020&stat) returnValues["Multiple voted-bit Errors"] = "red";
		if (0x00000010&stat) returnValues["Multiple Transmit Errors"] = "red";
	}
	if (stat&0x0000000F) {
		if (0x00000008&stat) returnValues["Mem/FIFO Full Error"] = "red";
		if (0x00000004&stat) returnValues["Fiber Error"] = "red";
		if (0x00000002&stat) returnValues["L1A Match Error"] = "orange";
		if (0x00000001&stat) returnValues["Not Ready Error"] = "red";
	}

	return returnValues;
}



std::map<std::string, std::string> emu::fed::DDUDebugger::FIFOStat(enum DEVTYPE dt, int stat)
{
	std::map<std::string, std::string> returnValues;

	//unsigned int fiberOffset = (dt == INFPGA0 ? 0 : 8);
	//*out << "<blockquote><font size=-1 color=black face=arial>";
	if (stat&0x00000055) {
		//returnValues["&nbsp InRD0 Status: &nbsp <font color=blue>";
		if (0x00000040&stat) returnValues["Ext.FIFO 3/4 Full"] = "blue";
		if (0x00000010&stat) returnValues["L1A FIFO Almost Full"] = "blue";
		if (0x00000004&stat) returnValues["MemCtrl Almost Full"] = "blue";
		//*out << "</font>";
		if (0x00000001&stat) returnValues["L1A FIFO Empty"] = "none";
		if (0x00000001&stat == 0) returnValues["L1A FIFO Not Empty"] = "none";
	}
	if (stat&0x000000AA) {
		//returnValues["&nbsp InRD1 Status: &nbsp <font color=blue>";
		if (0x00000080&stat) returnValues["Ext.FIFO 3/4 Full"] = "blue";
		if (0x00000020&stat) returnValues["L1A FIFO Almost Full"] = "blue";
		if (0x00000008&stat) returnValues["MemCtrl Almost Full"] = "blue";
		//*out << "</font>";
		if (0x000002&stat) returnValues["L1A FIFO Empty"] = "none";
		if (0x000002&stat == 0) returnValues["L1A FIFO Not Empty"] = "none";
	}

	return returnValues;
}



std::map<std::string, std::string> emu::fed::DDUDebugger::FIFOFull(enum DEVTYPE dt, int stat)
{
	std::map<std::string, std::string> returnValues;

	//unsigned int fiberOffset = (dt == INFPGA0 ? 0 : 8);
	//*out << "<blockquote><font size=-1 color=black face=arial>";
	if (stat&0x00000005) {
		//returnValues["&nbsp InRD0 Status: &nbsp <font color=red>";
		if (0x00000004&stat) returnValues["Ext.FIFO Full Occurred"] = "red";
		if (0x00000001&stat) returnValues["L1A FIFO Full Occurred"] = "red";
		//*out << "</font>" << cgicc::br();
	}
	if (stat&0x0000000A) {
		//returnValues["&nbsp InRD1 Status: &nbsp <font color=red>";
		if (0x00000008&stat) returnValues["Ext.FIFO Full Occurred"] = "red";
		if (0x00000002&stat) returnValues["L1A FIFO Full Occurred"] = "red";
		//*out << "</font>" << cgicc::br();
	}

	return returnValues;
}



std::map<std::string, std::string> emu::fed::DDUDebugger::CCodeStat(enum DEVTYPE dt, int stat)
{
	std::map<std::string, std::string> returnValues;

	//unsigned int fiberOffset = (dt == INFPGA0 ? 0 : 8);
	//out << "<blockquote><font size=-1 color=black face=arial>";
	if (stat&0x000000ff) {
		//*out << " &nbsp InRD0: &nbsp <font color=red>";
		if (0x000080&stat) returnValues["Critical Error ** needs reset **"] = "error";
		if (0x000040&stat) returnValues["Sync Error ** needs reset **"] = "error";
		if (0x000020&stat) returnValues["Single Error"] = "orange";
		if (0x000010&stat) returnValues["FIFO Overflow detected"] = "red";
		if (0x000008&stat) returnValues["Fiber Connection Error"] = "red";
		if (0x000004&stat) returnValues["Multi-Transmit Error"] = "red";
		if (0x000002&stat) returnValues["Stuck Data"] = "red";
		if (0x000001&stat) returnValues["Timeout"] = "red";
		//*out  << "</font>" << cgicc::br();
	}
	if (stat&0x0000ff00) {
		//*out << " &nbsp InRD1 &nbsp <font color=red>";
		if (0x0008000&stat) returnValues["Critical Error ** needs reset **"] = "error";
		if (0x00004000&stat) returnValues["Sync Error ** needs reset **"] = "error";
		if (0x00002000&stat) returnValues["Single Error"] = "orange";
		if (0x00001000&stat) returnValues["FIFO Overflow detected"] = "red";
		if (0x00000800&stat) returnValues["Fiber Connection Error"] = "red";
		if (0x00000400&stat) returnValues["Multi-Transmit Error"] = "red";
		if (0x00000200&stat) returnValues["Stuck Data"] = "red";
		if (0x00000100&stat) returnValues["Timeout"] = "red";
		//*out  << "</font>" << cgicc::br();
	}

	return returnValues;
}




std::map<std::string, std::string> emu::fed::DDUDebugger::FiberDiagnostics(enum DEVTYPE dt, int reg, unsigned long int stat)
{
	std::map<std::string, std::string> returnValues;

	unsigned int fiberOffset = (dt == INFPGA0 ? 0 : 8);
	if (0x1f000000&stat) {
		std::stringstream fiber;
		fiber << "Fiber " << (3 + fiberOffset + 4*reg) << ": " << ((0x1f000000&stat) >> 24);
		returnValues[fiber.str()] = "none";
	}
	if (0x001f0000&stat) {
		std::stringstream fiber;
		fiber << "Fiber " << (2 + fiberOffset + 4*reg) << ": " << ((0x001f0000&stat) >> 16);
		returnValues[fiber.str()] = "none";
	}
	if (0x00001f00&stat) {
		std::stringstream fiber;
		fiber << "Fiber " << (1 + fiberOffset + 4*reg) << ": " << ((0x00001f00&stat) >> 8);
		returnValues[fiber.str()] = "none";
	}
	if (0x0000001f&stat) {
		std::stringstream fiber;
		fiber << "Fiber " << (0 + fiberOffset + 4*reg) << ": " << ((0x0000001f&stat) >> 0);
		returnValues[fiber.str()] = "none";
	}

	return returnValues;
}





std::map<std::string, std::string> emu::fed::DDUDebugger::WriteMemoryActive(enum DEVTYPE dt, int iFiber, int stat)
{
	std::map<std::string, std::string> returnValues;

	unsigned int fiberOffset = (dt == INFPGA0 ? 0 : 8);
	int fiber0 = stat & 0x1f;
	int fiber1 = (stat >> 5) & 0x1f;
	if (fiber0 != 0x1f) {
		std::stringstream fiber;
		fiber << "Fiber " << (0 + iFiber*2 + fiberOffset) << ": " << fiber0;
		returnValues[fiber.str()] = "none";
	} else {
		std::stringstream fiber;
		fiber << "Fiber " << (0 + iFiber*2 + fiberOffset) << " is inactive";
		returnValues[fiber.str()] = "none";
	}
	if (fiber1 != 0x1f) {
		std::stringstream fiber;
		fiber << "Fiber " << (1 + iFiber*2 + fiberOffset) << ": " << fiber1;
		returnValues[fiber.str()] = "none";
	} else {
		std::stringstream fiber;
		fiber << "Fiber " << (1 + iFiber*2 + fiberOffset) << " is inactive";
		returnValues[fiber.str()] = "none";
	}

	return returnValues;
}



std::vector<std::string> emu::fed::DDUDebugger::INFPGADebugTrap(std::vector<uint16_t> lcode, enum DEVTYPE dt)
{
	std::vector<std::string> out;
	std::ostringstream outStream;

	// First, spit out the full status.

	std::string debugNames[12] = {
		"lf-full",
		"mem-avail",
		"c-code",
		"end-to",
		"start-to",
		"faf/nrdy",
		"l1-err",
		"dmb-warn",
		"32-bit empty-0m high",
		"32-bit empty-0m low",
		"32-bit stat high",
		"32-bit stat low"
	};

	// Pop out the decoded register.
	for (unsigned int iBits = 0; iBits < 12; iBits++) {
		//int lcodeBits = 5 - (iBits/2);
		//outStream << debugNames[iBits] << ": " << std::setw(4) << std::setfill('0') << std::hex << (iBits % 2 ? ((0xffff0000&lcode[lcodeBits]) >> 16) : (0xffff&lcode[lcodeBits]));
		outStream << debugNames[iBits] << ": " << std::setw(4) << std::setfill('0') << std::hex << lcode[11 - iBits];
		out.push_back(outStream.str());
		outStream.str("");
	}

/*
	unsigned long int lcode[10];
	int i;
	char buf[100], buf1[100], buf2[100], buf3[100], buf4[100];
	char cbuf1[20], cbuf2[20], cbuf3[20], cbuf4[20];
	char sred[20] = "<font color=red>";
	char syel[20] = "<font color=orange>";
	char sblu[20] = "<font color=blue>";
	char sgrn[20] = "<font color=green>";
	char snul[20] = "</font>";
	sprintf(buf1, " ");
	sprintf(buf2, " ");
	sprintf(buf3, " ");
	sprintf(buf4, " ");
	sprintf(cbuf1, " ");
	sprintf(cbuf2, " ");
	sprintf(cbuf3, " ");
	sprintf(cbuf4, " ");
	lcode[0] = thisDDU->fpga_lcode[0];
	lcode[1] = thisDDU->fpga_lcode[1];
	lcode[2] = thisDDU->fpga_lcode[2];
	lcode[3] = thisDDU->fpga_lcode[3];
	lcode[4] = thisDDU->fpga_lcode[4];
	lcode[5] = thisDDU->fpga_lcode[5];

	sprintf(buf, "  192-bit DDU InFPGA Diagnostic Trap (24 bytes) \n");
	*out << buf << std::endl;

	i = 23;
	sprintf(buf, "                        LFfull MemAvail C-code End-TO");
	*out << buf << std::endl;
	sprintf(buf, "      rcv bytes %2d-%2d:", i, i - 7);
	sprintf(cbuf1, "%s", sgrn);
	if (0x000f8000&lcode[5] < 3)sprintf(cbuf1, "%s", sblu);
	if (0x000f8000&lcode[5] < 2)sprintf(cbuf1, "%s", syel);
	if ((0x000f8000&lcode[5] < 1) || (0xfff00000&lcode[5]))sprintf(cbuf1, "%s", sred);
	sprintf(buf1, "%s   %04lx%s", cbuf1, (0xffff0000&lcode[5]) >> 16, snul);
	sprintf(cbuf2, "%s", sgrn);
	if ((0x001f&lcode[5] < 3) || (0x03e0&lcode[5] < 3) || (0x7c00&lcode[5] < 3) || (0x000f8000&lcode[5] < 3))sprintf(cbuf2, "%s", sblu);
	if ((0x001f&lcode[5] < 2) || (0x03e0&lcode[5] < 2) || (0x7c00&lcode[5] < 2) || (0x000f8000&lcode[5] < 2))sprintf(cbuf2, "%s", syel);
	if ((0x001f&lcode[5] < 1) || (0x03e0&lcode[5] < 1) || (0x7c00&lcode[5] < 1) || (0x000f8000&lcode[5] < 1))sprintf(cbuf2, "%s", sred);
	if (0x000f8000&lcode[5] < 3) {
		sprintf(buf2, "%s    <blink>%04lx</blink>%s", cbuf2, 0xffff&lcode[5], snul);
	} else {
		sprintf(buf2, "%s    %04lx%s", cbuf2, 0xffff&lcode[5], snul);
	}
	sprintf(cbuf3, "%s", sgrn);
	if (0x20200000&lcode[4])sprintf(cbuf3, "%s", syel);
	if (0xdfdf0000&lcode[4])sprintf(cbuf3, "%s", sred);
	sprintf(buf3, "%s    %04lx%s", cbuf3, (0xffff0000&lcode[4]) >> 16, snul);
	sprintf(cbuf4, "%s", sgrn);
	if (0xffff&lcode[4])sprintf(cbuf4, "%s", sred);
	sprintf(buf4, "%s   %04lx%s", cbuf4, 0xffff&lcode[4], snul);
	*out << buf << buf1 << buf2 << buf3 << buf4 << std::endl;

	i = 15;
	sprintf(buf, "                      Start-TO FAF/Nrdy L1err  DMBwarn");
	*out << buf << std::endl;
	sprintf(buf, "      rcv bytes %2d-%2d:", i, i - 7);
	sprintf(cbuf1, "%s", sgrn);
	if (0xff000000&lcode[3])sprintf(cbuf1, "%s", sblu);
	if (0x00ff0000&lcode[3])sprintf(cbuf1, "%s", sred);
	sprintf(buf1, "%s   %04lx%s", cbuf1, (0xffff0000&lcode[3]) >> 16, snul);
	sprintf(cbuf2, "%s", sgrn);
	if (0xfc00&lcode[3])sprintf(cbuf2, "%s", sblu);
	sprintf(buf2, "%s    %04lx%s", cbuf2, 0xffff&lcode[3], snul);
	sprintf(cbuf3, "%s", sgrn);
	if (0x00ff0000&lcode[2])sprintf(cbuf3, "%s", syel);
	if (0xff000000&lcode[2])sprintf(cbuf3, "%s", sred);
	sprintf(buf3, "%s    %04lx%s", cbuf3, (0xffff0000&lcode[2]) >> 16, snul);
	sprintf(cbuf4, "%s", sgrn);
	if (0x00ff&lcode[2])sprintf(cbuf4, "%s", sblu);
	if (0xff00&lcode[2])sprintf(cbuf4, "%s", sred);
	sprintf(buf4, "%s   %04lx%s", cbuf4, 0xffff&lcode[2], snul);
	*out << buf << buf1 << buf2 << buf3 << buf4 << std::endl;

	i = 7;
	sprintf(buf, "                        32-bit-Empty0M  32-bit-status");
	*out << buf << std::endl;
	sprintf(buf, "      rcv bytes %2d-%2d:", i, i - 7);
	sprintf(cbuf1, "%s", sgrn);
	if (0xffff0000&lcode[1] == 0xf8000000)sprintf(cbuf1, "%s", sred);
	sprintf(buf1, "%s   %04lx%s", cbuf1, (0xffff0000&lcode[1]) >> 16, snul);
	sprintf(cbuf2, "%s", sgrn);
	if (0xffff&lcode[1] == 0xf800)sprintf(cbuf2, "%s", sred);
	sprintf(buf2, "%s    %04lx%s", cbuf2, 0xffff&lcode[1], snul);
	sprintf(cbuf3, "%s", sgrn);
	if (0x0c080000&lcode[0])sprintf(cbuf3, "%s", sblu);
	if (0xf0660000&lcode[0])sprintf(cbuf3, "%s", sred);
	sprintf(buf3, "%s    %04lx%s", cbuf3, (0xffff0000&lcode[0]) >> 16, snul);
	sprintf(cbuf4, "%s", sgrn);
	if (0x2d00&lcode[0])sprintf(cbuf4, "%s", sblu);
	if (0x4202&lcode[0])sprintf(cbuf4, "%s", syel);
	if (0x80fd&lcode[0])sprintf(cbuf4, "%s", sred);
	sprintf(buf4, "%s   %04lx%s", cbuf4, 0xffff&lcode[0], snul);
	*out << buf << buf1 << buf2 << buf3 << buf4 << std::endl;

	return out->str();
	*/
	return out;
}



std::map<std::string, std::string> emu::fed::DDUDebugger::ParallelStat(int stat)
{
	std::map<std::string, std::string> returnValues;

	if (stat&0x0080) returnValues["VME DLL-2 Not Locked"] = "blue";
	if (stat&0x0040) returnValues["VME DLL-1 Not Locked"] = "blue";
	if (stat&0x8000) returnValues["VME FPGA detects a problem"] = "red";
	if (stat&0x4000) returnValues["VME FPGA has a clock problem"] = "red";
	if (stat&0x2000) returnValues["VME FPGA is Not Ready"] = "orange";
	if (stat&0x1000) returnValues["DDU is Not Ready"] = "orange";

	return returnValues;
}



std::map<std::string, std::string> emu::fed::DDUDebugger::FMMReg(int stat)
{
	std::map<std::string, std::string> returnValues;

	if (stat == 4) returnValues["BUSY"] = "orange";
	else if (stat == 1) returnValues["WARNING, Near Full"] = "blue";
	else if (stat == 8) returnValues["READY"] = "green";
	else if (stat == 2) returnValues["Lost Sync"] = "red";
	else if (stat == 0xC) returnValues["ERROR"] = "error";
	else returnValues["UNDEFINED"] = "error";

	return returnValues;
}



std::map<std::string, std::string> emu::fed::DDUDebugger::GbEPrescale(int stat)
{
	std::map<std::string, std::string> returnValues;

	int reg0 = stat & 0xF;
	int reg1 = (stat & 0xF0) >> 4;
	int reg2 = (stat & 0xF00) >> 8;
	int reg3 = (stat & 0xF000) >> 12;

	if (reg0 == reg2 && reg1 == reg3 && reg0 + reg1 == 0xF) {
		if ((0x7&stat) == 0x7) returnValues["Transmitting never"] = "none";
		else {
			unsigned int prescale = 1 << reg0;
			std::stringstream prescaleText;
			prescaleText << "1:" << prescale;
			returnValues["Transmitting "+prescaleText.str()+" events"] = "green";
		}
		if (0x8&stat) returnValues["Ignoring DCC/S-Link Wait"] = "orange";
	} else {
		returnValues["Transmitting never"] = "none";
	}

	return returnValues;
}



std::map<std::string, std::string> emu::fed::DDUDebugger::FakeL1Reg(int stat)
{
	std::map<std::string, std::string> returnValues;

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



std::map<std::string, std::string> emu::fed::DDUDebugger::F0EReg(int stat)
{
	std::map<std::string, std::string> returnValues;

	if ((stat & 0xFFF0) == 0xFED0) returnValues["FMM error reporting disabled"] = "orange";
	else returnValues["FMM error reporting enabled"] = "green";

	return returnValues;
}


/*
std::map<std::string, std::string> emu::fed::DDUDebugger::FIFOStat(int stat)
{
	std::map<std::string, std::string> returnValues;



	return returnValues;
}
*/
