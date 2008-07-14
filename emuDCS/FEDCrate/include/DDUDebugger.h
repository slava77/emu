#ifndef DDUDebugger_h
#define DDUDebugger_h

using namespace std;
#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include "DDU.h"

/** A class that reads particular variables from the DDU and returns debugging
*	information.  Everything is returned as a vector of pairs, the 2nd element
*	being the class of the particular debug message
*	(@sa LocalEmuApplication::CSS) and the 1st is the actual debug message.
*
*	@author Phillip Killewald
*	@author Jason Gilmore
**/
class DDUDebugger
{
public:
	/** Does nothing **/
	DDUDebugger();
	/** Does nothing **/
	~DDUDebugger();

	/** Reads from method @sa DDU::readFPGAStat(DDUFPGA) **/
	std::map<string, string> DDUFPGAStat(unsigned long int stat);

	/** Reads from method @sa DDU::readOutputStat **/
	std::map<string, string> OutputStat(int stat);

	/** Reads from method @sa DDU::readEBReg(1) **/
	std::map<string, string> EBReg1(int stat);

	/** Reads from method @sa DDU::readEBReg(2) **/
	std::map<string, string> EBReg2(int stat);

	/** Reads from method @sa DDU::readEBReg(3) **/
	std::map<string, string> EBReg3(int stat);

	/** Reads from method @sa DDU::checkFIFO(2) **/
	std::map<string, string> FIFO2(int stat);

	/** Reads from method @sa DDU::readFFError() **/
	std::map<string, string> FFError(int stat);

	/** Reads from method @sa DDU::readInCHistory() **/
	std::map<string, string> InCHistory(int stat);

	/** Reads from method @sa DDU::readWarnMon() **/
	std::map<string, string> WarnMon(int stat);

	/** Reads from method @sa DDU::ddu_fpgatrap() **/
	std::vector<std::string> ddu_fpgatrap(DDU *thisDDU);

	/** Reads from method @sa DDU::readKillFiber() **/
	std::map<string, string> KillFiber(long int stat);

	/** Reads from method @sa DDU::readFPGAStat(INFPGAX) **/
	std::map<string, string> InFPGAStat(enum DEVTYPE dt, unsigned long int stat);

	/** Reads from method @sa DDU::readFIFOStat(INFPGAX) **/
	std::map<string, string> FIFOStat(enum DEVTYPE dt, int stat);

	/** Reads from method @sa DDU::readFIFOFull(INFPGAX) **/
	std::map<string, string> FIFOFull(enum DEVTYPE dt, int stat);

	/** Reads from method @sa DDU::readCCodeStat(INFPGAX) **/
	std::map<string, string> CCodeStat(enum DEVTYPE dt, int stat);

	/** Reads from method @sa DDU::readFiberDiagnostics(INFPGAX,Y) **/
	std::map<string, string> FiberDiagnostics(enum DEVTYPE dt, int reg, unsigned long int stat);

	/** Reads from method @sa DDU::readWriteMemoryActive(INFPGAX) **/
	std::map<string, string> WriteMemoryActive(enum DEVTYPE dt, int iFiber, int stat);

	/** Reads from method @sa DDU::infpga_trap() **/
	string infpga_trap(DDU *thisDDU, enum DEVTYPE dt);

	/** Reads from method @sa DDU::readParallelStat() **/
	std::map<string, string> ParallelStat(int stat);

	/** Reads bits 8-11 from method @sa DDU::readParallelStat()
	*	(not readFMMReg?)
	**/
	std::map<string, string> FMMReg(int stat);

	/** Reads from method @sa DDU::readGbEPrescale() **/
	std::map<string, string> GbEPrescale(int stat);

	/** Reads from method @sa DDU::readFakeL1Reg() **/
	std::map<string, string> FakeL1Reg(int stat);

	/** Reads from method @sa DDU::readFMMReg() **/
	std::map<string, string> F0EReg(int stat);

private:

};

#endif
