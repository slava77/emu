/*****************************************************************************\
* $Id: DDUDebugger.cc,v 1.15 2012/08/27 14:45:11 cvuosalo Exp $
\*****************************************************************************/
#include "emu/fed/DDUDebugger.h"

#include <iostream>
#include <iomanip>
#include <sstream>

#include "emu/fed/DDU.h"
#include "emu/fed/Fiber.h"



std::map<std::string, std::string> emu::fed::DDUDebugger::FPGAStatus(const enum DEVTYPE &dev, const uint32_t &stat)
{
	std::map<std::string, std::string> returnMe;

	if (dev == DDUFPGA) {
		if (stat&0x0000F000) {
			if (stat&0xF0000000) {
				if (0x80000000&stat) returnMe["DMB LCT/DAV/MOVLP Mismatch"] = "red";
				if (0x40000000&stat) returnMe["CFEB L1A Mismatch"] = "red";
				if (0x20000000&stat) returnMe["DDUsawNoGoodDMB-CRCs"] = "blue";
				if (0x10000000&stat) returnMe["CFEB Count Mismatch"] = "red";
			}
			if (stat&0x0F000000) {
				if (0x08000000&stat) returnMe["FirstDat Error"] = "red";
				if (0x04000000&stat) returnMe["L1A-FIFO Full occurred"] = "red";
				if (0x02000000&stat) returnMe["Data Stuck in FIFO occurred"] = "red";
				if (0x01000000&stat) returnMe["NoLiveFiber warning"] = "blue";
			}
			if (stat&0x00F00000) {
				if (0x00800000&stat) returnMe["Special-word voted-bit warning"] = "orange";
				if (0x00400000&stat) returnMe["InRDctrl Error"] = "red";
				if (0x00200000&stat) returnMe["DAQ Stop bit set"] = "blue";
				if (0x00100000&stat) returnMe["DAQ says Not Ready"] = "blue";
				if (0x00300000&stat == 0x00200000) returnMe["DAQ Applied Backpressure"] = "blue";
			}
			if (stat&0x000F0000) {
				if (0x00080000&stat) returnMe["TMB Error"] = "orange";
				if (0x00040000&stat) returnMe["ALCT Error"] = "orange";
				if (0x00020000&stat) returnMe["Trigger Wordcount Error"] = "orange";
				if (0x00010000&stat) returnMe["Trigger L1A Match Error"] = "orange";
			}
			// JRG, low-order 16-bit status (most serious errors)"
			if (stat&0x0000F000) {
				if (0x00008000&stat) returnMe["Critical Error ** needs reset **"] = "error";
				if (0x00004000&stat) returnMe["Single Error, bad event"] = "orange";
				if (0x00002000&stat) returnMe["Single warning, possible data problem"] = "blue";
				if (0x00001000&stat) returnMe["Near Full Warning"] = "blue";
			}
			if (stat&0x00000F00) {
				if (0x00000800&stat) returnMe["64-bit Alignment Error"] = "blue";
				if (0x00000400&stat) returnMe["DDU Control DLL Error (recent)"] = "blue";
				if (0x00000200&stat) returnMe["DMB Error in event"] = "orange";
				if (0x00000100&stat) returnMe["Lost In Event Error"] = "orange";
			}
			if (stat&0x000000F0) {
				if (0x00000080&stat) returnMe["Lost In Data Error occurred"] = "red";
				if (0x00000040&stat) returnMe["Timeout Error occurred"] = "red";
				if (0x00000020&stat) returnMe["Trigger CRC Error"] = "orange";
				if (0x00000010&stat) returnMe["Multiple Transmit Errors occurred"] = "red";
			}
			if (stat&0x0000000F) {
				if (0x00000008&stat) returnMe["Lost Sync occurred (FIFO Full or L1A Error)"] = "red";
				if (0x00000004&stat) returnMe["Fiber/FIFO Connection Error occurred"] = "red";
				if (0x00000002&stat) returnMe["Single L1A Mismatch"] = "orange";
				if (0x00000001&stat) returnMe["DMB or CFEB CRC Error"] = "orange";
			}
		}
		
	} else if (dev == INFPGA0 || dev == INFPGA1) {
		const std::string minFiber = (dev == INFPGA0 ? "0" : "8");
		const std::string minFiber2 = (dev == INFPGA0 ? "4" : "12");
		const std::string maxFiber = (dev == INFPGA0 ? "7" : "14");
		const std::string rdCtrl1 = (dev == INFPGA0 ? "0" : "2");
		const std::string rdCtrl2 = (dev == INFPGA0 ? "1" : "3");
		
		if (stat&0xF0000000) {
			if (0x80000000&stat) returnMe["DLL2 Lock Error"] = "red";
			if (0x40000000&stat) returnMe["64-bit Filler was used"] = "red";
			if (0x20000000&stat) {
				returnMe["RdCtrl-"+rdCtrl2+" Not Ready"] = "red";
			}
			if (0x10000000&stat) {
				returnMe["RdCtrl-"+rdCtrl1+" Not Ready"] = "red";
			}
		}
		if (stat&0x0F000000) {
			if (0x08000000&stat) returnMe["NoLiveFiber 0 or 1"] = "blue";
			if (0x04000000&stat) returnMe["DLL Error occurred"] = "blue";
			if (0x02000000&stat) {
				returnMe["InRD"+rdCtrl2+" DMB Warn"] = "none";
			}
			if (0x01000000&stat) {
				returnMe["InRD"+rdCtrl1+" DMB Warn"] = "none";
			}
		}
		if (stat&0x00F00000) {
			if (0x00800000&stat) {
				returnMe["InRD"+rdCtrl2+" DMB Full"] = "orange";
			}
			if (0x00400000&stat) {

				returnMe["Mem/FIFO-InRD"+rdCtrl2+" Error"] = "red";
			}
			if (0x00200000&stat) {
				returnMe["MultL1A Error-InRD"+rdCtrl2] = "red";
			}
			if (0x00100000&stat) {
				returnMe["NoLiveFiber"+minFiber2+"-"+maxFiber] = "none";
			}
		}
		if (stat&0x000F0000) {
			if (0x00080000&stat) returnMe["InRD"+rdCtrl1+" DMB Full"] = "orange";
			if (0x00040000&stat) returnMe["Mem/FIFO-InRD"+rdCtrl1+" Error"] = "red";
			if (0x00020000&stat) returnMe["MultL1A Error-InRD"+rdCtrl1] = "red";
			if (0x00010000&stat) returnMe["NoLiveFiber"+minFiber+"-"+minFiber2] = "none";
		}
		// JRG, low-order 16-bit status (most serious errors):
		if (stat&0x0000F000) {
			if (0x00008000&stat) returnMe["Critical Error ** needs reset **"] = "error";
			if (0x00004000&stat) returnMe["Single Error, bad event"] = "orange";
			if (0x00002000&stat) returnMe["Single warning, possible data problem"] = "blue";
			if (0x00001000&stat) returnMe["Near Full Warning"] = "blue";
		}
		if (stat&0x00000F00) {
			if (0x00000800&stat) returnMe["RX Error occurred"] = "blue";
			if (0x00000400&stat) returnMe["DLL Error (recent)"] = "blue";
			if (0x00000200&stat) returnMe["SCA Full detected"] = "orange";
			if (0x00000100&stat) returnMe["Special Word voted-bit warning"] = "blue";
		}
		if (stat&0x000000F0) {
			if (0x00000080&stat) returnMe["Stuck Data occurred"] = "red";
			if (0x00000040&stat) returnMe["Timeout occurred"] = "red";
			if (0x00000020&stat) returnMe["Multiple voted-bit Errors"] = "red";
			if (0x00000010&stat) returnMe["Multiple Transmit Errors"] = "red";
		}
		if (stat&0x0000000F) {
			if (0x00000008&stat) returnMe["Mem/FIFO Full Error"] = "red";
			if (0x00000004&stat) returnMe["Fiber Error"] = "red";
			if (0x00000002&stat) returnMe["L1A Match Error"] = "orange";
			if (0x00000001&stat) returnMe["Not Ready Error"] = "red";
		}
	} else {
		// FIXME Error here.
	}

	return returnMe;
}



std::map<std::string, std::string> emu::fed::DDUDebugger::OutputStatus(const uint16_t &stat)
{
	std::map<std::string, std::string> returnMe;

	if (stat & 0xD981) {
		if (0x1000 & stat) returnMe["DDU S-Link Not Present"] = "green";
		if (0x0100 & stat) returnMe["SPY/GbE Fiber Disconnected"] = "blue";
		else if (0x0200 & stat) returnMe["SPY/GbE FIFO Always Empty"] = "none";
		if (stat & 0xECEF) {
			if (stat & 0xE000) {
				if (0x8000 & stat) returnMe["DDU Buffer Overflow occurred"] = "none";
				if (0x4000 & stat) returnMe["DAQ (DCC/S-Link) Wait occurred"] = "blue";
				if (0x2000 & stat) returnMe["DDU S-Link Full occurred"] = "none";
				//if (0x00001000&stat) returnMe["DDU S-Link Never Ready"] = "none";
			}
			if (stat & 0x0E00) {
				if (0x0800 & stat && (0x0100 & stat) == 0) returnMe["DDU GbE Overflow occurred"] = "blue";
				if (0x0400 & stat && (0x0100 & stat) == 0) returnMe["GbE Transmit Limit occurred"] = "none";
				//if (0x00000200&stat&&(0x00000100&stat)==0) returnMe["GbE FIFO Always Empty &nbsp ";
				//if (0x00000100&stat) returnMe["<font color=blue>SPY/GbE Fiber Disconnect occurred</font>";
			}
			if (stat & 0x00F0) {
				if (0x0080 & stat) returnMe["DDU DAQ-Limited Overflow occurred (DCC/S-Link Wait)"] = "red";
				if (0x0040 & stat) returnMe["DAQ (DCC/S-Link) Wait"] = "blue";
				if (0x0020 & stat) returnMe["DDU S-Link Full/Stop"] = "none";
				if (0x0010 & stat && (0x1000 & stat) == 0) returnMe["DDU S-Link Not Ready"] = "red";
			}
			if (stat & 0x000F) {
				if (0x0008 & stat && (0x0100&stat) == 0) returnMe["GbE FIFO Full"] = "none";
				if (0x0004 & stat && (0x0100&stat) == 0) returnMe["DDU Skipped SPY Event (GbE data not sent)"] = "none";
				if (0x0002 & stat && (0x0100&stat) == 0) returnMe["GbE FIFO Not Empty"] = "none";
				if (0x0001 & stat) returnMe["DCC Link Not Ready"] = "blue";
			}
		}
	}

	return returnMe;
}



std::map<std::string, std::string> emu::fed::DDUDebugger::EBRegister(const uint8_t &reg, const uint16_t &stat)
{

	std::map<std::string, std::string> returnMe;
	
	if (!stat) return returnMe;
	
	switch (reg) {
	case 0:
		if (stat & 0xF000) {
			if (0x8000&stat) returnMe["DMB Timeout signal, ** needs reset **"] = "error";
			if (0x4000&stat) returnMe["Mult L1A Error occurred"] = "red";
			if (0x2000&stat) returnMe["L1A-FIFO Near Full Warning"] = "blue";
			if (0x1000&stat) returnMe["GbE FIFO Almost-Full"] = "none";
		}
		if (stat & 0x0F00) {
			if (0x0800 & stat) returnMe["Ext.FIFO Near Full Warning"] = "blue";
			if (0x0400 & stat) returnMe["InSingle Warning"] = "blue";
			if (0x0200 & stat) returnMe["CFEB-CRC not OK"] = "none";
			if (0x0100 & stat) returnMe["CFEB-CRC End Error"] = "orange";
		}
		if (stat & 0x00F0) {
			if (0x0080 & stat) returnMe["CFEB-CRC Count Error"] = "orange";
			if (0x0040 & stat) returnMe["DMB or CFEB CRC Error"] = "orange";
			if (0x0020 & stat) returnMe["Trigger Readout Error"] = "orange";
			if (0x0010 & stat) returnMe["Trigger Trail Done"] = "none";
		}
		if (stat & 0x000F) {
			if (0x0008 & stat) returnMe["Start Timeout"] = "red";
			if (0x0004 & stat) returnMe["End Timeout"] = "red";
			if (0x0002 & stat) returnMe["SP/TF Error in last event"] = "orange";
			if (0x0001 & stat) returnMe["SP/TF data detected in last event"] = "orange";
		}
		break;
	case 1:
		if (0x0020 & stat) returnMe["Empty CSC in Event flag"] = "green";
		if (stat & 0xF000) {
			if (0x8000 & stat) returnMe["Lost In Event Error"] = "orange";
			if (0x4000 & stat) returnMe["DMB Error in Event"] = "orange";
			if (0x2000 & stat) returnMe["Control DLL Error occured"] = "blue";
			if (0x1000 & stat) returnMe["2nd Header First flag"] = "orange";
		}
		if (stat & 0x0F00) {
			if (0x0800 & stat) returnMe["Early 2nd Trailer flag"] = "none";
			if (0x0400 & stat) returnMe["Extra 1st Trailer flag"] = "none";
			if (0x0200 & stat) returnMe["Extra 1st Header flag"] = "none";
			if (0x0100 & stat) returnMe["Extra 2nd Header flag"] = "none";
		}
		if (stat & 0x00D0) {
			if (0x0080 & stat) returnMe["SCA Full detected this Event"] = "orange";
			if (0x0040 & stat) returnMe["Probable DMB Full occurred"] = "blue";
			//if (0x0020&stat) returnMe["Empty Event flag"] = "green";
			if (0x0010 & stat) returnMe["Bad Control Word Error occurred"] = "red";
		}
		if (stat & 0x000F) {
			if (0x0008 & stat) returnMe["Missed Trigger Trailer Error"] = "orange";
			if (0x0004 & stat) returnMe["First Dat Error"] = "orange";
			if (0x0002 & stat) returnMe["Bad First Word"] = "orange";
			if (0x0001 & stat) returnMe["Confirmed DMB Full occured"] = "red";
		}
		break;
	case 2:
		if (stat & 0xF000) {
			if (0x8000 & stat) returnMe["Trigger Readout Error"] = "orange";
			if (0x4000 & stat) returnMe["ALCT Trailer Done"] = "none";
			if (0x2000 & stat) returnMe["2nd ALCT Trailer detected"] = "red";
			if (0x1000 & stat) returnMe["ALCT L1A mismatch error occurred"] = "none";
		}
		if (stat & 0x0F00) {
			if (0x0800 & stat) returnMe["ALCT CRC Error occurred"] = "none";
			if (0x0400 & stat) returnMe["ALCT Wordcount Error occurred"] = "none";
			if (0x0200 & stat) returnMe["Missing ALCT Trailer occurred"] = "none";
			if (0x0100 & stat) returnMe["ALCT Error occurred"] = "none";
		}
		if (stat & 0x00F0) {
			if (0x0080 & stat) returnMe["DMB Critical Error occurred"] = "none";
			if (0x0040 & stat) returnMe["TMB Trailer Done"] = "none";
			if (0x0020 & stat) returnMe["2nd TMB Trailer detected"] = "red";
			if (0x0010 & stat) returnMe["TMB L1A mismatch error occurred"] = "none";
		}
		if (stat & 0x000F) {
			if (0x0008 & stat) returnMe["TMB CRC Error occurred"] = "none";
			if (0x0004 & stat) returnMe["TMB Word Count Error occurred"] = "none";
			if (0x0002 & stat) returnMe["Missing TMB Trailer occurred"] = "none";
			if (0x0001 & stat) returnMe["TMB Error occurred"] = "none";
		}
		break;
	default:
		// FIXME error
		break;
	}

	return returnMe;

}


unsigned int emu::fed::DDUDebugger::readErrors(DDU *const thisDDU, std::vector<std::string> &out, std::stringstream &outStream)
{
	// Spit out the funky fiber information.
	const unsigned long int CSCStat = thisDDU->readCSCStatus();
	if (CSCStat & 0x7fff) {
		outStream << "FMM errors detected on fiber(s) ";
		for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
			if (CSCStat & (1<<iFiber)) {
				outStream << std::dec << iFiber << " (" << thisDDU->getFiber(iFiber)->getName() << ") ";
			}
		}
		out.push_back(outStream.str());
		outStream.str("");
	}
	if (CSCStat & 0x8000) {
		out.push_back("DDU is in an FMM error condition");
	}

	const unsigned int DMBError = thisDDU->readDMBError();
	if (DMBError & 0x7fff) {
		outStream << "DMB errors detected on fiber(s) ";
		for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
			if (DMBError & (1<<iFiber)) {
				outStream << std::dec << iFiber << " (" << thisDDU->getFiber(iFiber)->getName() << ") ";
			}
		}
		out.push_back(outStream.str());
		outStream.str("");
	}
	
	const unsigned int TMBError = thisDDU->readTMBError();
	if (TMBError & 0x7fff) {
		outStream << "TMB errors detected on fiber(s) ";
		for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
			if (TMBError & (1<<iFiber)) {
				outStream << std::dec << iFiber << " (" << thisDDU->getFiber(iFiber)->getName() << ") ";
			}
		}
		out.push_back(outStream.str());
		outStream.str("");
	}
	
	const unsigned int ALCTError = thisDDU->readALCTError();
	if (ALCTError & 0x7fff) {
		outStream << "ALCT errors detected on fiber(s) ";
		for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
			if (ALCTError & (1<<iFiber)) {
				outStream << std::dec << iFiber << " (" << thisDDU->getFiber(iFiber)->getName() << ") ";
			}
		}
		out.push_back(outStream.str());
		outStream.str("");
	}
	
	const unsigned int XmitError = thisDDU->readXmitError();
	if (XmitError & 0x7fff) {
		outStream << "Transmit errors detected on fiber(s) ";
		for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
			if (XmitError & (1<<iFiber)) {
				outStream << std::dec << iFiber << " (" << thisDDU->getFiber(iFiber)->getName() << ") ";
			}
		}
		out.push_back(outStream.str());
		outStream.str("");
	}
	return (DMBError);
}


std::vector <std::string> emu::fed::DDUDebugger::DDUDebugTrap(const std::vector<uint16_t> &lcode, DDU *thisDDU)
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
	bool noError = true;
	for (unsigned int iBits = 0; iBits < 12; iBits++) {
		if (lcode[11 - iBits]) noError = false;
		outStream << debugNames[iBits] << ": " << std::setw(4) << std::setfill('0') << std::hex << lcode[11 - iBits];
		out.push_back(outStream.str());
		outStream.str("");
	}
	
	// If nothing here is set, we can return instantly
	if (noError) {
		return out;
	}

	const unsigned int DMBError = readErrors(thisDDU, out, outStream);

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

	const unsigned long int erastat = (0x0000ffff & lcode[1]);

	// INFPGAs in order.
	enum DEVTYPE devType[2] = {
		INFPGA0,
		INFPGA1
	};
	std::string devName[2] = {
		"INFPGA0",
		"INFPGA1"
	};

	for (unsigned int iDev = 0; iDev < 2; iDev++) {
		
		inStat[iDev] = thisDDU->readFPGAStatus(devType[iDev]);
		
		if (inStat[iDev]&0x04000000) {          // DLL Error
			out.push_back(devName[iDev] + ": DLL error detected");
			if (inStat[iDev]&0x00000800) out.push_back(devName[iDev] + ": (Probable cause of Gt-Rx errors)");
			solved = true;
			
		} else if (inStat[iDev]&0x00000004) {     // Fiber Change
			const unsigned int fiberCheck = thisDDU->readFiberStatus(devType[iDev]);
			outStream << devName[iDev] << ": Fiber connection error detected on fiber(s) ";
			for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
				// INFPGA0 looks at fibers 0-7, INFPGA1 looks at 8-15
				const unsigned int realFiber = iFiber + iDev*8;
				if ((fiberCheck >> 8) & (1<<iFiber)) {
					outStream << std::dec << realFiber << " (" << thisDDU->getFiber(realFiber)->getName() << ") ";
				}
			}
			out.push_back(outStream.str());
			outStream.str("");

			if (inStat[iDev]&0x00000800) out.push_back(devName[iDev] + ": (Probable cause of Gt-Rx errors)");
			else if (inStat[iDev]&0x00000130) out.push_back( devName[iDev] + ": (Probable cause of SpecialWord/Xmit errors)");
			solved = true;
			
		} else if (inStat[iDev]&0x00000800) {   // GT-Rx Error
			const unsigned int fiberCheck = thisDDU->readRxError(devType[iDev]);
			outStream << devName[iDev] << ": GT-Rx error detected on fiber(s) ";
			for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
				// INFPGA0 looks at fibers 0-7, INFPGA1 looks at 8-15
				const unsigned int realFiber = iFiber + iDev*8;
				if ((fiberCheck >> 8) & (1<<iFiber)) {
					outStream << std::dec << realFiber << " (" << thisDDU->getFiber(realFiber)->getName() << ") ";
				}
			}
			out.push_back(outStream.str());
			outStream.str("");
			
			if (inStat[iDev]&0x00000130) out.push_back(devName[iDev] + ": (Probable cause of SpecialWord/Xmit errors)");
			solved = true;

		} else if (lcode[1]&0x00010000 && inStat[iDev]&0x00880000) {  // DMB-Full
			const unsigned int fiberCheck = thisDDU->readDMBWarning(devType[iDev]);
			out.push_back(devName[iDev] + ": ");
			outStream << devName[iDev] << ": Confirmed DMB full on fiber(s) ";
			for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
				// INFPGA0 looks at fibers 0-7, INFPGA1 looks at 8-15
				const unsigned int realFiber = iFiber + iDev*8;
				if ((fiberCheck >> 8) & (1<<iFiber)) {
					outStream << std::dec << realFiber << " (" << thisDDU->getFiber(realFiber)->getName() << ") ";
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
			for (unsigned int iFIFO=0; iFIFO < 4; iFIFO++) {
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
				const unsigned int fiberCheck = thisDDU->readTxError(devType[iDev]);
				outStream << devName[iDev] << ": 64-bit align error detected on fiber(s) ";
				for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
					// INFPGA0 looks at fibers 0-7, INFPGA1 looks at 8-15
					const unsigned int realFiber = iFiber + iDev*8;
					if ((fiberCheck >> 8) & (1<<iFiber)) {
						outStream << std::dec << realFiber << " (" << thisDDU->getFiber(realFiber)->getName() << ") ";
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
		for (unsigned int iDev = 0; iDev < 2; iDev++) {
			if (inTrapSet[iDev]) {  // got_i0trap;
				//      *out << "-debug> inside 6>" << std::endl;
				if (inTrap[iDev][0]&0x0040) {
					if (inTrap[iDev][6]&0x00ff) {
						outStream << devName[iDev] << ": Start timeout for fiber(s) ";
						for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
							// INFPGA0 looks at fibers 0-7, INFPGA1 looks at 8-15
							const unsigned int realFiber = iFiber + iDev*8;
							if ((inTrap[iDev][7]) & (1<<iFiber)) {
								outStream << std::dec << realFiber << " (" << thisDDU->getFiber(realFiber)->getName() << ") ";
							}
						}
						out.push_back(outStream.str());
						outStream.str("");
					} else if (inTrap[iDev][8]) {
						outStream << devName[iDev] << ": End timeout for fiber(s) ";
						for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
							// INFPGA0 looks at fibers 0-7, INFPGA1 looks at 8-15
							const unsigned int realFiber = iFiber + iDev*8;
							if (((inTrap[iDev][8] >> 8) | inTrap[iDev][8]) & (1<<iFiber)) {
								outStream << std::dec << realFiber << " (" << thisDDU->getFiber(realFiber)->getName() << ") ";
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
							const unsigned int realFiber = iFiber + iDev*8;
							if ((inTrap[iDev][4] >> 8) & (1<<iFiber)) {
								outStream << std::dec << realFiber << " (" << thisDDU->getFiber(realFiber)->getName() << ") ";
							}
						}
						out.push_back(outStream.str());
						outStream.str("");
						
						if (iFill & (1 << iDev)) out.push_back("(may have caused 64-bit align error for " + devName[iDev] + ")");
						solved = true;
					} else if (inTrap[iDev][0]&0x0010) {  // Multi-Xmit error
						const unsigned int fiberCheck = thisDDU->readTxError(devType[iDev]);
						outStream << devName[iDev] << ": Multiple SpecialWord bit errors for fiber(s) ";
						for (unsigned int iFiber = 0; iFiber < 8; iFiber++) {
							// INFPGA0 looks at fibers 0-7, INFPGA1 looks at 8-15
							const unsigned int realFiber = iFiber + iDev*8;
							if (fiberCheck & (1<<iFiber)) {
								outStream << std::dec << realFiber << " (" << thisDDU->getFiber(realFiber)->getName() << ") ";
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
							const unsigned int realFiber = iFiber + iDev*8;
							if ((inTrap[iDev][5]) & (1<<iFiber)) {
								outStream << std::dec << realFiber << " (" << thisDDU->getFiber(realFiber)->getName() << ") ";
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
				for (unsigned int iReg = 0; iReg < 4; iReg++) {
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
				for (unsigned int iReg = 0; iReg < 4; iReg++) {
					if ((lcode[3] >> 20) & (1<<iReg)) outStream << std::dec << iReg << " ";
				}
				out.push_back(outStream.str() + ")");
				outStream.str("");
			}
			if (lcode[4]&0x0f000000) {
				outStream << "(End-wait timeout for external FIFO(s) ";
				for (unsigned int iReg = 0; iReg < 4; iReg++) {
					if ((lcode[3] >> 24) & (1<<iReg)) outStream << std::dec << iReg << " ";
				}
				out.push_back(outStream.str() + ")");
				outStream.str("");
			}
			if (lcode[4]&0xf0000000) {
				outStream << "(End-busy timeout for external FIFO(s) ";
				for (unsigned int iReg = 0; iReg < 4; iReg++) {
					if ((lcode[3] >> 28) & (1<<iReg)) outStream << std::dec << iReg << " ";
				}
				out.push_back(outStream.str() + ")");
				outStream.str("");
			}
			
			solved = true;
		}
		if (lcode[0]&0x02000000) {  // DDUctrl StuckDat error
			outStream << "[Stuck data error detected for external FIFO(s) ";
			for (unsigned int iReg = 0; iReg < 4; iReg++) {
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
				for (unsigned int iReg = 0; iReg < 4; iReg++) {
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
				for (unsigned int iReg = 0; iReg < 4; iReg++) {
					if ((lcode[3] >> 20) & (1<<iReg)) outStream << std::dec << iReg << " ";
				}
				out.push_back(outStream.str() + "]");
				outStream.str("");
				
				solved = true; // 2
				multiSolve = true;
				
			} else if (lcode[0]&0x00000002) {
				outStream << "[Likely caused by DMB L1A mismatch, other errors too for external FIFO(s) ";
				for (unsigned int iReg = 0; iReg < 4; iReg++) {
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



std::map<std::string, std::string> emu::fed::DDUDebugger::KillFiber(const uint32_t &stat)
{
	std::map<std::string, std::string> returnValues;

	if ((stat & 0x00018000) == 0x8000) returnValues["ALCT checking is disabled"] = "blue";
	if ((stat & 0x00028000) == 0x8000) returnValues["TMB checking is disabled"] = "blue";
	if ((stat & 0x00048000) == 0x8000) returnValues["CFEB DAV/LCT/MOVLP/L1A checks disabled"] = "blue";
	if ((stat & 0x00088000) == 0x8000) returnValues["Some DMB checks disabled for SP/TF compatibility"] = "blue";
	if ((stat & 0x00008000) == 0) returnValues["All checks enabled (forced)"] = "green";

	return returnValues;
}



std::vector<std::string> emu::fed::DDUDebugger::INFPGADebugTrap(const enum DEVTYPE &dt, const std::vector<uint16_t> &lcode)
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



std::map<std::string, std::string> emu::fed::DDUDebugger::ParallelStatus(const uint16_t &stat)
{
	std::map<std::string, std::string> returnValues;
	returnValues.insert(RealFMM(stat & 0xf));

	if (stat & 0x0080) returnValues["VME DLL-2 Not Locked"] = "blue";
	if (stat & 0x0040) returnValues["VME DLL-1 Not Locked"] = "blue";
	if (stat & 0x8000) returnValues["VME FPGA detects a problem"] = "red";
	if (stat & 0x4000) returnValues["VME FPGA has a clock problem"] = "red";
	if (stat & 0x2000) returnValues["VME FPGA is Not Ready"] = "orange";
	if (stat & 0x1000) returnValues["DDU is Not Ready"] = "orange";

	return returnValues;
}



std::map<std::string, std::string> emu::fed::DDUDebugger::FMM(const uint16_t &stat)
{
	std::map<std::string, std::string> returnValues;
	returnValues.insert(RealFMM(stat & 0xf));
	
	if ((stat & 0xfff0) == 0xfed0) returnValues["FMM reporting "] = "green";
	else returnValues[""] = "green";

	return returnValues;
}



std::pair<std::string, std::string> emu::fed::DDUDebugger::RealFMM(const uint8_t &stat)
{
	if (stat == 0x1) return std::make_pair("Warning", "warning");
	else if (stat == 0x2) return std::make_pair("Out-of-Sync", "error");
	else if (stat == 0x4) return std::make_pair("Busy", "caution");
	else if (stat == 0x8) return std::make_pair("Ready", "ok");
	else if (stat == 0xc) return std::make_pair("Error", "error");
	else return std::make_pair("Undefined", "undefined");
}



std::map<std::string, std::string> emu::fed::DDUDebugger::GbEPrescale(const uint16_t &stat)
{
	std::map<std::string, std::string> returnValues;

	int prescaleMap[] = {1, 2, 4, 16, 128, 1024, 32768};

	int reg0 = stat & 0xF;
	int reg1 = (stat & 0xF0) >> 4;
	int reg2 = (stat & 0xF00) >> 8;
	int reg3 = (stat & 0xF000) >> 12;

	if (reg0 == reg2 && reg1 == reg3 && reg0 + reg1 == 0xF) {
		if (reg0 == 7) returnValues["Transmitting never"] = "none";
		else {
			std::stringstream prescaleText;
			prescaleText << "1:" << prescaleMap[reg0 & 7];
			returnValues["Transmitting "+prescaleText.str()+" events"] = "green";
		}
		if (reg0 & 8) returnValues["Ignoring DCC/S-Link Wait"] = "orange";
	} else {
		returnValues["Transmitting never"] = "none";
	}

	return returnValues;
}



std::map<std::string, std::string> emu::fed::DDUDebugger::FakeL1(const uint16_t &stat)
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



std::pair<std::string, std::string> emu::fed::DDUDebugger::Temperature(const float &temp, const enum DDU::TEMPSCALE &scale)
{
	float warnTemp = 30;
	float errorTemp = 40;
	std::string degreeText = " C";
	if (scale == DDU::FAHRENHEIT) {
		warnTemp = warnTemp * 9. / 5. + 32.;
		errorTemp = errorTemp * 9. / 5. + 32.;
		degreeText = " F";
	} else if (scale == DDU::KELVIN) {
		warnTemp = warnTemp + 273.15;
		errorTemp = errorTemp + 273.15;
		degreeText = " K";
	} else if (scale == DDU::RANKINE) {
		warnTemp = (warnTemp + 273.15) * 9. / 5.;
		errorTemp = (errorTemp +273.15) * 9. / 5.;
		degreeText = " Ra";
	} else if (scale == DDU::DELISLE) {
		warnTemp = (100. - warnTemp) * 3. / 2.;
		errorTemp = (100. - errorTemp) * 3. / 2.;
		degreeText = " D";
	} else if (scale == DDU::NEWTON) {
		warnTemp = warnTemp * 33. / 100.;
		errorTemp = errorTemp * 33. / 100.;
		degreeText = " N";
	} else if (scale == DDU::REAUMUR) {
		warnTemp = warnTemp * 4. / 5.;
		errorTemp = errorTemp * 4. / 5.;
		degreeText = " R&eacute;";
	} else if (scale == DDU::ROMER) {
		warnTemp = warnTemp * 21. / 40. + 7.5;
		errorTemp = errorTemp * 21. / 40. + 7.5;
		degreeText = " R&oslash;";
	}
	
	if ((scale == DDU::DELISLE && temp < errorTemp) || temp > errorTemp) {
		std::ostringstream response;
		response << "> " << errorTemp << degreeText;
		return std::make_pair(response.str(), "red");
	} else if ((scale == DDU::DELISLE && temp < warnTemp) || temp > warnTemp) {
		std::ostringstream response;
		response << "> " << warnTemp << degreeText;
		return std::make_pair(response.str(), "yellow");
	}

	return std::make_pair("OK", "green");
}



std::pair<std::string, std::string> emu::fed::DDUDebugger::Voltage(const uint8_t &sensor, const float &voltage)
{

	unsigned int targetVoltage = 0;
	if (sensor == 0) targetVoltage = 1500;
	else if (sensor == 1 || sensor == 2) targetVoltage = 2500;
	else if (sensor == 3) targetVoltage = 3300;

	if (voltage > 3500 || voltage < 0) {
		return std::make_pair("???", "blue");
	} else if (voltage > targetVoltage*1.05) {
		std::ostringstream targetText;
		targetText << "> " << targetVoltage*1.05 << " mV";
		return std::make_pair(targetText.str(), "red");
	} else if (voltage < targetVoltage*.95) {
		std::ostringstream targetText;
		targetText << "< " << targetVoltage*.95 << " mV";
		return std::make_pair(targetText.str(), "red");
	} else if (voltage > targetVoltage*1.025) {
		std::ostringstream targetText;
		targetText << "> " << targetVoltage*1.025 << " mV";
		return std::make_pair(targetText.str(), "yellow");
	} else if (voltage < targetVoltage*.975) {
		std::ostringstream targetText;
		targetText << "< " << targetVoltage*.975 << " mV";
		return std::make_pair(targetText.str(), "yellow");
	}
	
	return std::make_pair("OK", "green");

}



std::map<std::string, std::string> emu::fed::DDUDebugger::DebugFiber(DDU *ddu, const uint16_t &value, const std::string &className)
{
	std::map<std::string, std::string> returnMe;

	for (int8_t iBit = 0; iBit < 15; iBit++) {
		if (value & (1 << iBit)) {
			returnMe[ddu->getFiber(iBit)->getName()] = className;
		}
	}
	if (value & 0x8000) {
		returnMe["The DDU itself"] = className;
	}
	
	return returnMe;
}



std::map<std::string, std::string> emu::fed::DDUDebugger::InRDStatus(const uint16_t &stat)
{
	std::map<std::string, std::string> returnMe;
	
	if (stat & 0xf000) {
		returnMe["Hard error: " + decodeInRD((stat >> 12) & 0xf)] = "red";
	}
	if (stat & 0x0f00) {
		returnMe["Sync error: " + decodeInRD((stat >> 8) & 0xf)] = "red";
	}
	if (stat & 0x00f0) {
		returnMe["Single event error: " + decodeInRD((stat >> 4) & 0xf)] = "red";
	}
	if (stat & 0x000f) {
		returnMe["Timeout error: " + decodeInRD(stat & 0xf)] = "red";
	}
	
	return returnMe;
}



std::string emu::fed::DDUDebugger::decodeInRD(const uint8_t &stat)
{
	std::ostringstream returnMe;
	
	for (unsigned int iBit = 0; iBit < 4; iBit++) {
		unsigned int minFiber = iBit * 4;
		unsigned int maxFiber = iBit * 4 + 3;
		if (iBit == 3) maxFiber -= 1;
		returnMe << "fibers " << minFiber << "-" << maxFiber << " ";
	}
	
	return returnMe.str();
}



std::string emu::fed::DDUDebugger::decodeINFPGAFibers(const enum DEVTYPE &dev, const uint8_t &stat, DDU *ddu)
{
	
	std::ostringstream returnMe;
	
	unsigned int minFiber = (dev == INFPGA0) ? 0 : 8;
	unsigned int maxFiber = (dev == INFPGA0) ? 7 : 14;
	
	unsigned int iBit = 0;
	for (unsigned int iFiber = minFiber; iFiber < maxFiber; ++iFiber, ++iBit) {
		if (stat && (1 << iBit)) returnMe << ddu->getFiber(iFiber)->getName() << " ";
	}
	
	return returnMe.str();
}



std::map<std::string, std::string> emu::fed::DDUDebugger::InCHistory(const uint16_t &stat)
{
	std::map<std::string, std::string> returnMe;
	
	if (stat & 0x000f) {
		returnMe["Multiple transmit errors: " + decodeInRD(stat & 0xf)] = "red";
	}
	
	if (0x08000 & stat) returnMe["InRD End C-Code Error occurred"] = "red";
	if (0x04000 & stat) returnMe["InRD Begin C-Code Error occurred"] =  "red";
	if (0x02000 & stat) returnMe["InRD multiple L1A Mismatches occurred"] = "red";
	else if (0x01000 & stat) returnMe["InRD Single L1A Mismatch occurred"] = "yellow";
	if (0x0800 & stat) returnMe["InRD Hard Error occurred"] = "red";
	if (0x0400 & stat) returnMe["InRD Sync Error occurred"] = "red";
	if (0x0200 & stat) returnMe["InRD Single Error occurred"] = "yellow";
	if (0x0100 & stat) returnMe["InRD Mem/FIFO Error occurred"] = "red";
	if (0x0080 & stat) returnMe["InRD Fiber Connection Error occurred"] = "red";
	if (0x0040 & stat) returnMe["InRD Multiple Transmit Errors occurred"] = "red";
	if (0x0020 & stat) returnMe["InRD Stuck Data Error occurred"] = "red";
	if (0x0010 & stat) returnMe["InRD Timeout Error occurred"] = "red";
	
	return returnMe;
}



std::map<std::string, std::string> emu::fed::DDUDebugger::FFError(const uint16_t &stat)
{
	std::map<std::string, std::string> returnMe;
	
	if (stat & 0x3b00) {
		returnMe["Raw Ext. FIFO empty: " + decodeInRD((stat >> 10) & 0xf)] = "none";
	}
	if (stat & 0x00f0) {
		returnMe["InRD FIFO full: " + decodeInRD((stat >> 4) & 0xf)] = "red";
	}
	if (stat & 0x000f) {
		returnMe["Ext. FIFO full: " + decodeInRD(stat & 0xf)] = "red";
	}
	
	if (0x4000 & stat) returnMe["L1A FIFO Empty"] = "none";
	if (0x4000 & stat == 0) returnMe["L1A FIFO Not Empty"] = "none";
	if (0x0200 & stat) returnMe["GbE FIFO Full occurred"] = "yellow";
	if (0x0100 & stat) returnMe["L1A FIFO Full occurred"] = "red";
	
	return returnMe;
}



std::map<std::string, std::string> emu::fed::DDUDebugger::MaxTimeoutCount(const uint16_t &stat)
{

	std::map<std::string, std::string> returnMe;
	
	std::ostringstream sL1ATime;
	sL1ATime << "DDU L1A-to-start max processing time: " << ((stat & 0xff) * 400.0) << " ns";
	returnMe[sL1ATime.str()] = "none";
	
	std::ostringstream sTotalTime;
	sTotalTime << "DDU start-to-end max processing time: " << (((stat >> 8) & 0xff) * 6.4) << " μs";
	returnMe[sTotalTime.str()] = "none";
	
	return returnMe;

}



std::map<std::string, std::string> emu::fed::DDUDebugger::FIFOStatus(const uint8_t &fifo, const uint16_t &stat)
{
	return FIFOStatus(DDUFPGA, fifo, stat);
}



std::map<std::string, std::string> emu::fed::DDUDebugger::FIFOStatus(const enum DEVTYPE &dev, const uint8_t &fifo, const uint16_t &stat)
{
	std::map<std::string, std::string> returnMe;
	
	if (dev == DDUFPGA) {
		
		switch (fifo) {
		case 0:
			if (stat & 0xf000) {
				returnMe["Stuck data error: " + decodeInRD((stat >> 12) & 0xf)] = "red";
			}
			if (stat & 0x0f00) {
				returnMe["Fiber or FIFO connection error: " + decodeInRD((stat >> 8) & 0xf)] = "red";
			}
			if (stat & 0x00f0) {
				returnMe["L1A mismatch: " + decodeInRD((stat >> 4) & 0xf)] = "orange";
			}
			if (stat & 0x000f) {
				returnMe["InRDs with active fiber: " + decodeInRD(stat & 0xf)] = "none";
			}
			break;
			
		case 1:
			if (stat & 0x3b00) {
				returnMe["Active ext. FIFO empty: " + decodeInRD((stat >> 10) & 0xf)] = "none";
			}
			if (stat & 0x00f0) {
				returnMe["InRD near-full warning: " + decodeInRD((stat >> 4) & 0xf)] = "orange";
			}
			if (stat & 0x000f) {
				returnMe["Ext. FIFO almost full: " + decodeInRD(stat & 0xf)] = "blue";
			}

			if (stat & 0xb300) {

				if (0x0040 & stat) returnMe["L1A FIFO Empty"] = "none";
				if (0x0040 & stat == 0) returnMe["L1A FIFO Not Empty"] = "none";
				if (0x0080 & stat) returnMe["DDU C-code L1A error"] = "blue";
				if (0x0002 & stat) returnMe["GbE FIFO Almost-Full occurred"] = "none";
				if (0x0001 & stat) returnMe["L1A FIFO Almost-Full occurred"] = "blue";

			}
			break;
			
		case 2:
			if (stat & 0xf000) {
				returnMe["Timeout-End/Busy: " + decodeInRD((stat >> 12) & 0xf)] = "red";
			}
			if (stat & 0x0f00) {
				returnMe["Timeout-End/Wait: " + decodeInRD((stat >> 8) & 0xf)] = "red";
			}
			if (stat & 0x00f0) {
				returnMe["Timeout-Start: " + decodeInRD((stat >> 4) & 0xf)] = "red";
			}
			if (stat & 0x000f) {
				returnMe["Lost-In-Data error: " + decodeInRD(stat & 0xf)] = "red";
			}
			break;
			
		default:
			// FIXME error
			break;
		}
		
	} else {
		// FIXME error
	}
	
	return returnMe;

}



std::map<std::string, std::string> emu::fed::DDUDebugger::FIFOStatus(const enum DEVTYPE &dev, const uint16_t &stat, DDU *ddu)
{
	std::map<std::string, std::string> returnMe;
	
	if (dev == INFPGA0 || dev == INFPGA1) {
		
		if (stat & 0xff00) {
			returnMe["Input buffer empty: " + decodeINFPGAFibers(INFPGA0, (stat >> 8), ddu)] = "none";
		}
		
		if (stat & 0x0055) {
			std::string fifoName = (dev == INFPGA0 ? "1" : "3");
			if (0x0040 & stat) returnMe["Ext.FIFO " + fifoName + " Full"] = "blue";
			if (0x0010 & stat) returnMe["L1A FIFO " + fifoName + " Almost Full"] = "blue";
			if (0x0004 & stat) returnMe["MemCtrl " + fifoName + " Almost Full"] = "blue";
			if (0x0001 & stat) returnMe["L1A FIFO " + fifoName + " Empty"] = "none";
			if (0x0001 & stat == 0) returnMe["L1A FIFO " + fifoName + " Not Empty"] = "none";
		}
		if (stat & 0x00AA) {
			std::string fifoName = (dev == INFPGA0 ? "2" : "4");
			if (0x0080 & stat) returnMe["Ext.FIFO " + fifoName + " Full"] = "blue";
			if (0x0020 & stat) returnMe["L1A FIFO " + fifoName + " Almost Full"] = "blue";
			if (0x0008 & stat) returnMe["MemCtrl " + fifoName + " Almost Full"] = "blue";
			if (0x0002 & stat) returnMe["L1A FIFO " + fifoName + " Empty"] = "none";
			if (0x0002 & stat == 0) returnMe["L1A FIFO " + fifoName + " Not Empty"] = "none";
		}
	} else {
		// FIXME error
	}
	
	return returnMe;
}



std::pair<std::string, std::string> emu::fed::DDUDebugger::SerialStatus(const uint8_t &stat)
{
	
	if (stat & 0x80) return std::make_pair("Error", "red");
	else if (stat & 0x0c) return std::make_pair("Warning", "orange");

	else return std::make_pair("", "");
}



std::map<std::string, std::string> emu::fed::DDUDebugger::FiberStatus(const enum DEVTYPE &dev, const uint16_t &stat, DDU *ddu)
{
	std::map<std::string, std::string> returnMe;
	
	if (stat & 0xff00) {
		returnMe["Fiber connection error: " + decodeINFPGAFibers(dev, (stat >> 8), ddu)] = "red";
	}
	returnMe["Fiber link inactive: " + decodeINFPGAFibers(dev, ~stat, ddu)] = "none";
	
	return returnMe;
}



std::map<std::string, std::string> emu::fed::DDUDebugger::DMBSync(const enum DEVTYPE &dev, const uint16_t &stat, DDU *ddu)
{
	std::map<std::string, std::string> returnMe;
	
	if (stat & 0xff00) {
		returnMe["Stuck data: " + decodeINFPGAFibers(dev, (stat >> 8), ddu)] = "red";
	}
	if (stat & 0x00ff) {
		returnMe["L1A mismatch: " + decodeINFPGAFibers(dev, stat, ddu)] = "orange";
	}
	
	return returnMe;
}



std::map<std::string, std::string> emu::fed::DDUDebugger::FIFOFull(const enum DEVTYPE &dev, const uint16_t &stat, DDU *ddu)
{
	std::map<std::string, std::string> returnMe;

	if (stat & 0x00ff) {
		returnMe["Input buffer full history: " + decodeINFPGAFibers(dev, stat, ddu)] = "red";
	}
	
	if (stat & 0x0500) {
		std::string fifoName = (dev == INFPGA0 ? "1" : "3");
		if (0x0400 & stat) returnMe["Ext. FIFO " + fifoName + " Full Occurred"] = "red";
		if (0x0100 & stat) returnMe["L1A FIFO " + fifoName + " Full Occurred"] = "red";
	}
	if (stat & 0x0A00) {
		std::string fifoName = (dev == INFPGA0 ? "2" : "4");
		if (0x0800 & stat) returnMe["Ext. FIFO " + fifoName + " Full Occurred"] = "red";
		if (0x0200 & stat) returnMe["L1A FIFO " + fifoName + " Full Occurred"] = "red";
	}
	
	return returnMe;
}



std::map<std::string, std::string> emu::fed::DDUDebugger::RxError(const enum DEVTYPE &dev, const uint16_t &stat, DDU *ddu)
{
	std::map<std::string, std::string> returnMe;
	
	if (stat & 0xff00) {
		returnMe["GT-Rx error: " + decodeINFPGAFibers(dev, (stat >> 8), ddu)] = "red";
	}
	if (stat & 0x00ff) {
		returnMe["Timeout-start: " + decodeINFPGAFibers(dev, stat, ddu)] = "red";
	}
	
	return returnMe;
}



std::map<std::string, std::string> emu::fed::DDUDebugger::TxError(const enum DEVTYPE &dev, const uint16_t &stat, DDU *ddu)
{
	std::map<std::string, std::string> returnMe;
	
	if (stat & 0xff00) {
		returnMe["SCA-full history: " + decodeINFPGAFibers(dev, (stat >> 8), ddu)] = "red";
	}
	if (stat & 0x00ff) {
		returnMe["CSC transmit error: " + decodeINFPGAFibers(dev, stat, ddu)] = "red";
	}
	
	return returnMe;
}



std::map<std::string, std::string> emu::fed::DDUDebugger::Timeout(const enum DEVTYPE &dev, const uint16_t &stat, DDU *ddu)
{
	std::map<std::string, std::string> returnMe;
	
	if (stat & 0xff00) {
		returnMe["Timeout-end busy: " + decodeINFPGAFibers(dev, (stat >> 8), ddu)] = "red";
	}
	if (stat & 0x00ff) {
		returnMe["Timeout-end wait: " + decodeINFPGAFibers(dev, stat, ddu)] = "red";
	}
	
	return returnMe;
}



std::map<std::string, std::string> emu::fed::DDUDebugger::ActiveWriteMemory(const enum DEVTYPE &dev, const uint8_t &reg, const uint16_t &stat, DDU *ddu)
{
	
	std::map<std::string, std::string> returnMe;
	
	// Get the fibers we are dealing with
	const unsigned int fiber0 = ((dev == INFPGA0) ? 0 : 8) + reg * 2;
	const unsigned int fiber1 = fiber0 + 1;
	
	if (stat & 0x1f) {
		std::ostringstream status;
		status << ddu->getFiber(fiber0)->getName() << ": " << (stat & 0x1f) << " units";
		returnMe[status.str()] = "none";
	}
	if ((stat >> 5) & 0x1f) {
		std::ostringstream status;
		status << ddu->getFiber(fiber1)->getName() << ": " << ((stat >> 5) & 0x1f) << " units";
		returnMe[status.str()] = "none";
	}
	
	return returnMe;
	
}



std::map<std::string, std::string> emu::fed::DDUDebugger::AvailableMemory(const enum DEVTYPE &dev, const uint16_t &stat)
{

	std::map<std::string, std::string> returnMe;
	
	std::ostringstream status;
	status << "Fibers " << (dev == INFPGA0 ? 0 : 8) << "-" << (dev == INFPGA0 ? 3 : 11) << ": " << (stat & 0x1f) << " units";
	if (stat & 0x1f == 1) returnMe[status.str()] = "orange";
	else if (!(stat & 0x1f)) returnMe[status.str()] = "red";
	else returnMe[status.str()] = "none";

	status.str("");
	status << "Fibers " << (dev == INFPGA0 ? 4 : 12) << "-" << (dev == INFPGA0 ? 7 : 14) << ": " << ((stat >> 5) & 0x1f) << " units";
	if ((stat >> 5) & 0x1f == 1) returnMe[status.str()] = "orange";
	else if (!((stat >> 5) & 0x1f)) returnMe[status.str()] = "red";
	else returnMe[status.str()] = "none";
	
	return returnMe;
	
}



std::map<std::string, std::string> emu::fed::DDUDebugger::LostError(const enum DEVTYPE &dev, const uint16_t &stat, DDU *ddu)
{
	std::map<std::string, std::string> returnMe;
	
	if (stat & 0xff00) {
		returnMe["DDU lost-in-event: " + decodeINFPGAFibers(dev, (stat >> 8), ddu)] = "red";
	}
	if (stat & 0x00ff) {
		returnMe["DDU lost-in-data: " + decodeINFPGAFibers(dev, stat, ddu)] = "red";
	}
	
	return returnMe;
}


std::map<std::string, std::string> emu::fed::DDUDebugger::CCodeStatus(const enum DEVTYPE &dev, const uint16_t &stat)
{
	std::map<std::string, std::string> returnMe;

	if (stat & 0x00ff) {

		std::ostringstream fibers;
		fibers << "Fibers " << (dev == INFPGA0 ? "0" : "8") << "-" << (dev == INFPGA0 ? "3" : "11");
		if (0x0080 & stat) returnMe["Critical Error " + fibers.str() + " ** needs reset **"] = "error";
		if (0x0040 & stat) returnMe["Sync Error " + fibers.str() + " ** needs reset **"] = "error";
		if (0x0020 & stat) returnMe["Single Error " + fibers.str() + ""] = "orange";
		if (0x0010 & stat) returnMe["FIFO Overflow detected " + fibers.str() + ""] = "red";
		if (0x0008 & stat) returnMe["Fiber Connection Error " + fibers.str() + ""] = "red";
		if (0x0004 & stat) returnMe["Multi-Transmit Error " + fibers.str() + ""] = "red";
		if (0x0002 & stat) returnMe["Stuck Data " + fibers.str() + ""] = "red";
		if (0x0001 & stat) returnMe["Timeout " + fibers.str() + ""] = "red";
	}
	if (stat & 0xff00) {
		std::ostringstream fibers;
		fibers << "Fibers " << (dev == INFPGA0 ? "4" : "12") << "-" << (dev == INFPGA0 ? "7" : "14");
		if (0x8000 & stat) returnMe["Critical Error " + fibers.str() + " ** needs reset **"] = "error";
		if (0x4000 & stat) returnMe["Sync Error " + fibers.str() + " ** needs reset **"] = "error";
		if (0x2000 & stat) returnMe["Single Error " + fibers.str() + ""] = "orange";
		if (0x1000 & stat) returnMe["FIFO Overflow detected " + fibers.str() + ""] = "red";
		if (0x0800 & stat) returnMe["Fiber Connection Error " + fibers.str() + ""] = "red";
		if (0x0400 & stat) returnMe["Multi-Transmit Error " + fibers.str() + ""] = "red";
		if (0x0200 & stat) returnMe["Stuck Data " + fibers.str() + ""] = "red";
		if (0x0100 & stat) returnMe["Timeout " + fibers.str() + ""] = "red";
	}

	return returnMe;
}



std::map<std::string, std::string> emu::fed::DDUDebugger::DMBWarning(const enum DEVTYPE &dev, const uint16_t &stat, DDU *ddu)
{
	std::map<std::string, std::string> returnMe;
	
	if (stat & 0xff00) {
		returnMe["DMB error: " + decodeINFPGAFibers(dev, (stat >> 8), ddu)] = "red";
	}
	if (stat & 0x00ff) {
		returnMe["DMB warning: " + decodeINFPGAFibers(dev, stat, ddu)] = "orange";
	}
	
	return returnMe;
}



std::string emu::fed::DDUDebugger::FirmwareDecode(const uint32_t &code)
{
	std::ostringstream returnMe;
	
	if ((code & 0xff000f00) == 0xb0000a00) { // VMEPROM
		
		returnMe << "VMEPROM v" << ((code >> 12) & 0xff) << " rev " << (code & 0xff);
		
	} else if ((code & 0xfc00ff00) == 0xc000dd00) { // DDUPROM
		
		returnMe << "DDUPROM" << ((code >> 24) & 0xf) << " v" << ((code >> 16) & 0xff) << " (id " << (code & 0xff) << ")";
		
	} else if ((code & 0xfc000f00) == 0xd0000a00) { // INPROM
		
		returnMe << "INPROM" << ((code >> 24) & 0xf) << " v" << ((code >> 12) & 0xff) << " rev " << (code & 0xff);
		
	} else if ((code & 0xff000f00) == 0xcf000a00) { // DDUFPGA
		
		returnMe << "DDUFPGA v" << ((code >> 12) & 0xff) << " rev " << (code & 0xff);
	
	} else if ((code & 0xff000f00) == 0xdf000a00) { // INFPGA
		
		returnMe << "INFPGA v" << ((code >> 12) & 0xff) << " rev " << (code & 0xff);
		
	} else {
		
		returnMe << "Unknown version " << std::hex << std::showbase << code;
		
	}
	
	return returnMe.str();
}
